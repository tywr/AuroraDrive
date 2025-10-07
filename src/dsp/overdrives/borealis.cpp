#include "borealis.h"

#include <juce_dsp/juce_dsp.h>

void BorealisOverdrive::prepare(const juce::dsp::ProcessSpec& spec)
{
    juce::dsp::ProcessSpec oversampled_spec = spec;
    oversampled_spec.sampleRate *= 2.0;
    processSpec = oversampled_spec;

    oversampler2x.reset();
    oversampler2x.initProcessing(static_cast<size_t>(spec.maximumBlockSize));

    attack_shelf.prepare(oversampled_spec);
    float attack_shelf_gain = charToGain(character);
    smoothed_attack_shelf_gain = attack_shelf_gain;
    auto attack_shelf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            oversampled_spec.sampleRate, attack_shelf_freq, 0.5f,
            attack_shelf_gain
        );
    *attack_shelf.coefficients = *attack_shelf_coefficients;

    ff1_lpf.prepare(oversampled_spec);
    auto ff1_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            oversampled_spec.sampleRate, ff1_lpf_cutoff
        );
    *ff1_lpf.coefficients = *ff1_lpf_coefficients;

    ff2_hpf.prepare(oversampled_spec);
    auto ff2_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            oversampled_spec.sampleRate, smoothed_ff2_frequency
        );
    *ff2_hpf.coefficients = *ff2_hpf_coefficients;

    ff2_lpf.prepare(oversampled_spec);
    auto ff2_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            oversampled_spec.sampleRate, ff2_lpf_cutoff
        );
    *ff2_lpf.coefficients = *ff2_lpf_coefficients;

    pre_hpf.prepare(oversampled_spec);
    auto pre_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            oversampled_spec.sampleRate, pre_hpf_cutoff
        );
    *pre_hpf.coefficients = *pre_hpf_coefficients;

    pre_lpf.prepare(oversampled_spec);
    auto pre_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            oversampled_spec.sampleRate, pre_lpf_cutoff
        );
    *pre_lpf.coefficients = *pre_lpf_coefficients;

    post_lpf.prepare(oversampled_spec);
    auto post_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            oversampled_spec.sampleRate, post_lpf_cutoff, post_lpf_q
        );
    *post_lpf.coefficients = *post_lpf_coefficients;

    triode = Triode(oversampled_spec.sampleRate);
    diode = GermaniumDiode(oversampled_spec.sampleRate);
}

float BorealisOverdrive::driveToFrequency(float d)
{
    float t = d / 10.0f;
    float min_frequency = 153.0f;
    float max_frequency = 453.0f;
    return max_frequency - (max_frequency - min_frequency) * std::pow(t, 2.0f);
}

float BorealisOverdrive::charToGain(float c)
{
    float t = c / 10.0f;
    float min_gain = juce::Decibels::decibelsToGain(-12.0f);
    float max_gain = juce::Decibels::decibelsToGain(6.0f);
    return min_gain + (max_gain - min_gain) * std::pow(t, 3.0f);
}

void BorealisOverdrive::setCoefficients()
{
    float attack_shelf_gain = charToGain(character);
    if (std::abs(smoothed_attack_shelf_gain - attack_shelf_gain) >= 1e-2)
    {
        smoothed_attack_shelf_gain +=
            (attack_shelf_gain - smoothed_attack_shelf_gain) * smoothing_factor;
        auto attack_shelf_coefficients =
            juce::dsp::IIR::Coefficients<float>::makeHighShelf(
                processSpec.sampleRate, attack_shelf_freq, 0.5,
                smoothed_attack_shelf_gain
            );
        *attack_shelf.coefficients = *attack_shelf_coefficients;
    }

    float ff2_frequency = driveToFrequency(drive);
    if (std::abs(smoothed_ff2_frequency - ff2_frequency) >= 1e-2)
    {
        smoothed_ff2_frequency +=
            (ff2_frequency - smoothed_ff2_frequency) * smoothing_factor;
        auto ff2_hpf_coefficients =
            juce::dsp::IIR::Coefficients<float>::makeLowPass(
                processSpec.sampleRate, smoothed_ff2_frequency
            );
        *ff2_hpf.coefficients = *ff2_hpf_coefficients;
    }
}

float BorealisOverdrive::driveToGain(float d)
{
    float t = d / 10.0f;
    float min_gain = juce::Decibels::decibelsToGain(-6.0f);
    float max_gain = juce::Decibels::decibelsToGain(9.0f);
    return min_gain + std::pow(t, 2) * (max_gain - min_gain);
}

void BorealisOverdrive::process(juce::AudioBuffer<float>& buffer)
{
    if (bypass)
    {
        return;
    }
    juce::AudioBuffer<float> dry_buffer;
    dry_buffer.makeCopyOf(buffer);

    float sampleRate = static_cast<float>(processSpec.sampleRate);
    setCoefficients();

    juce::dsp::AudioBlock<float> block(buffer);
    auto oversampledBlock = oversampler2x.processSamplesUp(block);

    auto* channelData = oversampledBlock.getChannelPointer(0);
    for (size_t i = 0; i < oversampledBlock.getNumSamples(); ++i)
    {
        float sample = channelData[i];
        applyOverdrive(sample, sampleRate);
        channelData[i] = sample;
    }
    oversampler2x.processSamplesDown(block);

    applyGain(buffer, previous_level, level);
    buffer.applyGain(mix);
    dry_buffer.applyGain(1.0f - mix);
    buffer.addFrom(0, 0, dry_buffer, 0, 0, buffer.getNumSamples());
};

void BorealisOverdrive::applyOverdrive(float& sample, float sampleRate)
{
    juce::ignoreUnused(sampleRate);

    float drive_gain = driveToGain(drive);
    float in = triode.processSample(sample);
    float in_drive = in * drive_gain;

    // feed forward 1
    float ff1 = ff1_lpf.processSample(in);

    // feed forward 2
    float ff2 = ff2_lpf.processSample(ff2_hpf.processSample(in_drive) + in);

    // distortion chain
    float hpfed = pre_hpf.processSample(in_drive);
    float lpfed = pre_lpf.processSample(hpfed);
    float shaped = attack_shelf.processSample(lpfed);
    float distorded = diode.processSample(shaped);
    float out = post_lpf.processSample(distorded);

    sample = padding * (out + 0.15f * ff1 + 0.10f * ff2);
}

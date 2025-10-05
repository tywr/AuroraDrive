#include "borealis.h"

#include <juce_dsp/juce_dsp.h>

void BorealisOverdrive::prepare(const juce::dsp::ProcessSpec& spec)
{
    juce::dsp::ProcessSpec oversampled_spec = spec;
    oversampled_spec.sampleRate *= 2.0;
    processSpec = oversampled_spec;

    oversampler2x.reset();
    oversampler2x.initProcessing(static_cast<size_t>(spec.maximumBlockSize));

    DBG("Preparing Borealis Overdrive with sample rate "
        << processSpec.sampleRate);

    dc_hpf.prepare(oversampled_spec);
    auto dc_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            oversampled_spec.sampleRate, dc_hpf_cutoff
        );
    *dc_hpf.coefficients = *dc_hpf_coefficients;

    attack_shelf.prepare(oversampled_spec);
    float attack_shelf_gain = charToGain(character);
    smoothed_attack_shelf_gain = attack_shelf_gain;
    auto attack_shelf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            oversampled_spec.sampleRate, attack_shelf_freq, 0.5f,
            attack_shelf_gain
        );
    *attack_shelf.coefficients = *attack_shelf_coefficients;

    pre_hpf.prepare(oversampled_spec);
    auto pre_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            oversampled_spec.sampleRate, pre_hpf_cutoff
        );
    *pre_hpf.coefficients = *pre_hpf_coefficients;

    post_lpf.prepare(oversampled_spec);
    auto post_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            oversampled_spec.sampleRate, post_lpf_cutoff
        );
    *post_lpf.coefficients = *post_lpf_coefficients;

    post_mid_cut.prepare(oversampled_spec);
    auto post_mid_cut_coefficients =
        juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            oversampled_spec.sampleRate, post_mid_cut_frequency, 0.707f,
            post_mid_cut_gain
        );
    *post_mid_cut.coefficients = *post_mid_cut_coefficients;

    diode = GermaniumDiode(oversampled_spec.sampleRate);
}

float BorealisOverdrive::charToGain(float c)
{
    float t = c / 10.0f;
    float min_gain = juce::Decibels::decibelsToGain(-12.0f);
    float max_gain = juce::Decibels::decibelsToGain(12.0f);
    return min_gain + (max_gain - min_gain) * (t * t);
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
                processSpec.sampleRate, attack_shelf_freq, 0.707,
                smoothed_attack_shelf_gain
            );
        *attack_shelf.coefficients = *attack_shelf_coefficients;
    }
}

float BorealisOverdrive::driveToGain(float d)
{
    float t = d / 10.0f;
    return 1.0f + std::pow(t, 3) * 20.0f;
}

void BorealisOverdrive::process(juce::AudioBuffer<float>& buffer)
{
    if (bypass)
    {
        return;
    }
    juce::AudioBuffer<float> dry_buffer;
    dry_buffer.makeCopyOf(buffer);

    float drive_gain = driveToGain(drive);
    applyGain(buffer, previous_drive_gain, drive_gain);
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

    float hpfed = pre_hpf.processSample(dc_hpf.processSample(sample));
    float shaped = attack_shelf.processSample(hpfed);
    float distorded = diode.processSample(shaped);
    float mid_cut = post_mid_cut.processSample(distorded);
    float out = post_lpf.processSample(mid_cut);
    sample = out;
}

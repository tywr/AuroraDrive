#include "Helios.h"
#include "../circuits/triode.h"

#include <juce_dsp/juce_dsp.h>

void HeliosOverdrive::prepare(const juce::dsp::ProcessSpec& spec)
{
    juce::dsp::ProcessSpec oversampled_spec = spec;
    oversampled_spec.sampleRate *= 2;
    processSpec = oversampled_spec;

    oversampler2x.reset();
    oversampler2x.initProcessing(static_cast<size_t>(spec.maximumBlockSize));

    dc_hpf.prepare(oversampled_spec);
    auto dc_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            oversampled_spec.sampleRate, dc_hpf_cutoff
        );
    *dc_hpf.coefficients = *dc_hpf_coefficients;

    dc_hpf2.prepare(oversampled_spec);
    auto dc_hpf2_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            oversampled_spec.sampleRate, dc_hpf2_cutoff
        );
    *dc_hpf2.coefficients = *dc_hpf2_coefficients;

    pre_hpf.prepare(oversampled_spec);
    auto pre_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            oversampled_spec.sampleRate, pre_hpf_cutoff
        );
    *pre_hpf.coefficients = *pre_hpf_coefficients;

    mid_scoop.prepare(oversampled_spec);
    auto mid_scoop_coefficients =
        juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            oversampled_spec.sampleRate, mid_scoop_frequency, mid_scoop_q,
            mid_scoop_gain
        );
    *mid_scoop.coefficients = *mid_scoop_coefficients;

    tone_lpf.prepare(oversampled_spec);
    auto tone_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            oversampled_spec.sampleRate, tone_lpf_cutoff
        );
    *tone_lpf.coefficients = *tone_lpf_coefficients;

    post_lpf.prepare(oversampled_spec);
    auto post_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            oversampled_spec.sampleRate, post_lpf_cutoff
        );
    *post_lpf.coefficients = *post_lpf_coefficients;

    triode = Triode(oversampled_spec.sampleRate);
    triode2 = Triode(oversampled_spec.sampleRate);
    triode_pre = Triode(oversampled_spec.sampleRate);
    triode_pre2 = Triode(oversampled_spec.sampleRate);
}

float HeliosOverdrive::driveToGain(float d)
{
    float t = d / 10.0f;
    float min_gain = juce::Decibels::decibelsToGain(3.0f);
    float max_gain = juce::Decibels::decibelsToGain(18.0f);
    return min_gain + std::pow(t, 3.0f) * (max_gain - min_gain);
}

float HeliosOverdrive::charToFreq(float c)
{
    float t = character / 10.0f;
    float max_value = 8000.0f;
    float min_value = 800.0f;
    return min_value + std::pow(t, 2) * (max_value - min_value);
}

void HeliosOverdrive::process(juce::AudioBuffer<float>& buffer)
{
    if (bypass)
    {
        return;
    }
    juce::AudioBuffer<float> dry_buffer;
    dry_buffer.makeCopyOf(buffer);

    // applyGain(buffer, previous_drive_gain, drive_gain);
    float sampleRate = static_cast<float>(processSpec.sampleRate);
    // Update tone cutoff
    float new_tone_lpf_cutoff = charToFreq(character);

    if (!juce::approximatelyEqual(tone_lpf_cutoff, new_tone_lpf_cutoff))
    {
        tone_lpf_cutoff = new_tone_lpf_cutoff;
        auto tone_lpf_coefficients =
            juce::dsp::IIR::Coefficients<float>::makeLowPass(
                sampleRate, tone_lpf_cutoff
            );
        *tone_lpf.coefficients = *tone_lpf_coefficients;
    }

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

void HeliosOverdrive::applyOverdrive(float& sample, float sampleRate)
{
    juce::ignoreUnused(sampleRate);

    float drive_gain = driveToGain(drive);
    float hpfed = pre_hpf.processSample(sample);
    float filtered = mid_scoop.processSample(tone_lpf.processSample(hpfed));
    float preamped1 =
        drive_gain * dc_hpf.processSample(triode_pre.processSample(filtered));
    float preamped2 =
        dc_hpf2.processSample(triode_pre2.processSample(preamped1));
    float out = post_lpf.processSample(preamped2);
    sample = out * padding;
}

#include "borealis.h"
#include "../circuits/triode.h"

#include <juce_dsp/juce_dsp.h>

void BorealisOverdrive::prepare(const juce::dsp::ProcessSpec& spec)
{
    juce::dsp::ProcessSpec oversampled_spec = spec;
    oversampled_spec.sampleRate *= 2;
    processSpec = oversampled_spec;

    oversampler2x.reset();
    oversampler2x.initProcessing(static_cast<size_t>(spec.maximumBlockSize));

    high_pass_filter.prepare(oversampled_spec);
    low_pass_filter.prepare(oversampled_spec);

    auto hpf_coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(
        oversampled_spec.sampleRate, borealis_params.hpf_cutoff
    );
    *high_pass_filter.coefficients = *hpf_coefficients;

    auto lpf_coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(
        oversampled_spec.sampleRate, borealis_params.hpf_cutoff
    );
    *low_pass_filter.coefficients = *lpf_coefficients;

    diode = Diode(oversampled_spec.sampleRate, c, r, i_s, v_t);
}

float BorealisOverdrive::driveToGain(float d)
{
    float t = d / 10.0f;
    return 2.0f + std::pow(t, 2) * 100.0f;
}

void BorealisOverdrive::process(juce::AudioBuffer<float>& buffer)
{
    if (bypass)
    {
        return;
    }
    float drive_gain = driveToGain(drive);
    applyGain(buffer, previous_drive_gain, drive_gain);
    float sampleRate = static_cast<float>(processSpec.sampleRate);

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
};

void BorealisOverdrive::applyOverdrive(float& sample, float sampleRate)
{
    juce::ignoreUnused(sampleRate);

    // Apply high-pass filter before distortion
    high_pass_filter.processSample(sample);

    // Apply triod overdrive
    float current = diode.processSample(sample);

    // Apply mix and low pass filter
    sample = current * mix + sample * (1 - mix);
    low_pass_filter.processSample(sample);
}

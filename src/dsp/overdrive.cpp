#include "overdrive.h"

#include <juce_dsp/juce_dsp.h>

void Overdrive::applyGain(
    juce::AudioBuffer<float>& buffer, float& previous_gain, float& gain
)
{
    if (juce::approximatelyEqual(gain, previous_gain))
    {
        buffer.applyGain(gain);
    }
    else
    {
        buffer.applyGainRamp(0, buffer.getNumSamples(), previous_gain, gain);
        previous_gain = gain;
    }
}

void Overdrive::prepare(const juce::dsp::ProcessSpec& spec)
{
    processSpec = spec;
    high_pass_filter.prepare(spec);
    low_pass_filter.prepare(spec);

    switch (type)
    {
    // Helios Case
    case 0:
        auto hpf_coefficients =
            juce::dsp::IIR::Coefficients<float>::makeHighPass(
                spec.sampleRate, helios_params.hpf_cutoff
            );
        *high_pass_filter.coefficients = *hpf_coefficients;

        auto lpf_coefficients =
            juce::dsp::IIR::Coefficients<float>::makeHighPass(
                spec.sampleRate, helios_params.hpf_cutoff
            );
        *low_pass_filter.coefficients = *lpf_coefficients;
        break;
    }
}

void Overdrive::applyOverdriveHelios(float& sample, float sampleRate)
{
    juce::ignoreUnused(sampleRate);

    // Apply high-pass filter before distortion
    high_pass_filter.processSample(sample);

    // Apply triod overdrive
    float current = sample;
    for (int i = 0; i < 3; ++i)
    {
        current = (helios_params.t_gain * current) /
                  (1.0f + std::pow(
                              std::abs(helios_params.t_gain * current),
                              helios_params.t_sat
                          ));
        current = current / (1.0f + std::abs(current) / helios_params.t_knee);
    }

    // Apply mix and low pass filter
    sample = current * mix + sample * (1 - mix);
    low_pass_filter.processSample(sample);
}

void Overdrive::process(juce::AudioBuffer<float>& buffer)
{
    if (bypass)
    {
        return;
    }
    applyGain(buffer, previous_drive, drive);
    float sampleRate = static_cast<float>(processSpec.sampleRate);

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            switch (type)
            {
            case 0:
                applyOverdriveHelios(channelData[sample], sampleRate);
                break;
            case 1:
                break;
            }
        }
    }
    applyGain(buffer, previous_level, level);
}

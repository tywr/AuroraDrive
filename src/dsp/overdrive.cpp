#include "overdrive.h"

#include <juce_dsp/juce_dsp.h>

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
    high_pass_filter.processSample(sample);

    float driven_sample = sample * drive;
    float soft_clipped = std::tanh(driven_sample);
    float hard_clipped;
    if (driven_sample > 0.0f)
    {
        hard_clipped = 1.0f - std::exp(-driven_sample);
    }
    else
    {
        hard_clipped = -1.0f + std::exp(driven_sample);
    }

    float combined_clipped =
        (soft_clipped * (1.0f - character)) + (hard_clipped * character);

    sample = (sample * (1.0f - mix)) + (combined_clipped * mix);
    sample = low_pass_filter.processSample(sample);
}

void Overdrive::process(juce::AudioBuffer<float>& buffer)
{
    if (bypass)
    {
        return;
    }
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
}

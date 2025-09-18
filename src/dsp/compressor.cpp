#include "compressor.h"

#include <juce_dsp/juce_dsp.h>

void Compressor::prepare(const juce::dsp::ProcessSpec& spec)
{
    processSpec = spec;
}

void Compressor::applyGain(juce::AudioBuffer<float>& buffer)
{
    if (juce::approximatelyEqual(gain, previousGain))
    {
        buffer.applyGain(gain);
    }
    else
    {
        buffer.applyGainRamp(0, buffer.getNumSamples(), previousGain, gain);
        previousGain = gain;
    }
}

void Compressor::process(juce::AudioBuffer<float>& buffer)
{
    if (bypass)
    {
        return;
    }
    auto sampleRate = processSpec.sampleRate;

    applyGain(buffer);

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float envelope = static_cast<float>(std::abs(channelData[sample]));
            float coef;

            if (envelope > smoothedLevel)
            {
                coef =
                    static_cast<float>(std::exp(-1.0f / (sampleRate * attack)));
            }
            else
            {
                if (smoothedLevel > peak)
                {
                    coef = static_cast<float>(
                        std::exp(-1.0f / (sampleRate * release1))
                    );
                }
                else
                {
                    coef = static_cast<float>(
                        std::exp(-1.0f / (sampleRate * release2))
                    );
                }
            }
            smoothedLevel = (coef * smoothedLevel) + ((1.0f - coef) * envelope);

            if (smoothedLevel > peak)
            {
                gainReductionDb = (peak - smoothedLevel) * (ratio - 1.0f);
            }
            gainReduction = juce::Decibels::decibelsToGain(gainReductionDb);

            channelData[sample] = (channelData[sample] * gainReduction * mix) +
                                  (channelData[sample] * (1.0f - mix));
        }
    }
}

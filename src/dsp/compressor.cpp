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

void Compressor::computeGainReductionOptometric(float sampleRate)
{
    float rawGainReduction;
    float rawGainReductionDb;
    if (envelopeLevel > threshold)
    {
        float overThreshold = juce::Decibels::gainToDecibels(envelopeLevel) -
                              juce::Decibels::gainToDecibels(threshold);
        rawGainReductionDb = -overThreshold * (1.0f - 1.0f / ratio);
    }
    else
    {
        rawGainReductionDb = 0.0f; // No compression when below threshold
    }

    rawGainReduction = juce::Decibels::decibelsToGain(rawGainReductionDb);

    float gainSmoothingCoef =
        std::exp(-1.0f / (sampleRate * gainSmoothingTime));

    // Actual gain reduction applied to the signal
    gainReduction = (gainSmoothingCoef * gainReduction) +
                    ((1.0f - gainSmoothingCoef) * rawGainReduction);
    gainReductionDb = juce::Decibels::gainToDecibels(gainReduction);
}

void Compressor::computeEnvelope(float inputSample, float sampleRate)
{
    float absSample = std::abs(inputSample);

    float coef;
    if (absSample > envelopeLevel)
    {
        coef = std::exp(-1.0f / (sampleRate * attack));
    }
    else
    {
        if (envelopeLevel > threshold)
        {
            coef = std::exp(-1.0f / (sampleRate * release1));
        }
        else
        {
            coef = std::exp(-1.0f / (sampleRate * release2));
        }
    }
    envelopeLevel = (coef * envelopeLevel) + ((1.0f - coef) * absSample);
}

void Compressor::process(juce::AudioBuffer<float>& buffer)
{
    if (bypass)
    {
        peakGainReductionDb = 0.0f;
        return;
    }
    float sampleRate = static_cast<float>(processSpec.sampleRate);

    applyGain(buffer);

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            computeEnvelope(channelData[sample], sampleRate);
            computeGainReductionOptometric(sampleRate);
            channelData[sample] = (channelData[sample] * gainReduction * mix) +
                                  (channelData[sample] * (1.0f - mix));
        }
    }
}

#include "compressor.h"

#include <juce_dsp/juce_dsp.h>

void Compressor::applyLevel(juce::AudioBuffer<float>& buffer)
{
    if (juce::approximatelyEqual(level, previous_level))
    {
        buffer.applyGain(level);
    }
    else
    {
        buffer.applyGainRamp(0, buffer.getNumSamples(), previous_level, level);
        previous_level = level;
    }
}

void Compressor::prepare(const juce::dsp::ProcessSpec& spec)
{
    processSpec = spec;
}

void Compressor::computeGainReductionOptometric(float& sample, float sampleRate)
{
    // Optometric Compressor
    //
    // Envelope processing
    float absSample = std::abs(sample);

    float coef;
    if (absSample > envelopeLevel)
    {
        coef = std::exp(-1.0f / (sampleRate * optoParams.attack));
    }
    else
    {
        if (envelopeLevel > threshold)
        {
            coef = std::exp(-1.0f / (sampleRate * optoParams.release1));
        }
        else
        {
            coef = std::exp(-1.0f / (sampleRate * optoParams.release2));
        }
    }
    envelopeLevel = (coef * envelopeLevel) + ((1.0f - coef) * absSample);

    // Gain Comutation
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
        std::exp(-1.0f / (sampleRate * optoParams.gainSmoothingTime));

    gainReduction = (gainSmoothingCoef * gainReduction) +
                    ((1.0f - gainSmoothingCoef) * rawGainReduction);
    gainReductionDb = juce::Decibels::gainToDecibels(gainReduction);
    sample = (sample * gainReduction * mix) + (sample * (1.0f - mix));

    // Saturation
    float saturated = std::tanh(sample * (1.0f + optoParams.saturationAmount));
    sample = sample + (saturated - sample) * optoParams.saturationAmount *
                          optoParams.saturationMix;
}

void Compressor::computeGainReductionFet(float& sample, float sampleRate)
{
    // FET-style peak envelope processing (much faster)
    float absSample = std::abs(sample);
    float coef;

    if (absSample > envelopeLevel)
    {
        coef = std::exp(-1.0f / (sampleRate * fetParams.attack));
    }
    else
    {
        coef = std::exp(-1.0f / (sampleRate * fetParams.release));
    }
    envelopeLevel = (coef * envelopeLevel) + ((1.0f - coef) * absSample);

    // FET Gain Reduction (more aggressive, higher ratios)
    float rawGainReduction;
    float rawGainReductionDb;
    if (envelopeLevel > threshold)
    {
        float overThreshold = juce::Decibels::gainToDecibels(envelopeLevel) -
                              juce::Decibels::gainToDecibels(threshold);
        rawGainReductionDb = -overThreshold * (1.0f - 1.0f / ratio);

        rawGainReductionDb = std::max(rawGainReductionDb, -40.0f);
    }
    else
    {
        rawGainReductionDb = 0.0f;
    }
    rawGainReduction = juce::Decibels::decibelsToGain(rawGainReductionDb);

    float gainSmoothingCoef =
        std::exp(-1.0f / (sampleRate * fetParams.gainSmoothingTime));
    gainReduction = (gainSmoothingCoef * gainReduction) +
                    ((1.0f - gainSmoothingCoef) * rawGainReduction);
    gainReductionDb = juce::Decibels::gainToDecibels(gainReduction);

    // Apply compression
    sample = (sample * gainReduction * mix) + (sample * (1.0f - mix));

    // FET-style saturation (more aggressive, asymmetric)
    float driven = sample * (1.0f + fetParams.saturationAmount * 2.0f);
    float fetSaturated;
    if (driven > 0.0f)
    {
        fetSaturated = std::tanh(driven * 1.5f); // Harder positive saturation
    }
    else
    {
        fetSaturated = std::tanh(driven * 0.8f); // Softer negative (asymmetric)
    }

    // More aggressive saturation blend for FET punch
    sample = sample + (fetSaturated - sample) * fetParams.saturationAmount *
                          fetParams.saturationMix;
}

void Compressor::computeGainReductionVca(float& sample, float sampleRate)
{
    float absSample = std::abs(sample);

    static float rmsBuffer[64] = {0}; // Small buffer for RMS calculation
    static int rmsIndex = 0;

    rmsBuffer[rmsIndex] = absSample * absSample;
    rmsIndex = (rmsIndex + 1) % 64;

    float rmsSum = 0.0f;
    for (int i = 0; i < 64; ++i)
        rmsSum += rmsBuffer[i];

    float rmsLevel = std::sqrt(rmsSum / 64.0f);

    float coef;
    if (rmsLevel > envelopeLevel)
    {
        coef = std::exp(-1.0f / (sampleRate * vcaParams.attack));
    }
    else
    {
        coef = std::exp(-1.0f / (sampleRate * vcaParams.release));
    }
    envelopeLevel = (coef * envelopeLevel) + ((1.0f - coef) * rmsLevel);

    float rawGainReduction;
    float rawGainReductionDb;
    if (envelopeLevel > threshold)
    {
        float overThreshold = juce::Decibels::gainToDecibels(envelopeLevel) -
                              juce::Decibels::gainToDecibels(threshold);

        rawGainReductionDb = -overThreshold * (1.0f - 1.0f / ratio);

        if (overThreshold < vcaParams.kneeWidth)
        {
            float kneeRatio = overThreshold / vcaParams.kneeWidth;
            rawGainReductionDb *= (kneeRatio * kneeRatio);
        }
    }
    else
    {
        rawGainReductionDb = 0.0f;
    }

    rawGainReduction = juce::Decibels::decibelsToGain(rawGainReductionDb);

    float gainSmoothingCoef =
        std::exp(-1.0f / (sampleRate * vcaParams.gainSmoothingTime));
    gainReduction = (gainSmoothingCoef * gainReduction) +
                    ((1.0f - gainSmoothingCoef) * rawGainReduction);
    gainReductionDb = juce::Decibels::gainToDecibels(gainReduction);

    sample = (sample * gainReduction * mix) + (sample * (1.0f - mix));

    float cleanSaturated =
        sample / (1.0f + std::abs(sample * vcaParams.saturationAmount * 0.5f));
    sample = sample + (cleanSaturated - sample) * vcaParams.saturationAmount *
                          vcaParams.saturationMix;
}

void Compressor::process(juce::AudioBuffer<float>& buffer)
{
    if (bypass)
    {
        gainReductionDb = 0.0f;
        return;
    }
    float sampleRate = static_cast<float>(processSpec.sampleRate);

    auto* channelData = buffer.getWritePointer(0);
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        switch (type)
        {
        case 0:
            computeGainReductionOptometric(channelData[sample], sampleRate);
            break;
        case 1:
            computeGainReductionFet(channelData[sample], sampleRate);
            break;
        case 2:
            computeGainReductionVca(channelData[sample], sampleRate);
            break;
        }
    }
    // apply level
    applyLevel(buffer);
}

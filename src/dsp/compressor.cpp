#include "compressor.h"

#include <juce_dsp/juce_dsp.h>

void Compressor::prepare(const juce::dsp::ProcessSpec& spec)
{
    processSpec = spec;
}

void Compressor::process(juce::AudioBuffer<float>& buffer)
{
    auto sampleRate = processSpec.sampleRate;
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            // 1. Level Detection
            // Get the absolute value of the signal
            float inputLevel = std::abs(channelData[sample]);

            // 2. Envelope Follower
            // Use a one-pole filter for attack and release smoothing
            float smoothingCoefficient;
            if (inputLevel > envelope)
            {
                // Attack phase: use a fast attack coefficient
                smoothingCoefficient = std::exp(-1.0f / (sampleRate * attack));
            }
            else
            {
                // Release phase: use a slow release coefficient
                smoothingCoefficient = std::exp(-1.0f / (sampleRate * release));
            }

            envelope = (envelope * smoothingCoefficient) +
                       (inputLevel * (1.0f - smoothingCoefficient));

            // 3. Gain Computer
            float gainReduction = 1.0f;
            if (envelope > threshold)
            {
                // Apply a gain reduction based on a linear
                // threshold
                float gainReductiondB = (threshold - envelope) * (1.0f - ratio);
                gainReduction = juce::Decibels::decibelsToGain(gainReductiondB);
            }

            // 4. Gain Application
            // Apply the gain to the sample
            channelData[sample] *= gainReduction;
        }
    }
}

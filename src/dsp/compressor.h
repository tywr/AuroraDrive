#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class Compressor
{
  public:
    // Prepares compressor with a ProcessSpec-Object containing samplerate,
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void applyGain(juce::AudioBuffer<float>& buffer);
    void computeEnvelope(float inputSample, float sampleRate);
    void computeGainReductionOptometric(float sampleRate);

    void setBypass(bool newBypass)
    {
        bypass = newBypass;
    }
    void setMix(float newMix)
    {
        mix = newMix;
    }
    void setThreshold(float newThreshold)
    {
        threshold = newThreshold;
    }

    void setGain(float newGain)
    {
        gain = newGain;
    }

    float getGainReductionDb()
    {
        return gainReductionDb;
    }

  private:
    juce::dsp::ProcessSpec processSpec{-1, 0, 0};
    int debugCounter = 0;

    // gui parameters
    bool bypass = false;
    float mix = 0.5f;
    float threshold = 0.5f;
    float gain = 0.5f;

    // internal state of compressor
    float envelopeLevel = 1.0f;
    float previousGain = 1.0f;
    float gainReductionDb = 0.0f;
    float gainReduction = 1.0f;

    // metering value
    float peakGainReductionDb = 0.0f;
    float peakDecay = 0.99f;

    // hardcoded parameters
    float gainSmoothingTime = 0.05f;
    float ratio = 3.0f; // 3:1 compression ratio
    float attack = 0.01f;
    float release1 = 0.06f;
    float release2 = 0.5f;
};

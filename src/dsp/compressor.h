#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class Compressor
{
  public:
    // Prepares compressor with a ProcessSpec-Object containing samplerate,
    void applyLevel(juce::AudioBuffer<float>& buffer);
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void computeGainReductionOptometric(float& sample, float sampleRate);
    void computeGainReductionFet(float& sample, float sampleRate);
    void computeGainReductionVca(float& sample, float sampleRate);

    void setRatio(float newRatio)
    {
        mix = newRatio;
    }
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

    void setLevel(float newLevel)
    {
        level = newLevel;
    }

    void setTypeFromIndex(int index)
    {
        // 0 = OPTO, 1 = FET, 2 = VCA
        type = index;
    }

    float getGainReductionDb()
    {
        return gainReductionDb;
    }

  private:
    juce::dsp::ProcessSpec processSpec{-1, 0, 0};
    int debugCounter = 0;

    // gui parameters
    int type;
    bool bypass;
    float mix;
    float threshold;
    float level;
    float ratio;

    // internal state of compressor
    float envelopeLevel = 1.0f;
    float previous_level = 1.0f;
    float gainReductionDb = 0.0f;
    float gainReduction = 1.0f;

    // hardcoded parameters for optometric compressor
    struct
    {
        float attack = 0.01f;
        float release1 = 0.06f;
        float release2 = 0.5f;
        float saturationAmount = 0.2f;
        float saturationMix = 0.05f;
        float gainSmoothingTime = 0.05f;
    } optoParams;

    struct
    {
        float attack = 0.0003f;
        float release = 0.1f;
        float saturationAmount = 0.4f;
        float saturationMix = 0.15f;
        float gainSmoothingTime = 0.01f;
    } fetParams;

    struct
    {
        float attack = 0.005f;
        float release = 0.4f;
        float saturationAmount = 0.2f;
        float saturationMix = 0.05f;
        float gainSmoothingTime = 0.01f;
        float kneeWidth = 2.0f;
    } vcaParams;
};

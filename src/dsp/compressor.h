#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class Compressor
{
  public:
    // Prepares compressor with a ProcessSpec-Object containing samplerate,
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);

    void setThreshold(float newThreshold);
    void setRatio(float newRatio);
    void setAttack(float newAttack);
    void setRelease(float newRelease);

  private:
    juce::dsp::ProcessSpec processSpec{-1, 0, 0};
    float threshold = 0.0f;
    float ratio = 1.0f;
    float attack = 0.01f;
    float release = 0.1f;
    float envelope = 0.0f;
};

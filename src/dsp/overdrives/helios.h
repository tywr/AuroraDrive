#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class Overdrive
{
  public:
    // Prepares compressor with a ProcessSpec-Object containing samplerate,
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void applyOverdriveHelios(float& sample, float sampleRate);

    void applyGain(
        juce::AudioBuffer<float>& buffer, float& previous_gain, float& gain
    );
    void setBypass(bool shouldBypass)
    {
        bypass = shouldBypass;
    }
    void setLevel(float newLevel)
    {
        level = newLevel;
    }
    void setMix(float newMix)
    {
        mix = newMix;
    }
    void setDrive(float newDrive)
    {
        drive = newDrive;
    }
    void setCharacter(float newCharacter)
    {
        character = newCharacter;
    }
    void setTypeFromIndex(int index)
    {
        type = index;
    }

  private:
    juce::dsp::ProcessSpec processSpec{-1, 0, 0};

    // gui parameters
    int type;
    bool bypass;
    float level;
    float drive;
    float character;
    float mix;

    // internal parameters
    float previous_drive = 1.0f;
    float previous_level = 1.0f;
    juce::dsp::IIR::Filter<float> high_pass_filter;
    juce::dsp::IIR::Filter<float> low_pass_filter;

    // hardcoded parameters for each overdrive type
    struct
    {
        float hpf_cutoff = 120.0f;
        float lpf_cutoff = 8000.0f;
        float t_gain = 1.1f;
        float t_sat = 0.3f;
        float t_knee = 0.1f;
    } helios_params;
};

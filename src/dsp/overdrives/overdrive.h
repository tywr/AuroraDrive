#pragma once
#include <juce_dsp/juce_dsp.h>

class Overdrive
{
  public:
    virtual void prepare(const juce::dsp::ProcessSpec& spec) {};
    virtual void applyOverdrive(float& sample, float sampleRate) {};
    virtual float driveToGain(float drive)
    {
        return drive;
    };
    virtual void process(juce::AudioBuffer<float>& buffer) {};

    void applyGain(
        juce::AudioBuffer<float>& buffer, float& previous_gain, float& gain
    )
    {
        if (juce::approximatelyEqual(gain, previous_gain))
        {
            buffer.applyGain(gain);
        }
        else
        {
            buffer.applyGainRamp(
                0, buffer.getNumSamples(), previous_gain, gain
            );
            previous_gain = gain;
        }
    };
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

  protected:
    juce::dsp::ProcessSpec processSpec{-1, 0, 0};

    // gui parameters
    int type;
    bool bypass;
    float level;
    float drive;
    float character;
    float mix;

    // state parameters
    float previous_drive_gain = 1.0f;
    float previous_level = 1.0f;
};

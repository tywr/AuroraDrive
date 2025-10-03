
class BaseOverdrive
{
  public:
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
    void setTypeFromIndex(int index)
    {
        type = index;
    }

  private:
    // gui parameters
    int type;
    bool bypass;
    float level;
    float drive;
    float character;
    float mix;
};

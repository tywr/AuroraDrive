#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
class PluginAudioProcessor final : public juce::AudioProcessor,
                                   public juce::ChangeBroadcaster {
  public:
    PluginAudioProcessor();
    ~PluginAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    // Decay factor for level smoothing
    double decayFactor = 0.95f;
    juce::Value inputLevel;
    juce::Value outputLevel;
    double smoothLevel(double newLevel, double currentLevel);

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

  private:
    juce::AudioProcessorValueTreeState parameters;
    float previousInputGain;
    float previousOutputGain;
    std::atomic<float>* inputGainParameter = nullptr;
    std::atomic<float>* outputGainParameter = nullptr;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginAudioProcessor)
};

#pragma once

#include "dsp/compressor.h"
#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
class PluginAudioProcessor final
    : public juce::AudioProcessor,
      public juce::AudioProcessorValueTreeState::Listener
{
  public:
    PluginAudioProcessor();
    ~PluginAudioProcessor() override;

    void parameterChanged(
        const juce::String& parameterID, float newValue
    ) override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    // Decay factor for level smoothing
    double decayFactor = 0.98f;
    juce::Value inputLevel;                // in dB
    juce::Value outputLevel;               // in dB
    juce::Value compressorGainReductionDb; // in dB
    void updateInputLevel(juce::AudioBuffer<float>& buffer);
    void updateOutputLevel(juce::AudioBuffer<float>& buffer);
    void applyInputGain(juce::AudioBuffer<float>& buffer);
    void applyOutputGain(juce::AudioBuffer<float>& buffer);
    double smoothLevel(double newLevel, double currentLevel);

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

  private:
    juce::AudioProcessorValueTreeState parameters;
    Compressor compressor;
    float previousInputGainLinear;
    float previousOutputGainLinear;
    std::atomic<float>* inputGainParameter = nullptr;
    std::atomic<float>* outputGainParameter = nullptr;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginAudioProcessor)
};

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor,
                                        public juce::ChangeBroadcaster {
  public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    float getRawInputLevel() const { return rawInputLevel.get(); }
    float getRawOutputLevel() const { return rawOutputLevel.get(); }

    void setInGain(float gain) { inGain = gain; }
    float getInGain() const { return inGain.get(); }
    void setOutGain(float gain) { outGain = gain; }
    float getOutGain() const { return outGain.get(); }

    float getInputLevel() const { return inputLevel.get(); }
    float getOutputLevel() const { return outputLevel.get(); }
    float getSmoothedInputLevel() const { return smoothedInputLevel.get(); }
    float getSmoothedOutputLevel() const { return smoothedOutputLevel.get(); }

    //==============================================================================
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
    void processInputLevels(juce::AudioBuffer<float>& buffer,
                            float decay_factor = 0.95f);
    void processOutputLevels(juce::AudioBuffer<float>& buffer,
                             float decay_factor = 0.95f);

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

  private:
    juce::Atomic<float> rawInputLevel = 0.0f;
    juce::Atomic<float> inGain = 1.0f;
    juce::Atomic<float> inputLevel = 0.0f;
    juce::Atomic<float> smoothedInputLevel = 0.0f;

    juce::Atomic<float> rawOutputLevel = 0.0f;
    juce::Atomic<float> outGain = 1.0f;
    juce::Atomic<float> outputLevel = 0.0f;
    juce::Atomic<float> smoothedOutputLevel = 0.0f;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessor)
};

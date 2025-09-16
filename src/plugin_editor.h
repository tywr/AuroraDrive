#pragma once

#include "plugin_processor.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                              public juce::ChangeListener,
                                              public juce::Slider::Listener {
  public:
    explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    void sliderValueChanged(juce::Slider* slider) override;

    // Gain Control Variables
    void setupGainControl(juce::Slider& slider, juce::Label& label,
                          double minRange, double maxRange, double initialValue,
                          const juce::String& labelText);
    void setupGainMeter(juce::Slider& slider, double minRange, double maxRange);
    void resizeGainControls();
    void setMeterSliders(AudioPluginAudioProcessor* processor);

  private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    // Gain Sliders
    juce::Slider inGainSlider;
    juce::Label inGainLabel;
    juce::Slider outGainSlider;
    juce::Label outGainLabel;

    // Input and Output Meters
    juce::Slider inGainMeterSlider;
    juce::Slider outGainMeterSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        AudioPluginAudioProcessorEditor)
};

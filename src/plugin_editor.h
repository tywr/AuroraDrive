#pragma once

#include "meter.h"
#include "plugin_audio_processor.h"

//==============================================================================
class PluginEditor final : public juce::AudioProcessorEditor,
                           public juce::ChangeListener,
                           public juce::Slider::Listener {
  public:
    explicit PluginEditor(PluginAudioProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    void sliderValueChanged(juce::Slider* slider) override;

    void setupGainControl(juce::Slider& slider, juce::Label& label,
                          double minRange, double maxRange, double initialValue,
                          const juce::String& labelText);
    void setupGainMeter(juce::Slider& slider, double minRange, double maxRange);
    void resizeGainControls();
    void setMeterSliders(PluginAudioProcessor* processorRef);

  private:
    PluginAudioProcessor& processorRef;
    Meter inputMeter;
    Meter outputMeter;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    // Gain Sliders
    juce::Slider inGainSlider;
    juce::Label inGainLabel;
    juce::Slider outGainSlider;
    juce::Label outGainLabel;

    // Input and Output Meters
    juce::Slider inGainMeterSlider;
    juce::Slider outGainMeterSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};

#pragma once

#include "looks/colors.h"
#include "preamp_footer_component.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <unordered_map>

class PreampFooterComponent : public juce::Component,
                              public juce::Value::Listener
{
  public:
    PreampFooterComponent(
        juce::AudioProcessorValueTreeState& g, juce::Value& v
    );
    ~PreampFooterComponent() override;

    void resized() override;
    void valueChanged(juce::Value& v) override;
    void paint(juce::Graphics&) override;

  private:
    juce::AudioProcessorValueTreeState& parameters;
    juce::Value gain_reduction_value;
    juce::Slider gain_reduction_slider;

    juce::Label compressor_bypass_label;
    juce::Label overdrive_bypass_label;

    juce::ToggleButton compressor_bypass_button;
    juce::ToggleButton overdrive_bypass_button;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        compressor_bypass_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        overdrive_bypass_attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PreampFooterComponent)
};

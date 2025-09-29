#pragma once

#include "../looks/colors.h"
#include "compressor_footer_component.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <unordered_map>

class CompressorFooterComponent : public juce::Component,
                                  public juce::Value::Listener
{
  public:
    CompressorFooterComponent(
        juce::AudioProcessorValueTreeState& g, juce::Value& v
    );
    ~CompressorFooterComponent() override;

    void resized() override;
    void valueChanged(juce::Value& v) override;
    void paint(juce::Graphics&) override;
    void switchColour(juce::Colour colour1, juce::Colour colour2);

  private:
    juce::AudioProcessorValueTreeState& parameters;
    juce::Value gain_reduction_value;
    juce::Slider gain_reduction_slider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorFooterComponent)
};

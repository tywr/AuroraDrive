#pragma once

#include "../colours.h"
#include "compressor_component.h"
#include "compressor_footer_component.h"
#include "compressor_knobs_component.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class CompressorComponent : public juce::Component
{
  public:
    CompressorComponent(juce::AudioProcessorValueTreeState&, juce::Value&);
    ~CompressorComponent() override;

    void resized() override;
    void paint(juce::Graphics&) override;

  private:
    juce::AudioProcessorValueTreeState& parameters;
    juce::Value& gain_reduction_decibels;

    // Sub-components
    CompressorKnobsComponent knobs_component;
    CompressorFooterComponent footer_component;

    // Bypass button
    juce::ToggleButton bypass_button;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        bypass_attachment;

    // Type slider (for selecting compressor type)
    juce::Slider type_slider;
    juce::Label type_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        type_slider_attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorComponent)
};

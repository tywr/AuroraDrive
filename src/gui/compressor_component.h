#pragma once

#include "compressor_component.h"
#include "looks/aurora.h"
#include "looks/colors.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <unordered_map>

class CompressorComponent : public juce::Component
{
  public:
    CompressorComponent(juce::AudioProcessorValueTreeState&);
    ~CompressorComponent() override;

    void resized() override;

    void switchCompressorColour();

  private:
    juce::AudioProcessorValueTreeState& parameters;

    juce::TextButton compressorSwitcherButton;
    juce::StringArray compressorChoices = {"OPTO", "FET", "VCA"};
    const std::unordered_map<std::string, juce::Colour>
        compressorColourMapping = {
            {"OPTO", AuroraColors::white0       },
            {"FET",  AuroraColors::blue3        },
            {"VCA",  AuroraColors::aurora_orange}
    };
    juce::Colour const defaultCompressorColour = AuroraColors::grey3;

    juce::Label compressorTypeLabel;
    std::unique_ptr<juce::ParameterAttachment> compressorTypeLabelAttachment;

    juce::Label bypassLabel;
    juce::ToggleButton bypassButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        bypassButtonAttachment;

    juce::Slider gainSlider;
    juce::Label gainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        gainSliderAttachment;

    juce::Slider thresholdSlider;
    juce::Label thresholdLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        thresholdSliderAttachment;

    juce::Slider mixSlider;
    juce::Label mixLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        mixSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorComponent)
};

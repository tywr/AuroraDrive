#pragma once

#include "../looks/colors.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <unordered_map>

struct CompressorKnob
{
    juce::Slider* slider;
    juce::Label* label;
    juce::String parameter_id;
    juce::String label_text;
};

class CompressorKnobsComponent : public juce::Component
{
  public:
    CompressorKnobsComponent(juce::AudioProcessorValueTreeState&);
    ~CompressorKnobsComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void switchColour(juce::Colour, juce::Colour);

  private:
    juce::AudioProcessorValueTreeState& parameters;

    juce::Colour const default_type_colour = AuroraColors::grey3;

    juce::Slider threshold_slider;
    juce::Label threshold_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        threshold_slider_attachment;

    juce::Slider ratio_slider;
    juce::Label ratio_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        ratio_slider_attachment;


    juce::Slider mix_slider;
    juce::Label mix_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        mix_slider_attachment;

    juce::Slider level_slider;
    juce::Label level_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        level_slider_attachment;

    std::vector<
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>>
        slider_attachments;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorKnobsComponent);

    // Define knobs for easy looping
    std::vector<CompressorKnob> knobs = {
        {&threshold_slider, &threshold_label, "compressor_threshold", "THR"  },
        {&ratio_slider,     &ratio_label,     "compressor_ratio",     "RATIO"},
        {&mix_slider,       &mix_label,       "compressor_mix",       "MIX"  },
        {&level_slider,     &level_label,     "compressor_level_db",  "LEVEL"}
    };
};

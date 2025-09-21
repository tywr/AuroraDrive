#pragma once

#include "looks/aurora.h"
#include "looks/colors.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <unordered_map>

struct Knob
{
    juce::Slider* slider;
    juce::Label* label;
    juce::String parameter_id;
    juce::String label_text;
};

class OverdriveComponent : public juce::Component
{
  public:
    OverdriveComponent(juce::AudioProcessorValueTreeState&);
    ~OverdriveComponent() override;

    void resized() override;
    void switchColour();

  private:
    juce::AudioProcessorValueTreeState& parameters;

    juce::TextButton switcher_button;
    juce::StringArray overdrive_choices = {"HELIOS", "BOREALIS"};
    const std::unordered_map<std::string, juce::Colour>
        overdrive_colour_mapping = {
            {"HELIOS",   AuroraColors::aurora_orange},
            {"BOREALIS", AuroraColors::blue2        }
    };
    juce::Colour const default_type_colour = AuroraColors::grey3;

    juce::Label type_label;
    std::unique_ptr<juce::ParameterAttachment> type_label_attachment;

    juce::Label bypass_label;
    juce::ToggleButton bypass_button;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        bypass_button_attachment;

    juce::Slider drive_slider;
    juce::Label drive_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        drive_slider_attachment;

    juce::Slider level_slider;
    juce::Label level_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        level_slider_attachment;

    juce::Slider character_slider;
    juce::Label character_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        character_slider_attachment;

    juce::Slider mix_slider;
    juce::Label mix_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        mix_slider_attachment;

    std::vector<
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>>
        slider_attachments;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverdriveComponent);

    // Define knobs for easy looping
    std::vector<Knob> knobs = {
        {&drive_slider,     &drive_label,     "overdrive_drive_db",  "DRIVE"},
        {&level_slider,     &level_label,     "overdrive_level_db",  "LEVEL"},
        {&character_slider, &character_label, "overdrive_character", "CHAR" },
        {&mix_slider,       &mix_label,       "overdrive_mix",       "MIX"  }
    };
};

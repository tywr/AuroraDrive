#pragma once

#include "looks/aurora.h"
#include "looks/colors.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <unordered_map>

class OverdriveGui : public juce::Component
{
  public:
    OverdriveGui(juce::AudioProcessorValueTreeState&);
    ~OverdriveGui() override;

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

    std::vector<juce::Slider*> sliders{
        &drive_slider, &level_slider, &character_slider, &mix_slider
    };
    std::vector<juce::Label*> labels{
        &drive_label, &level_label, &character_label, &mix_label
    };
    std::vector<std::string> label_texts{"DRIVE", "LEVEL", "CHAR", "MIX"};
    std::vector<std::string> parameter_ids{
        "overdrive_drive_db", "overdrive_level_db", "overdrive_character",
        "overdrive_mix"
    };
    std::vector<
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>>
        slider_attachments;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverdriveGui);
};

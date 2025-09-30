#pragma once

#include "../colours.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <unordered_map>

struct OverdriveKnob
{
    juce::Slider* slider;
    juce::Label* label;
    juce::String parameter_id;
    juce::String label_text;
};

class OverdriveKnobsComponent : public juce::Component
{
  public:
    OverdriveKnobsComponent(juce::AudioProcessorValueTreeState&);
    ~OverdriveKnobsComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void switchColour();

  private:
    juce::AudioProcessorValueTreeState& parameters;

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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverdriveKnobsComponent);

    // Define knobs for easy looping
    std::vector<OverdriveKnob> knobs = {
        {&drive_slider,     &drive_label,     "overdrive_knobs_drive_db",  "DRIVE"},
        {&level_slider,     &level_label,     "overdrive_knobs_level_db",  "LEVEL"},
        {&character_slider, &character_label, "overdrive_knobs_character",
         "CHAR"                                                                   },
        {&mix_slider,       &mix_label,       "overdrive_knobs_mix",       "MIX"  }
    };
};

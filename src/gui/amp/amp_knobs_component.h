#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <unordered_map>

struct AmpKnob
{
    juce::Slider* slider;
    juce::Label* label;
    juce::String parameter_id;
    juce::String label_text;
};


class AmpKnobsComponent : public juce::Component
{
  public:
    AmpKnobsComponent(juce::AudioProcessorValueTreeState&);
    ~AmpKnobsComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void switchColour(juce::Colour, juce::Colour);

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

    juce::Slider bass_slider;
    juce::Label bass_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        bass_slider_attachment;

    juce::Slider low_mid_slider;
    juce::Label low_mid_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        low_mid_slider_attachment;

    juce::Slider hi_mid_slider;
    juce::Label hi_mid_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        hi_mid_slider_attachment;

    juce::Slider treble_slider;
    juce::Label treble_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        treble_slider_attachment;

    std::vector<
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>>
        slider_attachments;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AmpKnobsComponent);

    // Define knobs for easy looping
    std::vector<AmpKnob> knobs = {
        {&drive_slider,     &drive_label,     "overdrive_drive",  "DRIVE"  },
        {&bass_slider,      &bass_label,      "amp_eq_bass",         "BASS"   },
        {&low_mid_slider,   &low_mid_label,   "amp_eq_low_mid",      "LO MIDS"},
        {&hi_mid_slider,    &hi_mid_label,    "amp_eq_hi_mid",       "HI MIDS"},
        {&treble_slider,    &treble_label,    "amp_eq_treble",       "TREBLE" },
        {&character_slider, &character_label, "overdrive_character", "CHAR"   },
        {&mix_slider,       &mix_label,       "overdrive_mix",       "MIX"    },
        {&level_slider,     &level_label,     "overdrive_level_db",  "LEVEL"  },
    };
};

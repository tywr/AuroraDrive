#pragma once

#include "../colours.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <unordered_map>

struct EQKnob
{
    juce::Slider* slider;
    juce::Label* label;
    juce::String parameter_id;
    juce::String label_text;
};

class EQKnobsComponent : public juce::Component
{
  public:
    EQKnobsComponent(juce::AudioProcessorValueTreeState&);
    ~EQKnobsComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void switchColour(juce::Colour, juce::Colour);

  private:
    juce::AudioProcessorValueTreeState& parameters;

    juce::Slider bass_slider;
    juce::Label bass_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        bass_slider_attachment;

    juce::Slider mid_slider;
    juce::Label mid_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        mid_slider_attachment;

    juce::Slider treble_slider;
    juce::Label treble_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        treble_slider_attachment;

    juce::Slider mid_freq_slider;
    juce::Label mid_freq_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        mid_freq_slider_attachment;

    std::vector<
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>>
        slider_attachments;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQKnobsComponent);

    // Define knobs for easy looping
    std::vector<EQKnob> knobs = {
        {&bass_slider,     &bass_label,     "amp_eq_bass",          "BASS"  },
        {&mid_slider,      &mid_label,      "amp_eq_mid",           "MID"   },
        {&treble_slider,   &treble_label,   "amp_eq_treble",        "TREBLE"},
        {&mid_freq_slider, &mid_freq_label, "amp_eq_mid_frequency", "FREQ"  },
    };
};

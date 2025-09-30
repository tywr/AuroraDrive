#pragma once

#include "eq_knobs_component.h"
#include "overdrive_knobs_component.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class AmpComponent : public juce::Component
{
  public:
    AmpComponent(juce::AudioProcessorValueTreeState&);
    ~AmpComponent() override;

    void setColours(juce::Colour, juce::Colour);
    void resized() override;
    void paint(juce::Graphics&) override;

  private:
    juce::AudioProcessorValueTreeState& parameters;
    OverdriveKnobsComponent overdrive_knobs;
    EQKnobsComponent eq_knobs;
    juce::ToggleButton bypass_button;

    juce::Colour selected_colour1 = juce::Colours::darkred;
    juce::Colour selected_colour2 = juce::Colours::red;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AmpComponent)
};

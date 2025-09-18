#pragma once

#include "tabs.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class Tabs : public juce::TabbedComponent
{
  public:
    Tabs(juce::AudioProcessorValueTreeState&);
    ~Tabs() override;

  private:
    juce::AudioProcessorValueTreeState& parameters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Tabs)
};

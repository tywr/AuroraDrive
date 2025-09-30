#pragma once

#include "compressor/compressor_component.h"
#include "tabs.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class Tabs : public juce::TabbedComponent
{
  public:
    Tabs(juce::AudioProcessorValueTreeState&, juce::Value&);
    ~Tabs() override;
    void paint(juce::Graphics&) override;

  private:
    juce::AudioProcessorValueTreeState& parameters;
    CompressorComponent compressor_component;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Tabs)
};

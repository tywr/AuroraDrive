#pragma once

#include "compressor_left_component.h"
#include "meter.h"
#include "overdrive_component.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class PreAmpComponent : public juce::Component
{
  public:
    PreAmpComponent(juce::AudioProcessorValueTreeState&, juce::Value&);
    ~PreAmpComponent() override;

    void resized() override;
    void paint(juce::Graphics&) override;

  private:
    juce::AudioProcessorValueTreeState& parameters;
    juce::Value& compressor_gain_reduction_decibels;
    CompressorLeftComponent compressor_left_component;
    OverdriveComponent overdrive_component;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PreAmpComponent)
};

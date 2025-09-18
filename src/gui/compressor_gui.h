#pragma once

#include "compressor_gui.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class CompressorGui : public juce::Component {
  public:
    CompressorGui(juce::AudioProcessorValueTreeState&);
    ~CompressorGui() override;

    void paint(juce::Graphics&) override;
    void resized() override;

  private:
    juce::AudioProcessorValueTreeState& parameters;
    juce::Slider gainSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorGui)
};

#pragma once

#include "looks/aurora.h"
#include "meter.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class Header : public juce::Component
{
  public:
    Header(juce::AudioProcessorValueTreeState&, juce::Value&, juce::Value&);
    ~Header() override;

    void resized() override;
    void paint(juce::Graphics&) override;

  private:
    juce::AudioProcessorValueTreeState& parameters;
    Meter inputMeter;
    Meter outputMeter;

    juce::Colour headerColour = AuroraColors::white0;
    juce::Label inputLabel;
    juce::Label outputLabel;

    juce::Slider inputGainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        inputGainAttachment;

    juce::Slider outputGainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        outputGainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Header)
};

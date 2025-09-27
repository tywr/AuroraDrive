#pragma once

#include "compressor_component.h"
#include "looks/aurora.h"
#include "looks/colors.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <unordered_map>

class CompressorComponent : public juce::Component
{
  public:
    CompressorComponent(juce::AudioProcessorValueTreeState&);
    ~CompressorComponent() override;

    void resized() override;
    void paint(juce::Graphics&) override;

  private:
    juce::AudioProcessorValueTreeState& parameters;

    juce::Slider level_slider;
    juce::Label level_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        level_slider_attachment;

    juce::Slider threshold_slider;
    juce::Label threshold_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        threshold_slider_attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorComponent)
};

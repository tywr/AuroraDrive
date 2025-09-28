#pragma once

#include "compressor_right_component.h"
#include "looks/colors.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <unordered_map>

class CompressorRightComponent : public juce::Component
{
  public:
    CompressorRightComponent(juce::AudioProcessorValueTreeState&);
    ~CompressorRightComponent() override;

    void resized() override;
    void paint(juce::Graphics&) override;

  private:
    juce::AudioProcessorValueTreeState& parameters;

    juce::Slider selection_slider;
    juce::Label selection_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        selection_slider_attachment;

    juce::Slider ratio_slider;
    juce::Label ratio_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        ratio_slider_attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorRightComponent)
};

#pragma once

#include "../colours.h"
#include "../components/labeled_knob.h"
#include "../components/effect_component.h"
#include "../components/solid_tooltip.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

struct IRKnob
{
    LabeledKnob* knob;
    juce::String parameter_id;
    juce::String label_text;
};

class IRComponent : public EffectComponent
{
  public:
    IRComponent(juce::AudioProcessorValueTreeState&);
    void refreshStatus();
    void switchColour();
    void setupSliderTooltipHandling(LabeledKnob* knob);

  private:
    void paintContent(juce::Graphics&) override;
    void resizedContent(juce::Rectangle<int>) override;
    void bypassStateChanged() override;

    juce::AudioProcessorValueTreeState& parameters;

    SolidTooltip drag_tooltip;
    bool slider_being_dragged = false;

    juce::Colour current_colour = ColourCodes::white0;

    LabeledKnob ir_mix_knob;
    LabeledKnob gain_knob;
    LabeledKnob type_knob;

    juce::DrawableText type_display;
    std::unique_ptr<juce::ParameterAttachment> type_display_attachment;

    std::vector<
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>>
        slider_attachments;

    std::vector<IRKnob> knobs = {
        {&ir_mix_knob, "ir_mix",   "mix" },
        {&gain_knob,   "ir_level", "gain"},
        {&type_knob,   "ir_type",  "type"},
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IRComponent)
};

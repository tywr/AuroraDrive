#include "chorus_component.h"
#include "../colours.h"
#include "../dimensions.h"
#include "chorus_dimensions.h"
#include "chorus_knobs_component.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

ChorusComponent::ChorusComponent(juce::AudioProcessorValueTreeState& params)
    : EffectComponent(params, "CHORUS", "chorus_bypass"),
      knobs_component(params)
{
    addAndMakeVisible(knobs_component);
}

void ChorusComponent::paintContent(juce::Graphics&)
{
    knobs_component.switchColour(getAccentColour(), getTextColour());
}

void ChorusComponent::resizedContent(juce::Rectangle<int> bounds)
{
    knobs_component.setBounds(bounds);
}

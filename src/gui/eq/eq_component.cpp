#include "eq_component.h"
#include "../colours.h"
#include "../dimensions.h"
#include "eq_dimensions.h"
#include "eq_sliders_component.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

EqComponent::EqComponent(juce::AudioProcessorValueTreeState& params)
    : EffectComponent(params, "EQUALIZER", "eq_bypass"),
      sliders_component(params)
{
    addAndMakeVisible(sliders_component);
}

void EqComponent::paintContent(juce::Graphics&)
{
    sliders_component.switchColour(getAccentColour(), getTextColour());
}

void EqComponent::resizedContent(juce::Rectangle<int> bounds)
{
    sliders_component.setBounds(bounds);
}

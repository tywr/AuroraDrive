#pragma once

#include "../components/effect_component.h"
#include "chorus_knobs_component.h"

class ChorusComponent : public EffectComponent
{
  public:
    ChorusComponent(juce::AudioProcessorValueTreeState&);

  private:
    void paintContent(juce::Graphics&) override;
    void resizedContent(juce::Rectangle<int>) override;

    ChorusKnobsComponent knobs_component;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChorusComponent)
};

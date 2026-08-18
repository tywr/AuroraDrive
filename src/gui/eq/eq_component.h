#pragma once

#include "../components/effect_component.h"
#include "eq_sliders_component.h"

class EqComponent : public EffectComponent
{
  public:
    EqComponent(juce::AudioProcessorValueTreeState&);

  private:
    void paintContent(juce::Graphics&) override;
    void resizedContent(juce::Rectangle<int>) override;

    EqSlidersComponent sliders_component;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqComponent)
};

#pragma once

#include "../components/effect_component.h"
#include "compressor_knobs_component.h"
#include "compressor_meter_component.h"

class CompressorComponent : public EffectComponent
{
  public:
    CompressorComponent(juce::AudioProcessorValueTreeState&, juce::Value&);

    void paintMeter(juce::Graphics&);
    void visibilityChanged() override
    {
        meter_component.setVisible(isVisible());
        meter_component.visibilityChanged();
    }

  private:
    void paintContent(juce::Graphics&) override;
    void resizedContent(juce::Rectangle<int>) override;

    CompressorKnobsComponent knobs_component;
    CompressorMeterComponent meter_component;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorComponent)
};

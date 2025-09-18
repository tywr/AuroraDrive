#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class Meter : public juce::Component, public juce::Value::Listener
{
  public:
    Meter(juce::Value& v);
    ~Meter() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void valueChanged(juce::Value& v) override;
    double getCurrentValue()
    {
        return currentValue;
    }

  private:
    juce::Slider slider;
    juce::Value& measuredValue;
    double currentValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Meter)
};

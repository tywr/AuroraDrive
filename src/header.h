#pragma once

#include "meter.h"
#include <juce_gui_basics/juce_gui_basics.h>

class Header : public juce::Component {
  public:
    Header(juce::Value& vin, juce::Value& vout);
    ~Header() override;

    void paint(juce::Graphics&) override;
    void resized() override;

  private:
    Meter inputMeter;
    Meter outputMeter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Header)
};

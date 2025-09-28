#pragma once

#include "base_look_and_feel.h"
#include "colors.h"
#include <juce_gui_basics/juce_gui_basics.h>

class OverdriveLookAndFeel : public BaseLookAndFeel
{
  private:
    float strokeWidth = 6.0f;
    juce::Font main_font =
        juce::Font(juce::FontOptions("Oxanium", 15.0f, juce::Font::plain))
            .withExtraKerningFactor(0.2f);

  public:
    OverdriveLookAndFeel();
    void drawLabel(juce::Graphics&, juce::Label&) override;
    void drawRotarySlider(
        juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider
    ) override;
};

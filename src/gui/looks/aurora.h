#pragma once

#include "colors.h"
#include <juce_gui_basics/juce_gui_basics.h>

class AuroraLookAndFeel : public juce::LookAndFeel_V4
{
  private:
    juce::Font mainFont =
        juce::Font(juce::FontOptions("Oxanium", 18.0f, juce::Font::plain))
            .withExtraKerningFactor(0.2f);

  public:
    AuroraLookAndFeel();

    void drawToggleButton(
        juce::Graphics& g, juce::ToggleButton& button, bool isMouseOverButton,
        bool isButtonDown
    ) override;

    void drawRotarySlider(
        juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider
    ) override;

    void drawLinearSlider(
        juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        float minSliderPos, float maxSliderPos,
        const juce::Slider::SliderStyle style, juce::Slider& slider
    ) override;

    juce::LookAndFeel_V4::ColourScheme getColourScheme() const
    {
        return juce::LookAndFeel_V4::ColourScheme(
            AuroraColors::bg,     // windowBackground
            AuroraColors::grey3,  // widgetBackground
            AuroraColors::grey2,  // menuBackground
            AuroraColors::grey3,  // outline
            AuroraColors::grey3,  // defaultText
            AuroraColors::bg,     // defaultFill
            AuroraColors::white2, // highlightedText
            AuroraColors::blue0,  // highlightedFill
            AuroraColors::white0  // menuText
        );
    }

    juce::Font getSliderPopupFont(juce::Slider&) override
    {
        return mainFont;
    }

    juce::Font getLabelFont(juce::Label&) override
    {
        return mainFont;
    }

    juce::Font getTextButtonFont(juce::TextButton& button, int height) override
    {
        juce::ignoreUnused(button, height);
        return mainFont;
    }

    juce::Font getAlertWindowMessageFont() override
    {
        return mainFont;
    }

    juce::Font getPopupMenuFont() override
    {
        return mainFont;
    }

    juce::Font getComboBoxFont(juce::ComboBox&) override
    {
        return mainFont;
    }
};

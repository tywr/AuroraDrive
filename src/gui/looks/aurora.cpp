#include "aurora.h"

#include "colors.h"
#include <juce_gui_basics/juce_gui_basics.h>

AuroraLookAndFeel::AuroraLookAndFeel()
{
    setColourScheme(getColourScheme());
    setColour(
        juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack
    );
    setColour(
        juce::Slider::rotarySliderFillColourId, AuroraColors::aurora_green
    );
}

void AuroraLookAndFeel::drawToggleButton(
    juce::Graphics& g, juce::ToggleButton& button, bool isMouseOverButton,
    bool isButtonDown
)
{
    auto bounds = button.getLocalBounds().toFloat();
    juce::Colour colour = juce::Colours::red; // Default color

    if (button.getToggleState())
        colour = juce::Colours::green; // Color for the "on" state

    if (isButtonDown)
        colour = colour.darker(0.2f);
    else if (isMouseOverButton)
        colour = colour.brighter(0.2f);

    g.setColour(colour);
    g.fillEllipse(bounds);

    g.setColour(juce::Colours::black);
    g.drawEllipse(bounds, 1.0f);
}

void AuroraLookAndFeel::drawRotarySlider(
    juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
    float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider
)
{
    const auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();

    auto radius = fmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    const auto toAngle =
        rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = fmin(6.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;

    juce::Path backgroundArc;
    backgroundArc.addCentredArc(
        bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius, 0.0f,
        rotaryStartAngle, rotaryEndAngle, true
    );

    g.setColour(slider.findColour(juce::Slider::rotarySliderOutlineColourId));
    g.strokePath(
        backgroundArc,
        juce::PathStrokeType(
            lineW, juce::PathStrokeType::mitered, juce::PathStrokeType::square
        )
    );

    if (slider.isEnabled())
    {
        juce::Path valueArc;
        valueArc.addCentredArc(
            bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius,
            0.0f, rotaryStartAngle, toAngle, true
        );

        g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
        g.strokePath(
            valueArc, juce::PathStrokeType(
                          lineW, juce::PathStrokeType::mitered,
                          juce::PathStrokeType::square
                      )
        );
    }
}

void AuroraLookAndFeel::drawLinearSlider(
    juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
    float minSliderPos, float maxSliderPos,
    const juce::Slider::SliderStyle style, juce::Slider& slider
)
{
    g.fillAll(slider.findColour(juce::Slider::backgroundColourId));

    juce::Rectangle<float> filledTrack;
    if (style == juce::Slider::LinearBar)
    {
        DBG("minSliderPos: " << minSliderPos);
        DBG("maxSliderPos: " << maxSliderPos);
        DBG("sliderPos: " << sliderPos);
        DBG("x: " << x);
        DBG("y: " << y);
        filledTrack = juce::Rectangle<float>(x, y, sliderPos, height);
    }
    else if (style == juce::Slider::LinearBarVertical)
    {
        filledTrack = juce::Rectangle<float>(x, y, width, sliderPos);
    }
    g.setColour(slider.findColour(juce::Slider::trackColourId));
    g.fillRect(filledTrack);
}

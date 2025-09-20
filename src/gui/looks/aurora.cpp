#include "aurora.h"

#include "colors.h"
#include <juce_gui_basics/juce_gui_basics.h>

AuroraLookAndFeel::AuroraLookAndFeel()
{
    setColourScheme(getColourScheme());
}

void AuroraLookAndFeel::drawButtonBackground(
    juce::Graphics& g, juce::Button& button,
    const juce::Colour& backgroundColour, bool isMouseOverButton,
    bool isButtonDown
)
{
    auto bounds = button.getLocalBounds().toFloat();

    // Get appropriate colour based on button state
    juce::Colour colour = backgroundColour;

    // For toggle-style behavior, check if it's a TextButton with toggle state
    auto* textButton = dynamic_cast<juce::TextButton*>(&button);
    if (textButton && textButton->getClickingTogglesState() &&
        textButton->getToggleState())
    {
        colour = button.findColour(juce::TextButton::buttonOnColourId);
    }
    else
    {
        colour = button.findColour(juce::TextButton::buttonColourId);
    }

    // Apply interaction states
    if (isButtonDown)
        colour = colour.darker(0.2f);
    else if (isMouseOverButton)
        colour = colour.brighter(0.2f);

    g.setColour(colour);
    g.drawEllipse(bounds.reduced(strokeWidth), strokeWidth);
}

void AuroraLookAndFeel::drawToggleButton(
    juce::Graphics& g, juce::ToggleButton& button, bool isMouseOverButton,
    bool isButtonDown
)
{
    auto bounds = button.getLocalBounds().toFloat();
    juce::Colour colour =
        button.findColour(juce::ToggleButton::tickDisabledColourId);
    if (button.getToggleState())
        colour = button.findColour(juce::ToggleButton::tickColourId);

    if (isButtonDown)
        colour = button.findColour(juce::ToggleButton::tickDisabledColourId);
    else if (isMouseOverButton)
        colour = colour.brighter(0.2f);

    g.setColour(colour);
    g.drawEllipse(bounds.reduced(strokeWidth), strokeWidth);
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
    auto lineW = fmin(strokeWidth, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;

    juce::Path backgroundArc;
    backgroundArc.addCentredArc(
        bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius, 0.0f,
        rotaryStartAngle, rotaryEndAngle, true
    );

    g.setColour(AuroraColors::grey0);
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
    g.fillAll(AuroraColors::grey0);

    juce::Rectangle<float> filledTrack;
    if (style == juce::Slider::LinearBar)
    {
        filledTrack = juce::Rectangle<float>(x, y, sliderPos, height);
    }
    else if (style == juce::Slider::LinearBarVertical)
    {
        // revert to always display from bottom to top
        auto deltaY = sliderPos - y;
        filledTrack = juce::Rectangle<float>(x, y + deltaY, width, height);
    }
    g.setColour(slider.findColour(juce::Slider::trackColourId));
    g.fillRect(filledTrack);
}

#include "preamp_footer_look_and_feel.h"

#include "colors.h"
#include <juce_gui_basics/juce_gui_basics.h>

PreampFooterLookAndFeel::PreampFooterLookAndFeel()
{
    setColourScheme(getColourScheme());
}

void PreampFooterLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    auto bounds = label.getLocalBounds();
    juce::String text = label.getText();
    g.setFont(main_font);
    g.setColour(label.findColour(juce::Label::textColourId));
    g.drawFittedText(text, bounds, juce::Justification::centred, 1);
}

void PreampFooterLookAndFeel::drawToggleButton(
    juce::Graphics& g, juce::ToggleButton& button, bool isMouseOverButton,
    bool isButtonDown
)
{
    juce::Colour colour;
    if (button.getToggleState())
        colour =
            button.findColour(juce::ToggleButton::tickColourId); // "On" colour
    else
        colour = button.findColour(
            juce::ToggleButton::tickDisabledColourId
        ); // "Off" colour

    if (isButtonDown)
        colour = button.findColour(juce::ToggleButton::tickDisabledColourId);
    else if (isMouseOverButton)
        colour = colour.brighter(0.2f);

    g.setColour(colour);

    const float strokeWidth = 2.0f;
    auto bounds = button.getLocalBounds().toFloat().reduced(strokeWidth / 2.0f);

    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    auto area = bounds.withSizeKeepingCentre(size, size);

    auto centerX = area.getCentreX();
    auto centerY = area.getCentreY();
    auto radius = size / 2.0f;

    juce::Path powerSymbol;

    // Draw an arc with a gap at the top
    powerSymbol.addCentredArc(
        centerX, centerY, radius, radius, 0.0f,
        juce::MathConstants<float>::pi * 0.15f, // Start angle
        juce::MathConstants<float>::pi * 1.85f, // End angle
        true
    ); // Draw clockwise

    // Draw a vertical line from the center extending through the top gap
    powerSymbol.startNewSubPath(centerX, centerY);
    powerSymbol.lineTo(centerX, area.getY());

    // 5. Draw the final path with the calculated stroke width
    g.strokePath(
        powerSymbol, juce::PathStrokeType(
                         strokeWidth, juce::PathStrokeType::curved,
                         juce::PathStrokeType::rounded
                     )
    ); // Rounded ends look nicer
}

void PreampFooterLookAndFeel::drawLinearSlider(
    juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
    float minSliderPos, float maxSliderPos,
    const juce::Slider::SliderStyle style, juce::Slider& slider
)
{
    const int numberOfDots = 15;
    const float dotRadius = 4.0f; // Adjust for desired dot size
    const float dotDiameter = dotRadius * 2.0f;
    const float dotSpacing = 1.0f * width / numberOfDots;

    const float totalDotDisplayWidth =
        (numberOfDots * dotDiameter) +
        ((numberOfDots - 1) * (dotSpacing - dotDiameter));
    const float totalDotDisplayHeight =
        (numberOfDots * dotDiameter) +
        ((numberOfDots - 1) * (dotSpacing - dotDiameter)); // For vertical

    int dotsToLight = static_cast<int>((sliderPos - x) / numberOfDots);

    const juce::Colour highlightColour =
        slider.findColour(juce::Slider::trackColourId);
    const juce::Colour darkColour = AuroraColors::grey0;

    if (style == juce::Slider::LinearHorizontal)
    {
        float startX = x + (width - totalDotDisplayWidth) * 0.5f;
        float dotY = y + (height - dotDiameter) * 0.5f;

        for (int i = 0; i < numberOfDots; ++i)
        {
            float currentDotX = startX + i * dotSpacing;

            if (i < dotsToLight)
                g.setColour(highlightColour);
            else
                g.setColour(darkColour);

            g.fillEllipse(
                currentDotX - dotRadius, dotY, dotDiameter, dotDiameter
            );
        }
    }
}

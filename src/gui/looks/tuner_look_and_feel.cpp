#include "tuner_look_and_feel.h"
#include "BinaryData.h"

#include <juce_gui_basics/juce_gui_basics.h>

TunerLookAndFeel::TunerLookAndFeel()
{
    setColourScheme(getColourScheme());

    if (auto svg = juce::XmlDocument::parse(juce::String::fromUTF8(
            BinaryData::gitfork_svg, BinaryData::gitfork_svgSize
        )))
    {
        icon = juce::Drawable::createFromSVG(*svg);
    }
}

void TunerLookAndFeel::drawToggleButton(
    juce::Graphics& g, juce::ToggleButton& button, bool isMouseOverButton,
    bool isButtonDown
)
{
    juce::Colour colour = ColourCodes::white0;

    if (isButtonDown)
        colour = ColourCodes::grey3;
    else if (isMouseOverButton)
        colour = colour.brighter(0.2f);

    if (icon == nullptr)
        return;

    auto bounds = button.getLocalBounds().toFloat();
    icon->replaceColour(currentIconColour, colour);
    currentIconColour = colour;

    const float iconSize =
        juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    icon->drawWithin(
        g, bounds.withSizeKeepingCentre(iconSize, iconSize),
        juce::RectanglePlacement::centred, 1.0f
    );
}

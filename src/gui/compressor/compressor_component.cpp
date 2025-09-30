#include "compressor_component.h"
#include "../dimensions.h"
#include "../colours.h"
#include "compressor_knobs_component.h"
#include "compressor_meter_component.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

CompressorComponent::CompressorComponent(
    juce::AudioProcessorValueTreeState& params, juce::Value& value
)
    : parameters(params), gain_reduction_decibels(value),
      knobs_component(params), meter_component(params, value)
{
    addAndMakeVisible(knobs_component);
    addAndMakeVisible(meter_component);
    addAndMakeVisible(bypass_button);

    bypass_attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, "compressor_bypass", bypass_button
        );

    bypass_button.setColour(
        juce::ToggleButton::tickColourId, GuiColours::DEFAULT_INACTIVE_COLOUR
    );
    bypass_button.setColour(
        juce::ToggleButton::tickDisabledColourId,
        GuiColours::COMPRESSOR_ACTIVE_COLOUR_1
    );
    bypass_button.onClick = [this]() { repaint(); };
}

CompressorComponent::~CompressorComponent()
{
}

void CompressorComponent::paint(juce::Graphics& g)
{
    bool bypass = bypass_button.getToggleState();
    juce::Colour colour1;
    juce::Colour colour2;
    if (!bypass)
    {
        colour1 = GuiColours::COMPRESSOR_ACTIVE_COLOUR_1;
        colour2 = GuiColours::COMPRESSOR_ACTIVE_COLOUR_2;
    }
    else
    {
        colour1 = GuiColours::DEFAULT_INACTIVE_COLOUR;
        colour2 = GuiColours::DEFAULT_INACTIVE_COLOUR;
    }
    float border_thickness = GuiDimensions::COMPRESSOR_BORDER_THICKNESS;
    float border_radius = GuiDimensions::COMPRESSOR_BORDER_RADIUS;

    auto outer_bounds = getLocalBounds().withSizeKeepingCentre(
        GuiDimensions::COMPRESSOR_WIDTH, GuiDimensions::COMPRESSOR_HEIGHT
    ).toFloat();
    auto inner_bounds = outer_bounds.reduced(border_thickness).toFloat();

    g.setColour(GuiColours::COMPRESSOR_BG_COLOUR);
    g.fillRoundedRectangle(inner_bounds, border_radius);

    juce::Path border_path;
    border_path.addRoundedRectangle(
        outer_bounds, border_radius + border_thickness
    );
    border_path.addRoundedRectangle(inner_bounds, border_radius);
    border_path.setUsingNonZeroWinding(false);

    juce::ColourGradient border_gradient(
        colour1, outer_bounds.getTopLeft(), colour2,
        outer_bounds.getBottomLeft(), false
    );
    g.setGradientFill(border_gradient);
    g.fillPath(border_path);

    knobs_component.switchColour(colour1, colour2);
    meter_component.switchColour(colour1, colour2);

    paintStyling(
        g, inner_bounds.withTrimmedBottom(
               GuiDimensions::COMPRESSOR_GAIN_REDUCTION_HEIGHT +
               GuiDimensions::COMPRESSOR_FOOTER_HEIGHT
           )
    );
}

void CompressorComponent::paintStyling(
    juce::Graphics& g, juce::Rectangle<float> bounds
)
{
    auto centre = bounds.getCentre();

    // juce::Colour colour = juce::Colours::white;
    juce::Colour colour = GuiColours::COMPRESSOR_STYLING_COLOUR;
    g.setColour(colour);

    const int spacing = 25;           // grid spacing
    const float eventHorizon = 60.0f; // radius of black hole
    const float distortion = 5000.0f;
    const float dotSize = 4.0f;

    // compute number of dots
    int numX = static_cast<int>(std::floor(bounds.getWidth() / spacing));
    int numY = static_cast<int>(std::floor(bounds.getHeight() / spacing));

    // total grid size
    float totalWidth = numX * spacing;
    float totalHeight = numY * spacing;

    // start coordinates to center the grid exactly
    float startX = centre.x - totalWidth / 2.0f + spacing / 2.0f;
    float startY = centre.y - totalHeight / 2.0f + spacing / 2.0f;

    for (int iy = 0; iy < numY; ++iy)
    {
        for (int ix = 0; ix < numX; ++ix)
        {
            float x = startX + ix * spacing;
            float y = startY + iy * spacing;

            float dx = x - centre.x;
            float dy = y - centre.y;
            float dist = std::sqrt(dx * dx + dy * dy);

            juce::Point<float> pos(x, y);

            if (dist > eventHorizon)
            {
                // symmetric radial warp
                float warpedDist = dist / (1.0f + distortion / (dist * dist));
                float scale = warpedDist / dist;

                pos.x = centre.x + dx * scale;
                pos.y = centre.y + dy * scale;
            }
            else
            {
                pos = centre;
            }

            float alpha =
                juce::jlimit(0.2f, 1.0f, dist / (bounds.getWidth() * 0.5f));
            g.setColour(colour.withAlpha(alpha));

            g.fillEllipse(
                pos.x - dotSize * 0.5f, pos.y - dotSize * 0.5f, dotSize, dotSize
            );
        }
    }

    // Draw the event horizon
    g.setColour(GuiColours::COMPRESSOR_BG_COLOUR);
    g.fillEllipse(
        centre.x - eventHorizon, centre.y - eventHorizon, eventHorizon * 2.0f,
        eventHorizon * 2.0f
    );
}

void CompressorComponent::resized()
{
    auto bounds = getLocalBounds().withSizeKeepingCentre(
        GuiDimensions::COMPRESSOR_WIDTH, GuiDimensions::COMPRESSOR_HEIGHT
    );
    bypass_button.setBounds(
        bounds.removeFromBottom(GuiDimensions::COMPRESSOR_FOOTER_HEIGHT)
            .withSizeKeepingCentre(
                GuiDimensions::COMPRESSOR_BYPASS_BUTTON_WIDTH,
                GuiDimensions::COMPRESSOR_BYPASS_BUTTON_HEIGHT
            )
    );
    meter_component.setBounds(
        bounds.removeFromBottom(GuiDimensions::COMPRESSOR_GAIN_REDUCTION_HEIGHT)
    );
    bounds.removeFromTop(GuiDimensions::COMPRESSOR_INNER_Y_TOP_PADDING);
    knobs_component.setBounds(bounds.removeFromTop(
        GuiDimensions::COMPRESSOR_KNOBS_TOP_BOX_HEIGHT +
        GuiDimensions::COMPRESSOR_KNOBS_BOTTOM_BOX_HEIGHT +
        GuiDimensions::COMPRESSOR_KNOBS_ROW_PADDING
    ));
}

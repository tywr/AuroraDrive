#include "compressor_component.h"
#include "../dimensions.h"
#include "../looks/colors.h"
#include "compressor_footer_component.h"
#include "compressor_knobs_component.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

CompressorComponent::CompressorComponent(
    juce::AudioProcessorValueTreeState& params, juce::Value& value
)
    : parameters(params), gain_reduction_decibels(value),
      knobs_component(params), footer_component(params, value)
{
    addAndMakeVisible(knobs_component);
    addAndMakeVisible(footer_component);
    addAndMakeVisible(bypass_button);
    addAndMakeVisible(type_slider);

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

    type_slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    type_slider.setTextBoxStyle(
        juce::Slider::TextBoxBelow, false, 100, 20
    );
    type_slider.setColour(
        juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack
    );
    type_slider.setColour(
        juce::Slider::textBoxTextColourId, AuroraColors::grey3
    );
    type_slider_attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "compressor_type", type_slider
        );
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

    auto outer_bounds = getLocalBounds()
                            .reduced(
                                GuiDimensions::COMPRESSOR_OUTER_X_PADDING,
                                GuiDimensions::COMPRESSOR_OUTER_Y_PADDING
                            )
                            .toFloat();
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
    footer_component.switchColour(colour1, colour2);
}

void CompressorComponent::resized()
{
    auto bounds = getLocalBounds()
                      .reduced(
                          GuiDimensions::COMPRESSOR_OUTER_X_PADDING,
                          GuiDimensions::COMPRESSOR_OUTER_Y_PADDING
                      )
                      .reduced(GuiDimensions::COMPRESSOR_BORDER_THICKNESS);
    bypass_button.setBounds(
        bounds.removeFromBottom(GuiDimensions::COMPRESSOR_FOOTER_HEIGHT)
            .withSizeKeepingCentre(
                GuiDimensions::COMPRESSOR_BYPASS_BUTTON_WIDTH,
                GuiDimensions::COMPRESSOR_BYPASS_BUTTON_HEIGHT
            )
    );
    footer_component.setBounds(
        bounds.removeFromBottom(GuiDimensions::COMPRESSOR_GAIN_REDUCTION_HEIGHT)
    );
    bounds.removeFromTop(GuiDimensions::COMPRESSOR_INNER_Y_TOP_PADDING);
    knobs_component.setBounds(
        bounds.removeFromTop(GuiDimensions::COMPRESSOR_KNOBS_BOX_HEIGHT)
    );
    type_slider.setBounds(bounds.removeFromTop(150));
}

#include "chorus_component.h"
#include "../colours.h"
#include "../dimensions.h"
#include "chorus_dimensions.h"
#include "chorus_knobs_component.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

ChorusComponent::ChorusComponent(juce::AudioProcessorValueTreeState& params)
    : parameters(params), knobs_component(params)
{
    addAndMakeVisible(title_label);
    addAndMakeVisible(knobs_component);
    addAndMakeVisible(bypass_button);

    title_label.setText("CHORUS", juce::dontSendNotification);
    title_label.setJustificationType(juce::Justification::centredLeft);

    bypass_attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, "chorus_bypass", bypass_button
        );

    bypass_button.setColour(
        juce::ToggleButton::tickColourId, GuiColours::DEFAULT_INACTIVE_COLOUR
    );
    bypass_button.setColour(
        juce::ToggleButton::tickDisabledColourId, ColourCodes::orange
    );
    bypass_button.onClick = [this]() { repaint(); };
}

ChorusComponent::~ChorusComponent()
{
}

void ChorusComponent::paint(juce::Graphics& g)
{
    bool bypass = bypass_button.getToggleState();
    juce::Colour colour1, colour2;
    if (!bypass)
    {
        colour1 = ColourCodes::orange;
        colour2 = ColourCodes::white0;
    }
    else
    {
        colour1 = GuiColours::DEFAULT_INACTIVE_COLOUR;
        colour2 = ColourCodes::grey0;
    }

    auto bounds = getLocalBounds();

    title_label.setColour(juce::Label::textColourId, colour2);

    g.setColour(GuiColours::COMPRESSOR_BG_COLOUR);
    g.fillRoundedRectangle(
        bounds.toFloat(), (float)GuiDimensions::BORDER_RADIUS
    );

    knobs_component.switchColour(colour1, colour2);
}

void ChorusComponent::resized()
{
    auto bounds = getLocalBounds();

    // Title bar with label and bypass button
    auto title_bounds =
        bounds.removeFromTop(GuiDimensions::PANEL_TITLE_BAR_HEIGHT);
    title_bounds.removeFromLeft(GuiDimensions::BYPASS_BUTTON_PADDING);
    bypass_button.setBounds(
        title_bounds
            .removeFromLeft(GuiDimensions::BYPASS_BUTTON_WIDTH)
            .reduced(GuiDimensions::PANEL_BORDER_THICKNESS)
    );
    title_bounds.removeFromLeft(GuiDimensions::PANEL_KNOB_PADDING);
    title_label.setBounds(title_bounds);

    // Knobs area
    knobs_component.setBounds(bounds);
}

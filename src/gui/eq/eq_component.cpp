#include "eq_component.h"
#include "../colours.h"
#include "../dimensions.h"
#include "eq_dimensions.h"
#include "eq_sliders_component.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

EqComponent::EqComponent(juce::AudioProcessorValueTreeState& params)
    : parameters(params), sliders_component(params)
{
    addAndMakeVisible(title_label);
    title_label.setText("EQUALIZER", juce::dontSendNotification);
    title_label.setJustificationType(juce::Justification::centredLeft);

    addAndMakeVisible(sliders_component);

    addAndMakeVisible(bypass_button);
    bypass_button.setButtonText("bypass");
    bypass_button.setColour(
        juce::ToggleButton::tickColourId, GuiColours::DEFAULT_INACTIVE_COLOUR
    );
    bypass_button.setColour(
        juce::ToggleButton::tickDisabledColourId, ColourCodes::orange
    );
    bypass_button.onClick = [this]() { repaint(); };

    bypass_attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, "eq_bypass", bypass_button
        );
}

EqComponent::~EqComponent()
{
}

void EqComponent::paint(juce::Graphics& g)
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

    auto full_bounds = getLocalBounds();

    title_label.setColour(juce::Label::textColourId, colour2);

    g.setColour(GuiColours::COMPRESSOR_BG_COLOUR);
    g.fillRoundedRectangle(
        full_bounds.toFloat(), (float)GuiDimensions::BORDER_RADIUS
    );

    sliders_component.switchColour(colour1, colour2);
}

void EqComponent::resized()
{
    auto full_bounds = getLocalBounds();

    // Title bar with label and bypass button
    auto title_bounds =
        full_bounds.removeFromTop(GuiDimensions::PANEL_TITLE_BAR_HEIGHT);
    title_bounds.removeFromLeft(GuiDimensions::BYPASS_BUTTON_PADDING);
    bypass_button.setBounds(
        title_bounds
            .removeFromLeft(GuiDimensions::BYPASS_BUTTON_WIDTH)
            .reduced(GuiDimensions::PANEL_BORDER_THICKNESS)
    );
    title_bounds.removeFromLeft(GuiDimensions::PANEL_KNOB_PADDING);
    title_label.setBounds(title_bounds);

    // Position sliders in remaining space
    sliders_component.setBounds(full_bounds);
}

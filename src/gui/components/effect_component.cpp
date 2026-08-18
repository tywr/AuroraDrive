#include "effect_component.h"

#include "../colours.h"
#include "../dimensions.h"

EffectComponent::EffectComponent(
    juce::AudioProcessorValueTreeState& parameters,
    const juce::String& title,
    const juce::String& bypassParameterId
)
{
    addAndMakeVisible(titleLabel);
    titleLabel.setText(title, juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centredLeft);

    addAndMakeVisible(bypassButton);
    bypassButton.setColour(
        juce::ToggleButton::tickColourId,
        GuiColours::DEFAULT_INACTIVE_COLOUR
    );
    bypassButton.setColour(
        juce::ToggleButton::tickDisabledColourId, ColourCodes::orange
    );
    bypassButton.onClick = [this]()
    {
        bypassStateChanged();
        repaint();
    };

    bypassAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, bypassParameterId, bypassButton
        );
}

void EffectComponent::paint(juce::Graphics& g)
{
    titleLabel.setColour(juce::Label::textColourId, getTextColour());

    g.setColour(GuiColours::PANEL_BACKGROUND);
    g.fillRoundedRectangle(
        getLocalBounds().toFloat(), (float)GuiDimensions::BORDER_RADIUS
    );

    paintContent(g);
}

void EffectComponent::resized()
{
    auto contentBounds = getLocalBounds();
    auto titleBounds =
        contentBounds.removeFromTop(GuiDimensions::PANEL_TITLE_BAR_HEIGHT);

    titleBounds.removeFromLeft(GuiDimensions::BYPASS_BUTTON_PADDING);
    bypassButton.setBounds(
        titleBounds
            .removeFromLeft(GuiDimensions::BYPASS_BUTTON_WIDTH)
            .reduced(GuiDimensions::PANEL_BORDER_THICKNESS)
    );
    titleBounds.removeFromLeft(GuiDimensions::PANEL_KNOB_PADDING);
    titleLabel.setBounds(titleBounds);

    resizedContent(contentBounds);
}

bool EffectComponent::isBypassed() const
{
    return bypassButton.getToggleState();
}

juce::Colour EffectComponent::getAccentColour() const
{
    return isBypassed() ? GuiColours::DEFAULT_INACTIVE_COLOUR
                        : ColourCodes::orange;
}

juce::Colour EffectComponent::getTextColour() const
{
    return isBypassed() ? ColourCodes::grey0 : ColourCodes::white0;
}

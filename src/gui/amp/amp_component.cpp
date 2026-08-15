#include "amp_component.h"
#include "../colours.h"
#include "../dimensions.h"
#include "amp_dimensions.h"
#include "amp_knobs_component.h"
#include "designs/borealis.h"
#include "designs/helios.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

AmpComponent::AmpComponent(juce::AudioProcessorValueTreeState& params)
    : parameters(params), knobs_component(params)
{
    addAndMakeVisible(title_label);
    addAndMakeVisible(knobs_component);
    addAndMakeVisible(bypass_button);

    title_label.setText("OVERDRIVE", juce::dontSendNotification);
    title_label.setJustificationType(juce::Justification::centred);

    bypass_attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, "amp_bypass", bypass_button
        );
    bypass_button.setColour(
        juce::ToggleButton::tickColourId, GuiColours::DEFAULT_INACTIVE_COLOUR
    );
    bypass_button.setColour(
        juce::ToggleButton::tickDisabledColourId, ColourCodes::orange
    );
    bypass_button.onClick = [this]()
    {
        is_cache_dirty = true;
        repaint();
    };
    knobs_component.switchType();
}

AmpComponent::~AmpComponent()
{
}

void AmpComponent::paintDesign(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    paintDesignHelios(g, bounds, current_colour1, current_colour2);
}

void AmpComponent::paintBorder(
    juce::Graphics& g, juce::Rectangle<float> bounds, float border_radius
)
{
    float border_thickness = AmpDimensions::AMP_BORDER_THICKNESS;

    auto outer_bounds = bounds.toFloat();
    auto inner_bounds = bounds.reduced(border_thickness).toFloat();

    g.setColour(GuiColours::COMPRESSOR_BG_COLOUR);
    g.fillRoundedRectangle(inner_bounds, border_radius);

    juce::Path border_path;
    border_path.addRoundedRectangle(
        outer_bounds, border_radius + border_thickness
    );
    border_path.addRoundedRectangle(inner_bounds, border_radius);
    border_path.setUsingNonZeroWinding(false);

    juce::ColourGradient border_gradient(
        current_colour1, outer_bounds.getTopLeft(), current_colour2,
        outer_bounds.getBottomLeft(), false
    );
    g.setGradientFill(border_gradient);
    g.fillPath(border_path);
}

void AmpComponent::paint(juce::Graphics& g)
{
    bool bypass = bypass_button.getToggleState();
    juce::Colour colour1, colour2, colour3;

    if (!bypass)
    {
        colour1 = ColourCodes::orange;
        colour2 = ColourCodes::orange_light;
        colour3 = ColourCodes::white0;
    }
    else
    {
        colour1 = GuiColours::DEFAULT_INACTIVE_COLOUR;
        colour2 = ColourCodes::grey0;
        colour3 = ColourCodes::grey0;
    }

    auto bounds = getLocalBounds();

    title_label.setColour(juce::Label::textColourId, colour3);

    g.setColour(GuiColours::AMP_BG_COLOUR);
    g.fillRoundedRectangle(
        bounds.toFloat(), (float)GuiDimensions::BORDER_RADIUS
    );

    // Get design area bounds
    bounds.removeFromTop(GuiDimensions::PANEL_TITLE_BAR_HEIGHT);
    auto design_bounds = bounds.removeFromTop(bounds.getHeight() / 2);

    float scale = g.getInternalContext().getPhysicalPixelScaleFactor();
    if (is_cache_dirty)
    {
        buildCache(scale, colour1, colour2);
        is_cache_dirty = false;
    }
    g.drawImage(background_cache, design_bounds.toFloat());

    knobs_component.switchColour(colour1, colour2);
}

void AmpComponent::resized()
{
    is_cache_dirty = true;
    auto bounds = getLocalBounds();
    auto height = bounds.getHeight() - GuiDimensions::PANEL_TITLE_BAR_HEIGHT;

    // Title bar with label and bypass button
    auto title_bounds =
        bounds.removeFromTop(GuiDimensions::PANEL_TITLE_BAR_HEIGHT);
    title_label.setBounds(title_bounds);
    title_bounds.removeFromRight(GuiDimensions::BYPASS_BUTTON_PADDING);
    bypass_button.setBounds(
        title_bounds
            .removeFromRight(GuiDimensions::BYPASS_BUTTON_WIDTH)
            .reduced(GuiDimensions::PANEL_BORDER_THICKNESS)
    );

    // Design area (upper half)
    auto design_bounds = bounds.removeFromTop(height / 2);

    // Knobs area (lower half)
    knobs_component.setBounds(bounds);
}

void AmpComponent::buildCache(
    float scale, juce::Colour colour1, juce::Colour colour2
)
{
    auto bounds = getLocalBounds();
    auto height = bounds.getHeight() - GuiDimensions::PANEL_TITLE_BAR_HEIGHT;
    bounds.removeFromTop(GuiDimensions::PANEL_TITLE_BAR_HEIGHT);
    auto design_bounds = bounds.removeFromTop(height / 2);

    int width = static_cast<int>(scale * design_bounds.getWidth());
    int cache_height = static_cast<int>(scale * design_bounds.getHeight());
    background_cache =
        juce::Image(juce::Image::ARGB, width, cache_height, true);

    juce::Graphics cache(background_cache);
    cache.addTransform(juce::AffineTransform::scale(scale));

    // Store colours for paintDesign to use
    current_colour1 = colour1;
    current_colour2 = colour2;

    // Draw the Helios design into the cache
    paintDesign(
        cache, juce::Rectangle<float>(
                   0, 0, design_bounds.getWidth(), design_bounds.getHeight()
               )
    );
}

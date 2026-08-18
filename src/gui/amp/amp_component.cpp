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
    : EffectComponent(params, "OVERDRIVE", "amp_bypass"),
      knobs_component(params)
{
    addAndMakeVisible(knobs_component);
    knobs_component.switchType();
}

void AmpComponent::bypassStateChanged()
{
    is_cache_dirty = true;
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

void AmpComponent::paintContent(juce::Graphics& g)
{
    const auto colour1 = getAccentColour();
    const auto colour2 =
        isBypassed() ? ColourCodes::grey0 : ColourCodes::orange_light;
    auto bounds = getLocalBounds();

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

void AmpComponent::resizedContent(juce::Rectangle<int> bounds)
{
    is_cache_dirty = true;
    bounds.removeFromTop(bounds.getHeight() / 2);

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

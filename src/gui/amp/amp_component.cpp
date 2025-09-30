#include "amp_component.h"
#include "../colours.h"
#include "../dimensions.h"
#include "eq_knobs_component.h"
#include "overdrive_knobs_component.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

AmpComponent::AmpComponent(juce::AudioProcessorValueTreeState& params)
    : parameters(params), overdrive_knobs(params), eq_knobs(params)
{

    addAndMakeVisible(overdrive_knobs);
    addAndMakeVisible(eq_knobs);
    addAndMakeVisible(bypass_button);
}

AmpComponent::~AmpComponent()
{
}

void AmpComponent::setColours(juce::Colour c1, juce::Colour c2)
{
    selected_colour1 = c1;
    selected_colour2 = c2;
}

void AmpComponent::paint(juce::Graphics& g)
{
    bool bypass = bypass_button.getToggleState();
    juce::Colour colour1;
    juce::Colour colour2;
    if (!bypass)
    {
        colour1 = selected_colour1;
        colour2 = selected_colour2;
    }
    else
    {
        colour1 = GuiColours::DEFAULT_INACTIVE_COLOUR;
        colour2 = GuiColours::DEFAULT_INACTIVE_COLOUR;
    }
    float border_thickness = GuiDimensions::AMP_BORDER_THICKNESS;
    float border_radius = GuiDimensions::AMP_BORDER_RADIUS;

    auto outer_bounds =
        getLocalBounds()
            .withSizeKeepingCentre(
                GuiDimensions::AMP_WIDTH, GuiDimensions::AMP_HEIGHT
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

    overdrive_knobs.switchColour(colour1, colour2);
    eq_knobs.switchColour(colour1, colour2);
}

void AmpComponent::resized()
{
    auto bounds = getLocalBounds().withSizeKeepingCentre(
        GuiDimensions::AMP_WIDTH, GuiDimensions::AMP_HEIGHT
    );
    auto middle_bounds = bounds.reduced(
        GuiDimensions::AMP_SIDE_WIDTH, GuiDimensions::AMP_INNER_Y_PADDING
    );
    overdrive_knobs.setBounds(
        middle_bounds.removeFromTop(GuiDimensions::AMP_KNOBS_TOP_BOX_HEIGHT)
    );
    eq_knobs.setBounds(middle_bounds.removeFromBottom(
        GuiDimensions::AMP_KNOBS_BOTTOM_BOX_HEIGHT
    ));

    // Remaining space for equalizer
}

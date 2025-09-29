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
}

CompressorComponent::~CompressorComponent()
{
}

void CompressorComponent::paint(juce::Graphics& g)
{
    // g.fillAll(AuroraColors::bg0);
    juce::Rectangle<float> bounds = getLocalBounds().toFloat().reduced(
        GuiDimensions::COMPRESSOR_OUTER_X_PADDING,
        GuiDimensions::COMPRESSOR_OUTER_Y_PADDING
    );
    g.setColour(AuroraColors::bg1);
    g.fillRoundedRectangle(bounds, 15.0f);
}

void CompressorComponent::resized()
{
    auto bounds = getLocalBounds().reduced(
        GuiDimensions::COMPRESSOR_OUTER_X_PADDING,
        GuiDimensions::COMPRESSOR_OUTER_Y_PADDING
    );
    footer_component.setBounds(bounds.removeFromBottom(
        GuiDimensions::COMPRESSOR_FOOTER_HEIGHT +
        GuiDimensions::COMPRESSOR_GAIN_REDUCTION_HEIGHT
    ));
    knobs_component.setBounds(
        bounds.removeFromTop(GuiDimensions::COMPRESSOR_KNOBS_BOX_HEIGHT)
    );
}

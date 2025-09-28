#include "header.h"

#include "compressor_left_component.h"
#include "dimensions.h"
#include "looks/colors.h"
#include "overdrive_component.h"
#include "preamp_component.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

PreAmpComponent::PreAmpComponent(
    juce::AudioProcessorValueTreeState& params, juce::Value& value
)
    : parameters(params), compressor_gain_reduction_decibels(value),
      compressor_left_component(params), overdrive_component(params)
{

    addAndMakeVisible(overdrive_component);
    addAndMakeVisible(compressor_left_component);
}

PreAmpComponent::~PreAmpComponent()
{
}

void PreAmpComponent::paint(juce::Graphics& g)
{
    g.fillAll(AuroraColors::bg);
}

void PreAmpComponent::resized()
{
    auto bounds = getLocalBounds().reduced(GuiDimensions::TAB_INNER_PADDING);
    // Placeholders for header and footer
    bounds.removeFromTop(GuiDimensions::TAB_HEADER_HEIGHT);
    bounds.removeFromBottom(GuiDimensions::TAB_FOOTER_HEIGHT);

    overdrive_component.setBounds(
        bounds.removeFromTop(GuiDimensions::PREAMP_TOP_KNOB_BOX_HEIGHT)
    );

    // Side buttons
    compressor_left_component.setBounds(
        bounds.removeFromLeft(GuiDimensions::PREAMP_SIDE_WIDTH)
    );
    bounds.removeFromLeft(GuiDimensions::PREAMP_SIDE_WIDTH);

    // Remaining space for equalizer
}

#include "header.h"

#include "compressor_component.h"
#include "looks/colors.h"
#include "overdrive_component.h"
#include "preamp_component.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

PreAmpComponent::PreAmpComponent(
    juce::AudioProcessorValueTreeState& params, juce::Value& value
)
    : parameters(params), compressor_gain_reduction_decibels(value),
      compressor_component(params), overdrive_component(params)
{

    addAndMakeVisible(overdrive_component);
    addAndMakeVisible(compressor_component);
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
    auto bounds = getLocalBounds();
    compressor_component.setBounds(bounds.removeFromLeft(bounds.getWidth() / 2));
    overdrive_component.setBounds(bounds);
}

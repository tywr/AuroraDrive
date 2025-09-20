#include "plugin_editor.h"

#include "gui/looks/aurora.h"
#include "plugin_audio_processor.h"

using namespace juce;

//==============================================================================
PluginEditor::PluginEditor(
    PluginAudioProcessor& p, juce::AudioProcessorValueTreeState& params
)
    : AudioProcessorEditor(&p), processorRef(p), parameters(params),
      header(params, processorRef.inputLevel, processorRef.outputLevel),
      tabs(params, processorRef.compressorGainReductionDb)
{

    setLookAndFeel(new AuroraLookAndFeel());
    setSize(800, 600);
    addAndMakeVisible(header);
    addAndMakeVisible(tabs);

    juce::ignoreUnused(processorRef);

    getLookAndFeel().setColour(
        juce::Slider::thumbColourId, juce::Colours::lightgreen
    );
}

PluginEditor::~PluginEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void PluginEditor::paint(juce::Graphics& g)
{
    g.setFont(18.0f);
}

void PluginEditor::resized()
{
    const float header_ratio = 0.1f;
    const int header_height = static_cast<int>(getHeight() * header_ratio);
    auto bounds = getLocalBounds();
    header.setBounds(bounds.removeFromTop(header_height));
    tabs.setBounds(bounds);
}

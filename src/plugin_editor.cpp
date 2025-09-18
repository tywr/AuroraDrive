#include "plugin_editor.h"

#include "plugin_audio_processor.h"

using namespace juce;

//==============================================================================
PluginEditor::PluginEditor(PluginAudioProcessor& p,
                           juce::AudioProcessorValueTreeState& params)
    : AudioProcessorEditor(&p), processorRef(p), parameters(params),
      header(params, processorRef.inputLevel, processorRef.outputLevel),
      tabs(params) {

    setSize(600, 600);

    addAndMakeVisible(header);
    addAndMakeVisible(tabs);

    juce::ignoreUnused(processorRef);

    getLookAndFeel().setColour(juce::Slider::thumbColourId,
                               juce::Colours::lightgreen);
}

PluginEditor::~PluginEditor() {}

//==============================================================================
void PluginEditor::paint(juce::Graphics& g) {
    // (Our component is opaque, so we must completely fill the background with
    // a solid colour)
    g.setFont(15.0f);
}

void PluginEditor::changeListenerCallback(juce::ChangeBroadcaster* source) {
    if (source == static_cast<juce::ChangeBroadcaster*>(&processorRef)) {
        auto* processor = static_cast<PluginAudioProcessor*>(source);
        setMeterSliders(processor);
    }
}

void PluginEditor::setMeterSliders(PluginAudioProcessor* p) {}

void PluginEditor::sliderValueChanged(juce::Slider* slider) {}

void PluginEditor::resized() {
    const int header_height = 100;
    auto bounds = getLocalBounds();
    tabs.setBounds(bounds.removeFromBottom(getHeight() - header_height));
    header.setBounds(bounds.removeFromTop(header_height));
}

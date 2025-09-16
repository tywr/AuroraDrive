#include "plugin_editor.h"

#include "plugin_processor.h"

using namespace juce;

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(
    AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p) {
    juce::ignoreUnused(processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(600, 600);

    addAndMakeVisible(inGainSlider);
    addAndMakeVisible(inGainLabel);
    inGainSlider.setRange(-100.0, 60.0, 0.01);
    inGainSlider.setSkewFactor(3.0);
    inGainSlider.setValue(24.0);
    inGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    inGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 25);
    inGainLabel.setText("Input Gain", dontSendNotification);
    inGainLabel.attachToComponent(&inGainSlider, false);

    addAndMakeVisible(outGainSlider);
    addAndMakeVisible(outGainLabel);
    outGainSlider.setRange(-100.0, 20.0, 0.01);
    outGainSlider.setSkewFactor(3.0);
    outGainSlider.setValue(-18.0);
    outGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    outGainLabel.setText("Output Gain", dontSendNotification);
    outGainLabel.attachToComponent(&outGainSlider, false);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() {}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g) {
    // (Our component is opaque, so we must completely fill the background with
    // a solid colour)
    g.setFont(15.0f);
}

void AudioPluginAudioProcessorEditor::resized() {
    auto xmargin = 100;
    auto ymargin = 100;
    auto sliderGainSize = 100;
    inGainSlider.setBounds(xmargin, ymargin, sliderGainSize, sliderGainSize);
}

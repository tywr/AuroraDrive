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

    processorRef.addChangeListener(this);

    getLookAndFeel().setColour(juce::Slider::thumbColourId,
                               juce::Colours::lightgreen);

    setupGainControl(inGainSlider, inGainLabel, -100.0, 60.0, 24.0, "Input");
    setupGainControl(outGainSlider, outGainLabel, -100.0, 20.0, -18.0,
                     "Output");
    setupGainMeter(inGainMeterSlider, -100.0, 6.0);
    setupGainMeter(outGainMeterSlider, -100.0, 6.0);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() {}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g) {
    // (Our component is opaque, so we must completely fill the background with
    // a solid colour)
    g.setFont(15.0f);
}

void AudioPluginAudioProcessorEditor::changeListenerCallback(
    juce::ChangeBroadcaster* source) {
    if (source == static_cast<juce::ChangeBroadcaster*>(&processorRef)) {
        // Cast the source to your processor type to access its methods
        auto* processor = static_cast<AudioPluginAudioProcessor*>(source);

        float inputLevel = processor->getInputLevel();
        float outputLevel = processor->getOutputLevel();

        inGainMeterSlider.setValue(juce::Decibels::gainToDecibels(inputLevel));
        outGainMeterSlider.setValue(
            juce::Decibels::gainToDecibels(outputLevel));

        DBG("Input Level: " << inGainMeterSlider.getValue() << "dB");

        repaint();
    }
}

void AudioPluginAudioProcessorEditor::resized() { resizeGainControls(); }

void AudioPluginAudioProcessorEditor::setupGainControl(
    juce::Slider& slider, juce::Label& label, double minRange, double maxRange,
    double initialValue, const juce::String& labelText) {
    addAndMakeVisible(slider);
    addAndMakeVisible(label);
    slider.setRange(minRange, maxRange, 0.01);
    slider.setSkewFactor(3.0);
    slider.setValue(initialValue);
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 25);
    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
}

void AudioPluginAudioProcessorEditor::setupGainMeter(juce::Slider& slider,
                                                     double min_range,
                                                     double max_range) {
    addAndMakeVisible(slider);
    slider.setRange(min_range, max_range, 0.01);
    slider.setSliderStyle(juce::Slider::LinearBarVertical);
    slider.setSkewFactor(3.0);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
}

void AudioPluginAudioProcessorEditor::resizeGainControls() {
    auto bounds = getLocalBounds();
    // Define a padding around the content.
    const int padding = 10;
    auto content_area = bounds.reduced(padding);
    auto gain_width = content_area.getWidth() / 5;
    auto gain_height = content_area.getHeight() / 5;
    auto gain_meter_width = gain_width / 5;
    auto label_height = 25;

    auto in_gain_area =
        content_area.removeFromLeft(gain_width).removeFromTop(gain_height);
    auto in_gain_meter_area = in_gain_area.removeFromLeft(gain_meter_width);
    auto in_gain_slider_area =
        in_gain_area.removeFromRight(gain_width - gain_meter_width);
    inGainLabel.setBounds(in_gain_slider_area.removeFromBottom(label_height));
    inGainSlider.setBounds(in_gain_slider_area);
    inGainMeterSlider.setBounds(in_gain_meter_area);

    auto out_gain_area =
        content_area.removeFromRight(gain_width).removeFromTop(gain_height);
    auto out_gain_meter_area = out_gain_area.removeFromRight(gain_meter_width);
    auto out_gain_slider_area =
        out_gain_area.removeFromRight(gain_width - gain_meter_width);
    outGainLabel.setBounds(out_gain_slider_area.removeFromBottom(label_height));
    outGainSlider.setBounds(out_gain_slider_area);
    outGainMeterSlider.setBounds(out_gain_meter_area);
}

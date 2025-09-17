#include "plugin_editor.h"

#include "plugin_audio_processor.h"

using namespace juce;

//==============================================================================
PluginEditor::PluginEditor(PluginAudioProcessor& p,
                           juce::AudioProcessorValueTreeState& params)
    : AudioProcessorEditor(&p), processorRef(p), parameters(params),
      header(params, processorRef.inputLevel, processorRef.outputLevel) {

    setSize(600, 600);

    addAndMakeVisible(header);

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
    auto bounds = getLocalBounds();
    const int header_height = 100;
    header.setBounds(bounds.removeFromTop(header_height));
}

void PluginEditor::setupGainControl(juce::Slider& slider, juce::Label& label,
                                    double minRange, double maxRange,
                                    double initialValue,
                                    const juce::String& labelText) {
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

void PluginEditor::setupGainMeter(juce::Slider& slider, double min_range,
                                  double max_range) {
    addAndMakeVisible(slider);
    slider.setRange(min_range, max_range, 0.01);
    slider.setSliderStyle(juce::Slider::LinearBarVertical);
    slider.setSkewFactor(3.0);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
}

void PluginEditor::resizeGainControls() {
    auto bounds = getLocalBounds();
    // Define a padding around the content.
    const int padding = 10;
    auto content_area = bounds.reduced(padding);
    auto gain_width = content_area.getWidth() / 5;
    auto gain_height = content_area.getHeight() / 5;
    auto gain_meter_width = gain_width / 10;
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

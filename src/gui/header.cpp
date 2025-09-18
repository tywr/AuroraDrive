#include "header.h"

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

Header::Header(juce::AudioProcessorValueTreeState& params, juce::Value& vin,
               juce::Value& vout)
    : parameters(params), inputMeter(vin), outputMeter(vout) {

    addAndMakeVisible(inputMeter);
    addAndMakeVisible(outputMeter);

    addAndMakeVisible(inputGainSlider);
    addAndMakeVisible(outputGainSlider);
    inputGainSlider.setSkewFactor(3.0);
    inputGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    inputGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 25);
    inputGainAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "input_gain_db", inputGainSlider);

    outputGainSlider.setSkewFactor(3.0);
    outputGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    outputGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 25);
    outputGainAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "output_gain_db", outputGainSlider);
}

Header::~Header() {}

void Header::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::green);
}

void Header::resized() {
    int padding = 10;
    auto bounds = getLocalBounds().reduced(padding);
    auto gain_width = bounds.getWidth() / 5;
    auto meter_width = gain_width / 10;
    auto label_height = 20;

    auto in_area = bounds.removeFromLeft(gain_width);
    auto in_meter_area = in_area.removeFromLeft(meter_width);

    auto out_area = bounds.removeFromRight(gain_width);
    auto out_meter_area = out_area.removeFromRight(meter_width);

    inputMeter.setBounds(in_meter_area);
    outputMeter.setBounds(out_meter_area);
    inputGainSlider.setBounds(in_area);
    outputGainSlider.setBounds(out_area);
}

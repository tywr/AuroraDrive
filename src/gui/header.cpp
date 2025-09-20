#include "header.h"

#include "looks/colors.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

Header::Header(
    juce::AudioProcessorValueTreeState& params, juce::Value& vin,
    juce::Value& vout
)
    : parameters(params), inputMeter(vin), outputMeter(vout)
{

    addAndMakeVisible(inputMeter);
    addAndMakeVisible(outputMeter);
    inputMeter.setSliderColour(headerColour);
    outputMeter.setSliderColour(headerColour);

    addAndMakeVisible(inputLabel);
    inputLabel.setText("IN", juce::dontSendNotification);
    inputLabel.setJustificationType(juce::Justification::left);

    addAndMakeVisible(outputLabel);
    outputLabel.setText("OUT", juce::dontSendNotification);
    outputLabel.setJustificationType(juce::Justification::right);

    addAndMakeVisible(inputGainSlider);
    inputGainSlider.setSkewFactor(3.0);
    inputGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    inputGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 25);
    inputGainSlider.setColour(
        juce::Slider::rotarySliderFillColourId, headerColour
    );
    inputGainAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "input_gain_db", inputGainSlider
        );

    addAndMakeVisible(outputGainSlider);
    outputGainSlider.setSkewFactor(3.0);
    outputGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    outputGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 25);
    outputGainSlider.setColour(
        juce::Slider::rotarySliderFillColourId, headerColour
    );
    outputGainAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "output_gain_db", outputGainSlider
        );
}

Header::~Header()
{
}

void Header::paint(juce::Graphics& g)
{
    g.fillAll(AuroraColors::bg);
}

void Header::resized()
{
    int const padding = 10;
    int const knob_padding = 3 * padding;
    int const knob_size = getHeight() - padding * 2;
    int const meter_width = 6;
    int const label_padding = 5;

    auto bounds = getLocalBounds().reduced(padding);

    inputMeter.setBounds(bounds.removeFromLeft(meter_width));
    outputMeter.setBounds(bounds.removeFromRight(meter_width));
    inputGainSlider.setBounds(bounds.removeFromLeft(knob_size + knob_padding));
    outputGainSlider.setBounds(
        bounds.removeFromRight(knob_size + knob_padding)
    );
    auto label_bounds =
        bounds.withTrimmedLeft(label_padding).withTrimmedRight(label_padding);
    inputLabel.setBounds(
        label_bounds.removeFromLeft(label_bounds.getWidth() / 2)
    );
    outputLabel.setBounds(label_bounds);
}

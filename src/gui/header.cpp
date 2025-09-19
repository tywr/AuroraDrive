#include "header.h"

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

    addAndMakeVisible(inputGainSlider);
    inputGainSlider.setSkewFactor(3.0);
    inputGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    inputGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 25);
    // inputGainSlider.setColour(
    //     juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack
    // );
    inputGainAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "input_gain_db", inputGainSlider
        );

    addAndMakeVisible(outputGainSlider);
    outputGainSlider.setSkewFactor(3.0);
    outputGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    outputGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 25);
    // outputGainSlider.setColour(
    //     juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack
    // );
    outputGainAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "output_gain_db", outputGainSlider
        );
}

Header::~Header()
{
}

void Header::resized()
{
    int const padding = 10;
    int const knob_padding = 3 * padding;
    int const knob_size = getHeight() - padding * 2;
    int const meter_width = 5;

    auto bounds = getLocalBounds().reduced(padding);

    inputMeter.setBounds(bounds.removeFromLeft(meter_width));
    outputMeter.setBounds(bounds.removeFromRight(meter_width));
    inputGainSlider.setBounds(bounds.removeFromLeft(knob_size + knob_padding));
    outputGainSlider.setBounds(
        bounds.removeFromRight(knob_size + knob_padding)
    );
}

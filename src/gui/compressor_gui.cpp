#include "compressor_gui.h"
#include "looks/colors.h"
#include "meter.h"

CompressorGui::CompressorGui(
    juce::AudioProcessorValueTreeState& params,
    juce::Value& compressorGainReductionDb
)
    : parameters(params), gainReductionValue(compressorGainReductionDb)
{
    setSize(600, 300);

    addAndMakeVisible(gainReductionMeterSlider);
    gainReductionMeterSlider.setRange(0.0, 24.0, 0.01);
    gainReductionMeterSlider.setSkewFactor(0.75);
    gainReductionMeterSlider.setSliderStyle(juce::Slider::LinearBar);
    gainReductionMeterSlider.setTextBoxStyle(
        juce::Slider::NoTextBox, false, 0, 0
    );
    gainReductionValue.addListener(this);
    gainReductionValue.setValue(0);

    addAndMakeVisible(bypassButton);
    bypassButton.setButtonText("BYPASS");
    bypassButtonAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, "compressor_bypass", bypassButton
        );

    addAndMakeVisible(gainSlider);
    addAndMakeVisible(gainLabel);
    gainLabel.setText("GAIN", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.attachToComponent(&gainSlider, false);
    gainSlider.setRange(0.0, 1.0, 0.01);
    gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    gainSlider.setColour(
        juce::Slider::rotarySliderFillColourId, compressorColour
    );
    gainSlider.setColour(
        juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack
    );
    gainSlider.setColour(
        juce::Slider::textBoxTextColourId, AuroraColors::grey3
    );
    gainSliderAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "compressor_gain_db", gainSlider
        );

    addAndMakeVisible(thresholdSlider);
    addAndMakeVisible(thresholdLabel);
    thresholdLabel.setText("THRESHOLD", juce::dontSendNotification);
    thresholdLabel.attachToComponent(&thresholdSlider, false);
    thresholdLabel.setJustificationType(juce::Justification::centred);
    thresholdSlider.setRange(0.0, 1.0, 0.01);
    thresholdSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    thresholdSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    thresholdSlider.setColour(
        juce::Slider::rotarySliderFillColourId, compressorColour
    );
    thresholdSlider.setColour(
        juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack
    );
    thresholdSlider.setColour(
        juce::Slider::textBoxTextColourId, AuroraColors::grey3
    );
    thresholdSliderAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "compressor_threshold", thresholdSlider
        );

    addAndMakeVisible(mixSlider);
    addAndMakeVisible(mixLabel);
    mixLabel.setText("MIX", juce::dontSendNotification);
    mixLabel.attachToComponent(&mixSlider, false);
    mixLabel.setJustificationType(juce::Justification::centred);
    mixSlider.setRange(0.0, 1.0, 0.01);
    mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    mixSlider.setColour(
        juce::Slider::rotarySliderFillColourId, compressorColour
    );
    mixSlider.setColour(
        juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack
    );
    mixSlider.setColour(juce::Slider::textBoxTextColourId, AuroraColors::grey3);
    mixSliderAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "compressor_mix", mixSlider
        );
}

CompressorGui::~CompressorGui()
{
}

void CompressorGui::valueChanged(juce::Value& newValue)
{
    double value = static_cast<float>(newValue.getValue());
    gainReductionMeterSlider.setValue(-1.0 * value);
}

void CompressorGui::resized()
{
    const int xpadding = 50;
    const int ypadding = 50;
    const int compressor_meter_height = 10;
    const int bypass_padding = 30;
    const int bypass_height = 30;
    const int bypass_size = 30;

    const int outer_knob_padding = 20;
    const int inner_knob_padding = 60;

    auto bounds = getLocalBounds().reduced(xpadding, ypadding);

    gainReductionMeterSlider.setBounds(
        bounds.removeFromTop(compressor_meter_height)
    );
    bypassButton.setBounds(
        bounds.removeFromBottom(bypass_height)
            .withSizeKeepingCentre(bypass_size, bypass_size)
    );

    auto knob_bounds = bounds.withTrimmedBottom(outer_knob_padding)
                           .withTrimmedTop(outer_knob_padding);

    const int knob_size = knob_bounds.getWidth() / 3;

    gainSlider.setBounds(
        knob_bounds.removeFromLeft(knob_size).reduced(inner_knob_padding)
    );
    thresholdSlider.setBounds(
        knob_bounds.removeFromLeft(knob_size).reduced(inner_knob_padding)
    );
    mixSlider.setBounds(knob_bounds.reduced(inner_knob_padding));
}

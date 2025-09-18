#include "compressor_gui.h"
#include "meter.h"

CompressorGui::CompressorGui(
    juce::AudioProcessorValueTreeState& params,
    juce::Value& compressorGainReductionDb
)
    : parameters(params), gainReductionValue(compressorGainReductionDb)
{
    setSize(600, 300);

    addAndMakeVisible(gainReductionMeterSlider);
    gainReductionMeterSlider.setRange(-30.0, 0.0, 0.01);
    gainReductionMeterSlider.setSliderStyle(juce::Slider::LinearBar);
    gainReductionValue.addListener(this);
    gainReductionValue.setValue(0);

    addAndMakeVisible(bypassButton);
    bypassButton.setButtonText("Bypass");
    bypassButtonAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, "compressor_bypass", bypassButton
        );
    bypassButton.setLookAndFeel(&bypassButtonLookAndFeel);

    addAndMakeVisible(gainSlider);
    addAndMakeVisible(gainLabel);
    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.attachToComponent(&gainSlider, false);
    gainSlider.setRange(0.0, 1.0, 0.01);
    gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    gainSliderAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "compressor_gain_db", gainSlider
        );

    addAndMakeVisible(peakSlider);
    addAndMakeVisible(peakLabel);
    peakLabel.setText("Peak", juce::dontSendNotification);
    peakLabel.attachToComponent(&peakSlider, false);
    peakLabel.setJustificationType(juce::Justification::centred);
    peakSlider.setRange(0.0, 1.0, 0.01);
    peakSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    peakSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    peakSliderAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "compressor_peak", peakSlider
        );

    addAndMakeVisible(mixSlider);
    addAndMakeVisible(mixLabel);
    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.attachToComponent(&mixSlider, false);
    mixLabel.setJustificationType(juce::Justification::centred);
    mixSlider.setRange(0.0, 1.0, 0.01);
    mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
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
    gainReductionMeterSlider.setValue(value);
}

void CompressorGui::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::red);
}

void CompressorGui::resized()
{
    const int gain_reduction_meter_height = 40;
    const int xpadding = 100;
    const int ypadding = 100;
    const int bypass_padding = 30;
    const int bypass_width = 50;

    auto bounds = getLocalBounds();
    auto gainReductionMeterBounds =
        bounds.removeFromTop(gain_reduction_meter_height);
    auto bypassButtonBounds =
        bounds.removeFromBottom(bypass_width + 2 * bypass_padding);
    bounds.reduce(xpadding, ypadding);

    auto gainSliderBounds = bounds.removeFromLeft(bounds.getWidth() / 3);
    auto peakSliderBounds = bounds.removeFromLeft(bounds.getWidth() / 2);
    auto mixSliderBounds = bounds;

    gainReductionMeterSlider.setBounds(gainReductionMeterBounds);
    gainSlider.setBounds(gainSliderBounds);
    peakSlider.setBounds(peakSliderBounds);
    mixSlider.setBounds(mixSliderBounds);

    bypassButton.setBounds(
        bypassButtonBounds.withSizeKeepingCentre(bypass_width, bypass_width)
    );
}

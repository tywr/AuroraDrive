#include "compressor_footer_component.h"
#include "../dimensions.h"
#include "../looks/compressor_footer_look_and_feel.h"

CompressorFooterComponent::CompressorFooterComponent(
    juce::AudioProcessorValueTreeState& params, juce::Value& v
)
    : parameters(params), gain_reduction_value(v)
{
    setLookAndFeel(new CompressorFooterLookAndFeel());

    addAndMakeVisible(gain_reduction_slider);

    gain_reduction_slider.setRange(0, 20.0f, 0.5f);
    gain_reduction_slider.setSkewFactor(1.0);
    gain_reduction_slider.setValue(0.0);
    gain_reduction_slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    gain_reduction_value.addListener(this);
    gain_reduction_value.setValue(0);
}

CompressorFooterComponent::~CompressorFooterComponent()
{
}

void CompressorFooterComponent::valueChanged(juce::Value& v)
{
    double value = -1 * static_cast<double>(v.getValue());
    gain_reduction_slider.setValue(value, juce::dontSendNotification);
}

void CompressorFooterComponent::paint(juce::Graphics& g)
{
}

void CompressorFooterComponent::resized()
{
    auto bounds = getLocalBounds();
    gain_reduction_slider.setBounds(bounds.withSizeKeepingCentre(
        GuiDimensions::COMPRESSOR_GAIN_REDUCTION_WIDTH,
        GuiDimensions::COMPRESSOR_GAIN_REDUCTION_HEIGHT
    ));
}

void CompressorFooterComponent::switchColour(
    juce::Colour colour1, juce::Colour colour2
)
{
    gain_reduction_slider.setColour(
        juce::Slider::rotarySliderFillColourId, colour1
    );
    repaint();
}

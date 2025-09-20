#include "meter.h"
#include <juce_audio_basics/juce_audio_basics.h>

Meter::Meter(juce::Value& v) : measuredValue(v)
{
    addAndMakeVisible(slider);
    slider.setRange(-48.0, 6.0, 0.1);
    slider.setSkewFactor(1.0);
    slider.setValue(0.0);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    slider.setSliderStyle(juce::Slider::LinearBarVertical);
    measuredValue.addListener(this);
    measuredValue.setValue(0);
    currentValue = static_cast<double>(measuredValue.getValue());
}

Meter::~Meter()
{
    measuredValue.removeListener(this);
}

void Meter::resized()
{
    auto bounds = getLocalBounds();
    slider.setBounds(bounds);
}

void Meter::valueChanged(juce::Value& newValue)
{

    double dbValue = juce::Decibels::gainToDecibels(
        static_cast<double>(newValue.getValue())
    );
    slider.setValue(dbValue);
}

void Meter::setSliderColour(juce::Colour c)
{
    slider.setColour(juce::Slider::trackColourId, c);
}

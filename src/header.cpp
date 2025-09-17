#include "header.h"
#include <juce_audio_basics/juce_audio_basics.h>

Header::Header(juce::Value& vin, juce::Value& vout)
    : inputMeter(vin), outputMeter(vout) {
    addAndMakeVisible(inputMeter);
    addAndMakeVisible(outputMeter);
}

Header::~Header() {}

void Header::paint(juce::Graphics& g) {}

void Header::resized() {
    int padding = 10;
    auto bounds = getLocalBounds().reduced(padding);
    inputMeter.setBounds(bounds.removeFromLeft(10));
    outputMeter.setBounds(bounds.removeFromRight(10));
}

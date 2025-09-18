#include "compressor_gui.h"

CompressorGui::CompressorGui(juce::AudioProcessorValueTreeState& params)
    : parameters(params) {
    setSize(600, 300);
    addAndMakeVisible(gainSlider);
    gainSlider.setRange(0.0, 1.0, 0.01);
    gainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
}

CompressorGui::~CompressorGui() {}

void CompressorGui::paint(juce::Graphics& g) { g.fillAll(juce::Colours::red); }

void CompressorGui::resized() {
    const int padding = 10;
    auto bounds = getLocalBounds().reduced(padding);
    gainSlider.setBounds(bounds);
}

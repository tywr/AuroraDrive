#include "header.h"

#include "colours.h"
#include "dimensions.h"
#include "looks/tuner_look_and_feel.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

Header::Header(
    juce::AudioProcessorValueTreeState& params, juce::Value& vin,
    juce::Value& vout, SessionManager& sm
)
    : parameters(params), inputMeter(vin), outputMeter(vout),
      sessionNameDisplay(sm), presetBar(sm)
{
    addAndMakeVisible(inputMeter);
    addAndMakeVisible(outputMeter);
    inputMeter.setSliderColour(headerColour);
    outputMeter.setSliderColour(headerColour);

    addAndMakeVisible(inputGainSlider);
    inputGainSlider.setSkewFactor(3.0);
    inputGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    inputGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 25);
    inputGainSlider.setColour(
        juce::Slider::rotarySliderFillColourId, ColourCodes::bg0
    );
    inputGainSlider.setColour(
        juce::Slider::rotarySliderOutlineColourId, ColourCodes::white0
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
        juce::Slider::rotarySliderFillColourId, ColourCodes::bg0
    );
    outputGainSlider.setColour(
        juce::Slider::rotarySliderOutlineColourId, ColourCodes::white0
    );
    outputGainAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "output_gain_db", outputGainSlider
        );

    addAndMakeVisible(tunerButton);
    tunerButton.setLookAndFeel(new TunerLookAndFeel());
    tunerButton.onClick = [this]()
    {
        if (onTunerClicked)
            onTunerClicked();
    };

    addAndMakeVisible(presetIconButtons);
    addAndMakeVisible(sessionNameDisplay);
    addAndMakeVisible(presetBar);
}

Header::~Header()
{
}

void Header::paint(juce::Graphics& g)
{
    g.setColour(GuiColours::PANEL_BACKGROUND);
    g.fillRect(getLocalBounds());
}

void Header::resized()
{
    int const padding = 10;
    int const knob_padding = 3 * padding;
    int const knob_size = getHeight() - padding * 2;
    int const meter_width = 6;

    auto bounds = getLocalBounds().reduced(padding);

    // Left side: input meter and gain
    inputMeter.setBounds(bounds.removeFromLeft(meter_width));
    bounds.removeFromLeft(padding);
    inputGainSlider.setBounds(
        bounds.removeFromLeft(knob_size + knob_padding)
            .reduced(GuiDimensions::HEADER_GAIN_SLIDER_PADDING)
    );
    bounds.removeFromLeft(padding);

    // Right side: output gain and meter
    outputMeter.setBounds(bounds.removeFromRight(meter_width));
    bounds.removeFromRight(padding);
    outputGainSlider.setBounds(
        bounds.removeFromRight(knob_size + knob_padding)
            .reduced(GuiDimensions::HEADER_GAIN_SLIDER_PADDING)
    );
    bounds.removeFromRight(padding);

    // Center area: tuner button and preset controls
    int const iconButtonSize = 42;
    int const iconButtonsWidth = iconButtonSize * 4;
    int const sessionNameWidth = 140;
    int const innerPadding = 5;

    int const controlsWidth = 5 * iconButtonSize + sessionNameWidth;

    // Preset bar takes remaining space on the right
    int const presetBarWidth = bounds.getWidth() - controlsWidth;
    presetBar.setBounds(
        bounds.removeFromRight(presetBarWidth).reduced(innerPadding, 0)
    );

    // Center tuner + icon buttons + selector in remaining area
    int const horizontalOffset = (bounds.getWidth() - controlsWidth) / 2;
    bounds.removeFromLeft(horizontalOffset);

    tunerButton.setBounds(bounds.removeFromLeft(iconButtonSize));
    presetIconButtons.setBounds(bounds.removeFromLeft(iconButtonsWidth));
    sessionNameDisplay.setBounds(
        bounds.removeFromLeft(sessionNameWidth).reduced(innerPadding, 0)
    );
}

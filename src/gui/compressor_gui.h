#pragma once

#include "compressor_gui.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class BypassButtonLookAndFeel : public juce::LookAndFeel_V4
{
  public:
    void drawToggleButton(
        juce::Graphics& g, juce::ToggleButton& button, bool isMouseOverButton,
        bool isButtonDown
    ) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        juce::Colour colour = juce::Colours::red; // Default color

        if (button.getToggleState())
            colour = juce::Colours::green; // Color for the "on" state

        if (isButtonDown)
            colour = colour.darker(0.2f);
        else if (isMouseOverButton)
            colour = colour.brighter(0.2f);

        g.setColour(colour);
        g.fillEllipse(bounds);

        g.setColour(juce::Colours::black);
        g.drawEllipse(bounds, 1.0f);
    }
};

class CompressorGui : public juce::Component
{
  public:
    CompressorGui(juce::AudioProcessorValueTreeState&);
    ~CompressorGui() override;

    void paint(juce::Graphics&) override;
    void resized() override;

  private:
    juce::AudioProcessorValueTreeState& parameters;

    juce::ToggleButton bypassButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        bypassButtonAttachment;
    BypassButtonLookAndFeel bypassButtonLookAndFeel;

    juce::Slider gainSlider;
    juce::Label gainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        gainSliderAttachment;

    juce::Slider peakSlider;
    juce::Label peakLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        peakSliderAttachment;

    juce::Slider mixSlider;
    juce::Label mixLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        mixSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorGui)
};

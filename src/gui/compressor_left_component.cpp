#include "compressor_left_component.h"
#include "dimensions.h"
#include "looks/colors.h"
#include "looks/compressor_look_and_feel.h"
#include "meter.h"

CompressorLeftComponent::CompressorLeftComponent(
    juce::AudioProcessorValueTreeState& params
)
    : parameters(params)
{
    setLookAndFeel(new CompressorLookAndFeel());
    addAndMakeVisible(level_slider);
    addAndMakeVisible(level_label);
    level_label.setText("LVL", juce::dontSendNotification);
    level_label.setJustificationType(juce::Justification::centred);
    level_label.attachToComponent(&level_slider, false);
    level_slider.setRange(0.0, 1.0, 0.01);
    level_slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    level_slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    level_slider.setColour(
        juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack
    );
    level_slider.setColour(
        juce::Slider::textBoxTextColourId, AuroraColors::grey3
    );
    level_slider_attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "compressor_level_db", level_slider
        );

    addAndMakeVisible(threshold_slider);
    addAndMakeVisible(threshold_label);
    threshold_label.setText("THR", juce::dontSendNotification);
    threshold_label.attachToComponent(&threshold_slider, false);
    threshold_label.setJustificationType(juce::Justification::centred);
    threshold_slider.setRange(0.0, 1.0, 0.01);
    threshold_slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    threshold_slider.setTextBoxStyle(
        juce::Slider::TextBoxBelow, false, 100, 20
    );
    threshold_slider.setColour(
        juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack
    );
    threshold_slider.setColour(
        juce::Slider::textBoxTextColourId, AuroraColors::grey3
    );
    threshold_slider_attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "compressor_threshold", threshold_slider
        );
}

CompressorLeftComponent::~CompressorLeftComponent()
{
}

void CompressorLeftComponent::paint(juce::Graphics& g)
{
    // g.fillAll(AuroraColors::grey0);
}

void CompressorLeftComponent::resized()
{
    auto bounds = getLocalBounds();
    const int knob_box_size = bounds.getHeight() / 2;

    threshold_slider.setBounds(bounds.removeFromTop(knob_box_size)
                                   .withSizeKeepingCentre(
                                       GuiDimensions::PREAMP_SIDE_KNOB_WIDTH,
                                       GuiDimensions::PREAMP_SIDE_KNOB_HEIGHT
                                   ));
    level_slider.setBounds(bounds.withSizeKeepingCentre(
        GuiDimensions::PREAMP_SIDE_KNOB_WIDTH,
        GuiDimensions::PREAMP_SIDE_KNOB_HEIGHT
    ));
}

#include "compressor_right_component.h"
#include "dimensions.h"
#include "looks/colors.h"
#include "looks/compressor_look_and_feel.h"
#include "meter.h"

CompressorRightComponent::CompressorRightComponent(
    juce::AudioProcessorValueTreeState& params
)
    : parameters(params)
{
    setLookAndFeel(new CompressorLookAndFeel());

    addAndMakeVisible(ratio_slider);
    addAndMakeVisible(ratio_label);
    ratio_label.setText("RATIO", juce::dontSendNotification);
    ratio_label.attachToComponent(&ratio_slider, false);
    ratio_label.setJustificationType(juce::Justification::centred);
    ratio_slider.setRange(0.0, 1.0, 0.01);
    ratio_slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    ratio_slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    ratio_slider.setColour(
        juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack
    );
    ratio_slider.setColour(
        juce::Slider::textBoxTextColourId, AuroraColors::grey3
    );
    ratio_slider_attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "compressor_ratio", ratio_slider
        );

    addAndMakeVisible(selection_slider);
    addAndMakeVisible(selection_label);
    selection_label.setText("TYPE", juce::dontSendNotification);
    selection_label.attachToComponent(&selection_slider, false);
    selection_label.setJustificationType(juce::Justification::centred);
    selection_slider.setRange(0.0, 1.0, 0.01);
    selection_slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    selection_slider.setTextBoxStyle(
        juce::Slider::TextBoxBelow, false, 100, 20
    );
    selection_slider.setColour(
        juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack
    );
    selection_slider.setColour(
        juce::Slider::textBoxTextColourId, AuroraColors::grey3
    );
    selection_slider_attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "compressor_type", selection_slider
        );
}

CompressorRightComponent::~CompressorRightComponent()
{
}

void CompressorRightComponent::paint(juce::Graphics& g)
{
    // g.fillAll(AuroraColors::grey0);
}

void CompressorRightComponent::resized()
{
    auto bounds = getLocalBounds();
    const int knob_box_size = bounds.getHeight() / 2;

    ratio_slider.setBounds(bounds.removeFromTop(knob_box_size)
                               .withSizeKeepingCentre(
                                   GuiDimensions::PREAMP_SIDE_KNOB_WIDTH,
                                   GuiDimensions::PREAMP_SIDE_KNOB_HEIGHT
                               ));
    selection_slider.setBounds(bounds.withSizeKeepingCentre(
        GuiDimensions::PREAMP_SIDE_KNOB_WIDTH,
        GuiDimensions::PREAMP_SIDE_KNOB_HEIGHT
    ));
}

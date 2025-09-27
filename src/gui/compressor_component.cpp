#include "compressor_component.h"
#include "looks/colors.h"
#include "meter.h"

CompressorComponent::CompressorComponent(
    juce::AudioProcessorValueTreeState& params
)
    : parameters(params)
{
    // setSize(600, 300);

    addAndMakeVisible(compressorSwitcherButton);
    compressorSwitcherButton.onClick = [this]()
    {
        static int index = 0;
        index = (index + 1) % compressorChoices.size();
        parameters.getParameter("compressor_type")
            ->setValueNotifyingHost(
                index / (float)(compressorChoices.size() - 1)
            );
    };

    addAndMakeVisible(compressorTypeLabel);
    compressorTypeLabel.setJustificationType(juce::Justification::left);
    compressorTypeLabel.setText(
        parameters.getParameter("compressor_type")->getCurrentValueAsText(),
        juce::dontSendNotification
    );
    compressorTypeLabelAttachment = std::make_unique<juce::ParameterAttachment>(
        *parameters.getParameter("compressor_type"),
        [this](float newValue)
        {
            // Convert parameter value to display text
            int index = static_cast<int>(newValue);
            if (index >= 0 && index < compressorChoices.size())
            {
                compressorTypeLabel.setText(
                    compressorChoices[index], juce::dontSendNotification
                );
            }
            switchCompressorColour();
        }
    );

    addAndMakeVisible(bypassLabel);
    bypassLabel.setText("BYPASS", juce::dontSendNotification);
    bypassLabel.setJustificationType(juce::Justification::right);

    addAndMakeVisible(bypassButton);
    bypassButton.setButtonText("BYPASS");
    bypassButtonAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, "compressor_bypass", bypassButton
        );
    bypassButton.onClick = [this]() { switchCompressorColour(); };

    addAndMakeVisible(gainSlider);
    addAndMakeVisible(gainLabel);
    gainLabel.setText("GAIN", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.attachToComponent(&gainSlider, false);
    gainSlider.setRange(0.0, 1.0, 0.01);
    gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
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
    thresholdLabel.setText("THR", juce::dontSendNotification);
    thresholdLabel.attachToComponent(&thresholdSlider, false);
    thresholdLabel.setJustificationType(juce::Justification::centred);
    thresholdSlider.setRange(0.0, 1.0, 0.01);
    thresholdSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    thresholdSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
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
        juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack
    );
    mixSlider.setColour(juce::Slider::textBoxTextColourId, AuroraColors::grey3);
    mixSliderAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "compressor_mix", mixSlider
        );
    switchCompressorColour();
}

CompressorComponent::~CompressorComponent()
{
}

void CompressorComponent::resized()
{
    const int xpadding = 50;
    const int ypadding = 50;
    const int compressor_meter_height = 6;
    const int button_padding = 30;
    const int button_height = 50;
    const int button_size = 50;
    const int label_padding = 20;

    const int outer_knob_padding = 20;
    const int inner_knob_padding = 60;

    auto bounds = getLocalBounds().reduced(xpadding, ypadding);

    auto bottom_bounds = bounds.removeFromBottom(button_height);
    bypassButton.setBounds(bottom_bounds.removeFromRight(button_size));
    compressorSwitcherButton.setBounds(
        bottom_bounds.removeFromLeft(button_size)
    );
    compressorTypeLabel.setBounds(
        bottom_bounds.removeFromLeft(bottom_bounds.getWidth() / 2)
            .withTrimmedLeft(label_padding)
    );
    bypassLabel.setBounds(bottom_bounds.withTrimmedRight(label_padding));

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

    // Don't forget to set compressor colour based on updated label value
    switchCompressorColour();
}

void CompressorComponent::switchCompressorColour()
{
    juce::Colour compressorColour = defaultCompressorColour;
    auto currentType = compressorTypeLabel.getText();
    if (bypassButton.getToggleState())
    {
        if (compressorColourMapping.find(currentType.toStdString()) !=
            compressorColourMapping.end())
        {
            compressorColour =
                compressorColourMapping.at(currentType.toStdString());
        }
        bypassButton.setColour(
            juce::ToggleButton::tickColourId, compressorColour
        );
        compressorSwitcherButton.setColour(
            juce::TextButton::buttonColourId, compressorColour
        );
    }
    else
    {
        compressorSwitcherButton.setColour(
            juce::TextButton::buttonColourId, AuroraColors::grey0
        );
    }
    gainSlider.setColour(
        juce::Slider::rotarySliderFillColourId, compressorColour
    );
    thresholdSlider.setColour(
        juce::Slider::rotarySliderFillColourId, compressorColour
    );
    mixSlider.setColour(
        juce::Slider::rotarySliderFillColourId, compressorColour
    );

    repaint();
}

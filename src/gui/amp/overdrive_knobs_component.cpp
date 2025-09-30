#include "overdrive_knobs_component.h"
#include "../colours.h"
#include "../dimensions.h"

OverdriveKnobsComponent::OverdriveKnobsComponent(
    juce::AudioProcessorValueTreeState& params
)
    : parameters(params)
{
    for (auto knob : knobs)
    {
        addAndMakeVisible(knob.slider);
        addAndMakeVisible(knob.label);
        knob.label->setText(knob.label_text, juce::dontSendNotification);
        knob.label->setJustificationType(juce::Justification::centred);
        // knob.label->attachToComponent(knob.slider, false);
        knob.slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        knob.slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
        knob.label->setColour(
            juce::Slider::textBoxOutlineColourId,
            juce::Colours::transparentBlack
        );
        knob.label->setColour(
            juce::Slider::textBoxTextColourId, ColourCodes::grey3
        );
        slider_attachments.push_back(
            std::make_unique<
                juce::AudioProcessorValueTreeState::SliderAttachment>(
                parameters, knob.parameter_id, *knob.slider
            )
        );
    }
}

OverdriveKnobsComponent::~OverdriveKnobsComponent()
{
}

void OverdriveKnobsComponent::paint(juce::Graphics& g)
{
    // g.fillAll(AuroraColors::bg);
}

void OverdriveKnobsComponent::resized()
{

    auto bounds = getLocalBounds();
    auto label_bounds =
        bounds.removeFromTop(GuiDimensions::OVERDRIVE_LABEL_HEIGHT);

    const int knob_box_size = bounds.getWidth() / knobs.size();

    for (auto knob : knobs)
    {
        knob.label->setBounds(label_bounds.removeFromLeft(knob_box_size)
                                  .withSizeKeepingCentre(
                                      GuiDimensions::OVERDRIVE_KNOB_WIDTH,
                                      GuiDimensions::OVERDRIVE_LABEL_HEIGHT
                                  ));
        knob.slider->setBounds(bounds.removeFromLeft(knob_box_size)
                                   .withSizeKeepingCentre(
                                       GuiDimensions::OVERDRIVE_KNOB_WIDTH,
                                       GuiDimensions::OVERDRIVE_KNOB_HEIGHT
                                   ));
    }
    // Don't forget to set compressor colour based on updated label value
    switchColour();
}

void OverdriveKnobsComponent::switchColour()
{
    repaint();
}

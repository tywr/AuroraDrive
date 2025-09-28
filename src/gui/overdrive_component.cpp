#include "overdrive_component.h"
#include "dimensions.h"
#include "looks/colors.h"
#include "looks/overdrive_look_and_feel.h"

OverdriveComponent::OverdriveComponent(
    juce::AudioProcessorValueTreeState& params
)
    : parameters(params)
{
    setLookAndFeel(new OverdriveLookAndFeel());

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
            juce::Slider::textBoxTextColourId, AuroraColors::grey3
        );
        slider_attachments.push_back(
            std::make_unique<
                juce::AudioProcessorValueTreeState::SliderAttachment>(
                parameters, knob.parameter_id, *knob.slider
            )
        );
    }
}

OverdriveComponent::~OverdriveComponent()
{
}

void OverdriveComponent::paint(juce::Graphics& g)
{
    // g.fillAll(AuroraColors::bg);
}

void OverdriveComponent::resized()
{

    auto bounds = getLocalBounds();
    auto label_bounds =
        bounds.removeFromTop(GuiDimensions::OVERDRIVE_LABEL_HEIGHT);

    const int knob_box_size = bounds.getWidth() / knobs.size();

    for (Knob knob : knobs)
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

void OverdriveComponent::switchColour()
{
    juce::Colour overdrive_colour = default_type_colour;
    auto current_type =
        parameters.getParameter("overdrive_type")->getCurrentValueAsText();
    bool bypass =
        parameters.getRawParameterValue("overdrive_bypass")->load() < 0.5f;
    {
        if (overdrive_colour_mapping.find(current_type.toStdString()) !=
            overdrive_colour_mapping.end())
        {
            overdrive_colour =
                overdrive_colour_mapping.at(current_type.toStdString());
        }
    }
    for (Knob knob : knobs)
    {
        knob.slider->setColour(
            juce::Slider::rotarySliderFillColourId, overdrive_colour
        );
    }
    repaint();
}

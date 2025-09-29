#include "compressor_knobs_component.h"
#include "../dimensions.h"
#include "../looks/colors.h"
#include "../looks/compressor_look_and_feel.h"

CompressorKnobsComponent::CompressorKnobsComponent(
    juce::AudioProcessorValueTreeState& params
)
    : parameters(params)
{
    setLookAndFeel(new CompressorLookAndFeel());

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

CompressorKnobsComponent::~CompressorKnobsComponent()
{
}

void CompressorKnobsComponent::paint(juce::Graphics& g)
{
    // g.fillAll(AuroraColors::bg);
}

void CompressorKnobsComponent::resized()
{

    auto bounds = getLocalBounds();
    auto label_bounds =
        bounds.removeFromTop(GuiDimensions::OVERDRIVE_LABEL_HEIGHT);

    const int knob_box_size = bounds.getWidth() / knobs.size();

    for (CompressorKnob knob : knobs)
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

void CompressorKnobsComponent::switchColour()
{
    juce::Colour compressor_colour = default_type_colour;
    auto current_type =
        parameters.getParameter("compressor_type")->getCurrentValueAsText();
    bool bypass =
        parameters.getRawParameterValue("compressor_bypass")->load() < 0.5f;
    {
        if (compressor_colour_mapping.find(current_type.toStdString()) !=
            compressor_colour_mapping.end())
        {
            compressor_colour =
                compressor_colour_mapping.at(current_type.toStdString());
        }
    }
    for (CompressorKnob knob : knobs)
    {
        knob.slider->setColour(
            juce::Slider::rotarySliderFillColourId, compressor_colour
        );
    }
    repaint();
}

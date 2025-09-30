#include "compressor_knobs_component.h"
#include "../dimensions.h"
#include "../colours.h"
#include "../looks/compressor_look_and_feel.h"
#include "../looks/compressor_selector_look_and_feel.h"

CompressorKnobsComponent::CompressorKnobsComponent(
    juce::AudioProcessorValueTreeState& params
)
    : parameters(params)
{
    // setLookAndFeel(new CompressorLookAndFeel());
    type_slider.setLookAndFeel(&selector_look_and_feel);
    ratio_slider.setLookAndFeel(&selector_look_and_feel);

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

CompressorKnobsComponent::~CompressorKnobsComponent()
{
}

void CompressorKnobsComponent::paint(juce::Graphics& g)
{
}

void CompressorKnobsComponent::resized()
{

    int const limit = 3;
    auto bounds = getLocalBounds();
    auto top_bounds =
        bounds.removeFromTop(GuiDimensions::COMPRESSOR_KNOBS_TOP_BOX_HEIGHT);
    auto label_bounds =
        top_bounds.removeFromTop(GuiDimensions::DEFAULT_LABEL_HEIGHT);
    const int knob_box_size = bounds.getWidth() / limit;

    for (size_t i = 0; i < limit; ++i)
    {
        CompressorKnob knob = knobs[i];
        knob.label->setBounds(label_bounds.removeFromLeft(knob_box_size)
                                  .withSizeKeepingCentre(
                                      GuiDimensions::DEFAULT_KNOB_WIDTH,
                                      GuiDimensions::DEFAULT_LABEL_HEIGHT
                                  ));
        knob.slider->setBounds(top_bounds.removeFromLeft(knob_box_size)
                                   .withSizeKeepingCentre(
                                       GuiDimensions::DEFAULT_KNOB_WIDTH,
                                       GuiDimensions::DEFAULT_KNOB_HEIGHT
                                   ));
    }

    const int bottom_knob_box_size = bounds.getWidth() / (knobs.size() - limit);
    auto bottom_bounds = bounds.removeFromBottom(
        GuiDimensions::COMPRESSOR_KNOBS_BOTTOM_BOX_HEIGHT
    );
    auto label_bottom_bounds =
        bottom_bounds.removeFromTop(GuiDimensions::DEFAULT_SMALL_LABEL_HEIGHT);

    for (size_t i = limit; i < knobs.size(); ++i)
    {
        CompressorKnob knob = knobs[i];
        knob.label->setBounds(label_bottom_bounds
                                  .removeFromLeft(bottom_knob_box_size)
                                  .withSizeKeepingCentre(
                                      GuiDimensions::DEFAULT_KNOB_WIDTH,
                                      GuiDimensions::DEFAULT_LABEL_HEIGHT
                                  ));
        knob.slider->setBounds(bottom_bounds
                                   .removeFromLeft(bottom_knob_box_size)
                                   .withSizeKeepingCentre(
                                       GuiDimensions::DEFAULT_SMALL_KNOB_WIDTH,
                                       GuiDimensions::DEFAULT_SMALL_KNOB_HEIGHT
                                   ));
    }
}

void CompressorKnobsComponent::switchColour(
    juce::Colour colour1, juce::Colour colour2
)
{
    for (CompressorKnob knob : knobs)
    {
        knob.slider->setColour(juce::Slider::rotarySliderFillColourId, colour1);
    }
    repaint();
}

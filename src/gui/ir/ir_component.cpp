#include "ir_component.h"
#include "../colours.h"
#include "../dimensions.h"
#include "../fonts.h"
#include "ir_dimensions.h"

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

IRComponent::IRComponent(juce::AudioProcessorValueTreeState& params)
    : EffectComponent(params, "IMPULSE", "ir_bypass"), parameters(params)
{
    addAndMakeVisible(drag_tooltip);
    drag_tooltip.setJustificationType(juce::Justification::centred);
    drag_tooltip.setAlwaysOnTop(true);
    drag_tooltip.setColour(juce::Label::backgroundColourId, ColourCodes::bg0);

    addAndMakeVisible(type_display);
    type_display.setFont(Fonts::getFont(24.0f), true);
    type_display.setJustification(juce::Justification::centred);
    type_display.setColour(ColourCodes::grey3);
    auto* parameter = parameters.getParameter("ir_type");
    type_display_attachment = std::make_unique<juce::ParameterAttachment>(
        *parameter,
        [this, parameter](float new_value)
        {
            juce::StringArray values = parameter->getAllValueStrings();
            juce::String text = values[(int)new_value];
            type_display.setText(text);
            repaint();
        }
    );
    type_display_attachment->sendInitialUpdate();

    for (auto knob : knobs)
    {
        addAndMakeVisible(knob.knob);
        knob.knob->setLabelText(knob.label_text);
        knob.knob->setKnobSize(
            IRDimensions::KNOB_SIZE, IRDimensions::KNOB_SIZE
        );
        knob.knob->setLabelHeight(IRDimensions::LABEL_HEIGHT);
        knob.knob->getSlider().setColour(
            juce::Slider::rotarySliderOutlineColourId, ColourCodes::grey3
        );
        knob.knob->getSlider().setColour(
            juce::Slider::rotarySliderFillColourId, juce::Colours::transparentBlack
        );
        slider_attachments.push_back(
            std::make_unique<
                juce::AudioProcessorValueTreeState::SliderAttachment>(
                parameters, knob.parameter_id, knob.knob->getSlider()
            )
        );
        setupSliderTooltipHandling(knob.knob);
    }
    switchColour();
}

void IRComponent::bypassStateChanged()
{
    switchColour();
}

void IRComponent::paintContent(juce::Graphics& g)
{
    // Calculate display bounds to match resized() layout
    auto bounds_for_display = getLocalBounds();
    bounds_for_display.removeFromTop(GuiDimensions::PANEL_TITLE_BAR_HEIGHT);
    auto display_section =
        bounds_for_display.removeFromTop(bounds_for_display.getHeight() / 2);

    auto display_bounds =
        display_section
            .withSizeKeepingCentre(
                IRDimensions::IR_LABEL_WIDTH, IRDimensions::IR_LABEL_HEIGHT
            )
            .toFloat();

    type_display.setBoundingBox(display_bounds);
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(display_bounds, 5.0f);
    type_display.draw(g, 1.0f);
}

void IRComponent::resizedContent(juce::Rectangle<int> bounds)
{
    // Split remaining bounds into top row (display) and bottom row (knobs)
    auto display_section =
        bounds.removeFromTop(bounds.getHeight() / 2);
    auto knobs_section = bounds;

    // Position display centered in top section
    auto display_bounds = display_section.withSizeKeepingCentre(
        IRDimensions::IR_LABEL_WIDTH, IRDimensions::IR_LABEL_HEIGHT
    );
    type_display.setBounds(display_bounds);

    // Position 3 knobs horizontally in bottom section
    auto knob_area = knobs_section.withSizeKeepingCentre(
        knobs_section.getWidth() * 0.8f, IRDimensions::BOX_HEIGHT
    );

    const int knob_box_size = knob_area.getWidth() / (int)knobs.size();
    for (size_t i = 0; i < 3; ++i)
    {
        IRKnob knob = knobs[i];
        knob.knob->setBounds(knob_area.removeFromLeft(knob_box_size));
    }
}

void IRComponent::switchColour()
{
    if (isBypassed())
    {
        current_colour = GuiColours::DEFAULT_INACTIVE_COLOUR;
    }
    else
    {
        current_colour = ColourCodes::orange;
    }
    for (auto knob : knobs)
    {
        knob.knob->getSlider().setColour(
            juce::Slider::rotarySliderOutlineColourId, current_colour
        );
    }
    type_display.setColour(current_colour);
    repaint();
}

void IRComponent::setupSliderTooltipHandling(LabeledKnob* knob)
{
    auto& slider = knob->getSlider();
    auto& label = knob->getLabel();

    slider.onDragStart = [this, &slider, &label, knob]()
    {
        slider_being_dragged = true;
        drag_tooltip.setVisible(false);
        // delay using a Timer
        juce::Timer::callAfterDelay(
            300,
            [this, &slider, &label, knob]()
            {
                if (slider.isMouseButtonDown())
                {
                    drag_tooltip.setText(
                        juce::String(slider.getValue(), 2),
                        juce::dontSendNotification
                    );
                    auto labelBounds = getLocalArea(knob, label.getBounds());
                    drag_tooltip.setBounds(labelBounds);
                    drag_tooltip.toFront(true);
                    drag_tooltip.setVisible(true);
                    drag_tooltip.repaint();
                }
            }
        );
    };
    slider.onDragEnd = [this]()
    {
        slider_being_dragged = false;
        drag_tooltip.setVisible(false);
    };
    slider.onValueChange = [this, &slider]()
    {
        if (slider_being_dragged && drag_tooltip.isVisible())
            drag_tooltip.setText(
                juce::String(slider.getValue(), 2), juce::dontSendNotification
            );
    };
}

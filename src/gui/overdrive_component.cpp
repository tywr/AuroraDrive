#include "overdrive_component.h"
#include "dimensions.h"
#include "looks/colors.h"

OverdriveComponent::OverdriveComponent(
    juce::AudioProcessorValueTreeState& params
)
    : parameters(params)
{
    // Switcher button
    addAndMakeVisible(switcher_button);
    switcher_button.onClick = [this]()
    {
        auto parameter = parameters.getParameter("overdrive_type");
        juce::String current_type = parameter->getCurrentValueAsText();
        juce::StringArray choices = parameter->getAllValueStrings();
        int index = choices.indexOf(current_type);
        parameter->setValueNotifyingHost(
            (index + 1) % choices.size() / (float)(choices.size() - 1)
        );
    };

    // Sliders and labels
    addAndMakeVisible(type_label);
    type_label.setJustificationType(juce::Justification::left);
    type_label.setText(
        parameters.getParameter("overdrive_type")->getCurrentValueAsText(),
        juce::dontSendNotification
    );
    type_label_attachment = std::make_unique<juce::ParameterAttachment>(
        *parameters.getParameter("overdrive_type"),
        [this](float newValue)
        {
            auto choices =
                parameters.getParameter("overdrive_type")->getAllValueStrings();
            int index = static_cast<int>(newValue);
            if (index >= 0 && index < choices.size())
            {
                type_label.setText(
                    choices[index], juce::dontSendNotification
                );
            }
            switchColour();
        }
    );

    addAndMakeVisible(bypass_label);
    bypass_label.setText("BYPASS", juce::dontSendNotification);
    bypass_label.setJustificationType(juce::Justification::right);

    addAndMakeVisible(bypass_button);
    bypass_button.setButtonText("BYPASS");
    bypass_button_attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, "overdrive_bypass", bypass_button
        );
    bypass_button.onClick = [this]() { switchColour(); };

    for (auto knob : knobs)
    {
        addAndMakeVisible(knob.slider);
        addAndMakeVisible(knob.label);
        knob.label->setText(knob.label_text, juce::dontSendNotification);
        knob.label->setJustificationType(juce::Justification::centred);
        knob.label->attachToComponent(knob.slider, false);
        knob.slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        knob.slider->setTextBoxStyle(
            juce::Slider::TextBoxBelow, false, 100, 20
        );
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

void OverdriveComponent::resized()
{

    auto bounds = getLocalBounds().reduced(GuiDimensions::TAB_INNER_PADDING);

    // header layout (here we paint nothing inside)
    bounds.withTrimmedTop(GuiDimensions::TAB_HEADER_HEIGHT);

    // footer layout
    const int button_size = 50;

    auto footer_bounds =
        bounds.removeFromBottom(GuiDimensions::TAB_FOOTER_HEIGHT);
    bypass_button.setBounds(
        footer_bounds.removeFromRight(GuiDimensions::TAB_BUTTON_SIZE)
    );
    switcher_button.setBounds(
        footer_bounds.removeFromLeft(GuiDimensions::TAB_BUTTON_SIZE)
    );
    type_label.setBounds(
        footer_bounds.removeFromLeft(footer_bounds.getWidth() / 2)
            .withTrimmedLeft(GuiDimensions::TAB_BUTTON_LABEL_PADDING)
    );
    bypass_label.setBounds(
        footer_bounds.withTrimmedRight(GuiDimensions::TAB_BUTTON_LABEL_PADDING)
    );

    // middle layout
    auto middle_bounds =
        bounds.withTrimmedBottom(GuiDimensions::TAB_MIDDLE_VERTICAL_PADDING)
            .withTrimmedTop(GuiDimensions::TAB_MIDDLE_VERTICAL_PADDING);

    const int knob_box_size = middle_bounds.getWidth() / knobs.size();
    const int inner_knob_padding = 30;

    for (Knob knob : knobs)
    {
        knob.slider->setBounds(middle_bounds.removeFromLeft(knob_box_size)
                                   .withSizeKeepingCentre(
                                       GuiDimensions::TAB_KNOB_BOX_WIDTH,
                                       GuiDimensions::TAB_KNOB_BOX_HEIGHT
                                   ));
    }

    // Don't forget to set compressor colour based on updated label value
    switchColour();
}

void OverdriveComponent::switchColour()
{
    juce::Colour overdrive_colour = default_type_colour;
    auto current_type = type_label.getText();
    if (bypass_button.getToggleState())
    {
        if (overdrive_colour_mapping.find(current_type.toStdString()) !=
            overdrive_colour_mapping.end())
        {
            overdrive_colour =
                overdrive_colour_mapping.at(current_type.toStdString());
        }
        bypass_button.setColour(
            juce::ToggleButton::tickColourId, overdrive_colour
        );
        switcher_button.setColour(
            juce::TextButton::buttonColourId, overdrive_colour
        );
    }
    else
    {
        switcher_button.setColour(
            juce::TextButton::buttonColourId, AuroraColors::grey0
        );
    }
    for (Knob knob : knobs)
    {
        knob.slider->setColour(
            juce::Slider::rotarySliderFillColourId, overdrive_colour
        );
    }
    repaint();
}

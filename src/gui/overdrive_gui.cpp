#include "overdrive_gui.h"
#include "looks/colors.h"

OverdriveGui::OverdriveGui(juce::AudioProcessorValueTreeState& params)
    : parameters(params)
{
    // Switcher button
    addAndMakeVisible(switcher_button);
    switcher_button.onClick = [this]()
    {
        static int index = 0;
        index = (index + 1) % overdrive_choices.size();
        parameters.getParameter("overdrive_type")
            ->setValueNotifyingHost(
                index / (float)(overdrive_choices.size() - 1)
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
            int index = static_cast<int>(newValue);
            if (index >= 0 && index < overdrive_choices.size())
            {
                type_label.setText(
                    overdrive_choices[index], juce::dontSendNotification
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

    for (size_t i = 0; i < sliders.size(); ++i)
    {
        addAndMakeVisible(sliders[i]);
        addAndMakeVisible(labels[i]);
        labels[i]->setText(label_texts[i], juce::dontSendNotification);
        labels[i]->setJustificationType(juce::Justification::centred);
        labels[i]->attachToComponent(sliders[i], false);
        sliders[i]->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        sliders[i]->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
        sliders[i]->setColour(
            juce::Slider::textBoxOutlineColourId,
            juce::Colours::transparentBlack
        );
        sliders[i]->setColour(
            juce::Slider::textBoxTextColourId, AuroraColors::grey3
        );
        slider_attachments.push_back(
            std::make_unique<
                juce::AudioProcessorValueTreeState::SliderAttachment>(
                parameters, parameter_ids[i], *sliders[i]
            )
        );
    }
}

OverdriveGui::~OverdriveGui()
{
}

void OverdriveGui::resized()
{

    const int xpadding = 50;
    const int ypadding = 50;
    auto bounds = getLocalBounds().reduced(xpadding, ypadding);

    // footer layout
    const int footer_height = 50;
    const int button_size = 50;
    const int label_padding = 10;

    auto footer_bounds = bounds.removeFromBottom(footer_height);
    bypass_button.setBounds(footer_bounds.removeFromRight(button_size));
    switcher_button.setBounds(footer_bounds.removeFromLeft(button_size));
    type_label.setBounds(footer_bounds
                             .removeFromLeft(footer_bounds.getWidth() / 2)
                             .withTrimmedLeft(label_padding));
    bypass_label.setBounds(footer_bounds.withTrimmedRight(label_padding));

    // middle layout
    const int middle_top_padding = 40;
    const int middle_bottom_padding = 60;

    auto middle_bounds = bounds.withTrimmedBottom(middle_bottom_padding)
                             .withTrimmedTop(middle_top_padding);

    const int knob_size = middle_bounds.getWidth() / 4;
    const int inner_knob_padding = 30;

    level_slider.setBounds(
        middle_bounds.removeFromLeft(knob_size).reduced(inner_knob_padding)
    );
    drive_slider.setBounds(
        middle_bounds.removeFromLeft(knob_size).reduced(inner_knob_padding)
    );
    character_slider.setBounds(
        middle_bounds.removeFromLeft(knob_size).reduced(inner_knob_padding)
    );
    mix_slider.setBounds(middle_bounds.reduced(inner_knob_padding));

    // Don't forget to set compressor colour based on updated label value
    switchColour();
}

void OverdriveGui::switchColour()
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
    for (auto* slider : sliders)
    {
        slider->setColour(
            juce::Slider::rotarySliderFillColourId, overdrive_colour
        );
    }
    repaint();
}

#include "preamp_footer_component.h"
#include "dimensions.h"
#include "looks/preamp_footer_look_and_feel.h"

PreampFooterComponent::PreampFooterComponent(
    juce::AudioProcessorValueTreeState& params, juce::Value& v
)
    : parameters(params), gain_reduction_value(v)
{
    setLookAndFeel(new PreampFooterLookAndFeel());

    addAndMakeVisible(compressor_bypass_button);
    addAndMakeVisible(overdrive_bypass_button);
    addAndMakeVisible(compressor_bypass_label);
    addAndMakeVisible(overdrive_bypass_label);
    addAndMakeVisible(gain_reduction_slider);

    compressor_bypass_label.setText("COMP", juce::dontSendNotification);
    compressor_bypass_label.setJustificationType(juce::Justification::right);
    compressor_bypass_attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, "compressor_bypass", compressor_bypass_button
        );

    overdrive_bypass_label.setText("DRIVE", juce::dontSendNotification);
    overdrive_bypass_label.setJustificationType(juce::Justification::left);
    overdrive_bypass_attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, "overdrive_bypass", overdrive_bypass_button
        );

    gain_reduction_slider.setRange(0, 15.0f, 0.5f);
    gain_reduction_slider.setSkewFactor(1.0);
    gain_reduction_slider.setValue(0.0);
    gain_reduction_slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    gain_reduction_value.addListener(this);
    gain_reduction_value.setValue(0);
}

PreampFooterComponent::~PreampFooterComponent()
{
}

void PreampFooterComponent::valueChanged(juce::Value& v)
{
    double value = -1 * static_cast<double>(v.getValue());
    gain_reduction_slider.setValue(value, juce::dontSendNotification);
}

void PreampFooterComponent::paint(juce::Graphics& g)
{
    // g.fillAll(AuroraColors::grey0);
}

void PreampFooterComponent::resized()
{
    auto bounds = getLocalBounds();
    compressor_bypass_label.setBounds(
        bounds.removeFromLeft(GuiDimensions::PREAMP_FOOTER_LABEL_WIDTH)
    );
    overdrive_bypass_label.setBounds(
        bounds.removeFromRight(GuiDimensions::PREAMP_FOOTER_LABEL_WIDTH)
    );
    compressor_bypass_button.setBounds(
        bounds.removeFromLeft(GuiDimensions::PREAMP_BYPASS_BUTTON_WIDTH)
    );
    overdrive_bypass_button.setBounds(
        bounds.removeFromRight(GuiDimensions::PREAMP_BYPASS_BUTTON_WIDTH)
    );
    gain_reduction_slider.setBounds(bounds.withSizeKeepingCentre(
        GuiDimensions::PREAMP_COMPRESSOR_SLIDER_WIDTH,
        GuiDimensions::PREAMP_COMPRESSOR_SLIDER_HEIGHT
    ));
}

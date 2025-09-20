#include "ir.h"
#include "looks/colors.h"

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

IRLoader::IRLoader(juce::AudioProcessorValueTreeState& params)
    : parameters(params)
{
    // Set up the "Load File" button
    addAndMakeVisible(loadButton);
    loadButton.setButtonText("LOAD IR");
    loadButton.onClick = [this] { chooseFile(); };
    loadButton.setColour(
        juce::TextButton::buttonColourId, juce::Colours::transparentBlack
    );

    addAndMakeVisible(bypassLabel);
    bypassLabel.setText("BYPASS", juce::dontSendNotification);
    bypassLabel.setJustificationType(juce::Justification::right);

    addAndMakeVisible(bypassButton);
    bypassButton.setButtonText("BYPASS");
    bypassButton.onClick = [this]() { switchColour(); };
    bypassButtonAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, "ir_bypass", bypassButton
        );

    addAndMakeVisible(statusLabel);
    statusLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(irMixSlider);
    irMixSlider.setRange(0.0, 1.0, 0.01);
    irMixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    irMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    irMixSlider.setColour(
        juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack
    );
    irMixSlider.setColour(
        juce::Slider::textBoxTextColourId, AuroraColors::grey3
    );
    irMixSlider.setColour(
        juce::Slider::rotarySliderFillColourId, AuroraColors::white0
    );
    irMixSliderAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "compressor_mix", irMixSlider
        );

    // Refresh the status of the IR loader
    refreshStatus();
    switchColour();
}

IRLoader::~IRLoader()
{
}

void IRLoader::paint(juce::Graphics& g)
{
    // g.fillAll(
    //     getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId)
    // );
}

void IRLoader::resized()
{
    const int xpadding = 50;
    const int ypadding = 50;
    const int load_button_height = 30;
    const int button_size = 50;
    const int knob_padding = 60;
    const int label_padding = 20;
    auto bounds = getLocalBounds().reduced(xpadding, ypadding);

    loadButton.setBounds(bounds.removeFromTop(load_button_height));

    auto bottom_bounds = bounds.removeFromBottom(button_size);
    bypassButton.setBounds(bottom_bounds.removeFromRight(button_size));
    statusLabel.setBounds(bottom_bounds
                              .removeFromLeft(bottom_bounds.getWidth() / 2)
                              .withTrimmedLeft(label_padding));
    bypassLabel.setBounds(bottom_bounds.withTrimmedRight(label_padding));

    irMixSlider.setBounds(bounds.reduced(knob_padding));
}

void IRLoader::switchColour()
{
    if (bypassButton.getToggleState())
    {
        iRColour = AuroraColors::white0;
    }
    else
    {
        iRColour = AuroraColors::grey3;
    }
    irMixSlider.setColour(juce::Slider::rotarySliderFillColourId, iRColour);
    repaint();
}

void IRLoader::refreshStatus()
{
    choosenFilePath = parameters.state.getProperty("ir_filepath", "");
    juce::File file(choosenFilePath);
    if (choosenFilePath.isEmpty() || !file.existsAsFile())
    {
        statusLabel.setText("NO FILE LOADED", juce::dontSendNotification);
        irMixSlider.setColour(
            juce::Slider::rotarySliderFillColourId, AuroraColors::grey3
        );
        bypassButton.setColour(juce::ToggleButton::tickColourId, AuroraColors::grey3);
    }
    else if (file.existsAsFile())
    {
        statusLabel.setText(
            "LOADED " + file.getFileName(), juce::dontSendNotification
        );
        irMixSlider.setColour(juce::Slider::rotarySliderFillColourId, iRColour);
        bypassButton.setColour(
            juce::ToggleButton::tickColourId, iRColour
        );
    }
    repaint();
}

void IRLoader::chooseFile()
{
    // Create a FileChooser to let the user select a file
    chooser = std::make_unique<juce::FileChooser>(
        "Select an Impulse Response File", juce::File(), "*.wav"
    );

    chooser->launchAsync(
        juce::FileBrowserComponent::openMode |
            juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc)
        {
            const auto resultFile = fc.getResult();

            if (resultFile.existsAsFile())
            {
                choosenFilePath = resultFile.getFullPathName();
                parameters.state.setProperty(
                    "ir_filepath", choosenFilePath, nullptr
                );
            }
            refreshStatus();
        }
    );
}

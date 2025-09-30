#include "plugin_editor.h"

#include "gui/looks/base_look_and_feel.h"
#include "plugin_audio_processor.h"
#include <juce_core/juce_core.h>

using namespace juce;

//==============================================================================
PluginEditor::PluginEditor(
    PluginAudioProcessor& p, juce::AudioProcessorValueTreeState& params
)
    : AudioProcessorEditor(&p), processorRef(p), parameters(params),
      header(params, processorRef.inputLevel, processorRef.outputLevel),
      tabs(params, processorRef.compressorGainReductionDb)
{

    setLookAndFeel(new BaseLookAndFeel());
    setSize(900, 650);
    addAndMakeVisible(header);
    addAndMakeVisible(tabs);

    juce::ignoreUnused(processorRef);

    getLookAndFeel().setColour(
        juce::Slider::thumbColourId, juce::Colours::lightgreen
    );
}

PluginEditor::~PluginEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void PluginEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    juce::Random random(3);
    const int gridSize = 30; // Space between potential dots

    for (int x = 0; x < getWidth(); x += gridSize)
    {
        for (int y = 0; y < getHeight(); y += gridSize)
        {
            if (random.nextFloat() > 0.7f) // 30% chance of dot
            {
                float offsetX = random.nextFloat() * gridSize;
                float offsetY = random.nextFloat() * gridSize;
                float size = random.nextFloat() * 3.0f + 0.5f;
                float alpha = random.nextFloat() * 0.5f + 0.05f;

                g.setColour(juce::Colours::white.withAlpha(alpha));
                g.fillEllipse(x + offsetX, y + offsetY, size, size);
            }
        }
    }
}

// void PluginEditor::paint(juce::Graphics& g)
// {
//     g.fillAll(juce::Colours::black);
//
//     juce::Random random(12345);
//     const int gridSize = 30;    // Space between dots
//     const float dotSize = 3.0f; // Fixed dot size
//
//     for (int x = 0; x < getWidth(); x += gridSize)
//     {
//         for (int y = 0; y < getHeight(); y += gridSize)
//         {
//             float alpha = random.nextFloat() * 0.2f + 0.05f; // Random alpha
//
//             g.setColour(juce::Colours::grey.withAlpha(alpha));
//             g.fillEllipse(
//                 x - dotSize / 2.0f, // Center the dot on grid point
//                 y - dotSize / 2.0f, dotSize, dotSize
//             );
//         }
//     }
//
//     // ... rest of your UI
// }

void PluginEditor::resized()
{
    const float header_ratio = 0.1f;
    const int header_height = static_cast<int>(getHeight() * header_ratio);
    auto bounds = getLocalBounds();
    header.setBounds(bounds.removeFromTop(header_height));
    tabs.setBounds(bounds);
}

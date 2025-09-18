#include "tabs.h"
#include "compressor_gui.h"

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

Tabs::Tabs(juce::AudioProcessorValueTreeState& params)
    : juce::TabbedComponent(juce::TabbedButtonBar::TabsAtTop),
      parameters(params) {
    addTab("Compressor", juce::Colours::red, new CompressorGui(params), true);
    addTab("Compressor2", juce::Colours::red, new CompressorGui(params), true);

    // addAndMakeVisible(getTabbedButtonBar());
}

Tabs::~Tabs() {}


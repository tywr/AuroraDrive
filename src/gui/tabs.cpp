#include "tabs.h"
#include "compressor_gui.h"
#include "looks/colors.h"

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

Tabs::Tabs(
    juce::AudioProcessorValueTreeState& params,
    juce::Value& compressorGainReductionDb
)
    : juce::TabbedComponent(juce::TabbedButtonBar::TabsAtTop),
      parameters(params)
{
    addTab(
        "Compressor", AuroraColors::bg,
        new CompressorGui(params, compressorGainReductionDb), true
    );
}

Tabs::~Tabs()
{
}

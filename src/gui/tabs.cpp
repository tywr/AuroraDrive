#include "tabs.h"
#include "compressor_gui.h"
#include "ir_gui.h"
#include "looks/colors.h"
#include "overdrive_gui.h"

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
        "COMP", AuroraColors::bg,
        new CompressorGui(params, compressorGainReductionDb), true
    );
    addTab("OVERDRIVE", AuroraColors::bg, new OverdriveGui(params), true);
    addTab("CHORUS", AuroraColors::bg, new juce::Component(), true);
    addTab("IR", AuroraColors::bg, new IRLoader(params), true);
    setTabBarDepth(60);
}

Tabs::~Tabs()
{
}

#include "tabs.h"
#include "compressor/compressor_component.h"
#include "ir_gui.h"
#include "looks/colors.h"
#include "preamp_component.h"

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
        new CompressorComponent(params, compressorGainReductionDb), true
    );
    addTab(
        "PREAMP", AuroraColors::bg,
        new PreAmpComponent(params, compressorGainReductionDb), true
    );
    addTab("CHORUS", AuroraColors::bg, new juce::Component(), true);
    addTab("IR", AuroraColors::bg, new IRLoader(params), true);
    setTabBarDepth(60);
}

Tabs::~Tabs()
{
}

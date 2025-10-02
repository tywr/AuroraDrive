#pragma once

#include "../colours.h"
#include "amp_knobs_component.h"
#include "utils/perlin.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

struct AmpType
{
    juce::ToggleButton* button;
    juce::String id;
    juce::Colour colour1;
    juce::Colour colour2;
};

class AmpComponent : public juce::Component
{
  public:
    AmpComponent(juce::AudioProcessorValueTreeState&);
    ~AmpComponent() override;

    void setColours(juce::Colour, juce::Colour);
    void initType();
    void switchType(AmpType);

    void resized() override;

    void paint(juce::Graphics&) override;
    void paintTypeButtons(juce::Graphics&);
    void paintDesign(juce::Graphics&, juce::Rectangle<float>);
    void paintBorder(juce::Graphics&, juce::Rectangle<float>, float);

  private:
    juce::AudioProcessorValueTreeState& parameters;
    AmpKnobsComponent knobs_component;
    juce::ToggleButton bypass_button;

    juce::Slider type_slider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        type_slider_attachment;

    juce::Colour current_colour1;
    juce::Colour current_colour2;

    juce::ToggleButton helios_button;
    juce::ToggleButton borealis_button;

    std::vector<AmpType> types = {
        {&helios_button,   "helios",   ColourCodes::helios_yellow,
         ColourCodes::helios_orange},
        {&borealis_button, "borealis", juce::Colours::darkblue,
         juce::Colours::blue       }
    };

    AmpType selected_type = types[0];
    PerlinNoise perlin_noise;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AmpComponent)
};

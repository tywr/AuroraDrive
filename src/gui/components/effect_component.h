#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class EffectComponent : public juce::Component
{
  public:
    EffectComponent(
        juce::AudioProcessorValueTreeState& parameters,
        const juce::String& title,
        const juce::String& bypassParameterId
    );
    ~EffectComponent() override = default;

    void paint(juce::Graphics&) final;
    void resized() final;

  protected:
    bool isBypassed() const;
    juce::Colour getAccentColour() const;
    juce::Colour getTextColour() const;

    virtual void paintContent(juce::Graphics&) = 0;
    virtual void resizedContent(juce::Rectangle<int>) = 0;
    virtual void bypassStateChanged() {}

  private:
    juce::Label titleLabel;
    juce::ToggleButton bypassButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        bypassAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectComponent)
};

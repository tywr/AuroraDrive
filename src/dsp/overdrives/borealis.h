#pragma once

#include "../circuits/germanium_diode.h"
#include "overdrive.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class BorealisOverdrive : public Overdrive
{
  public:
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    float driveToGain(float) override;
    float charToFreq(float);
    void applyOverdrive(float& sample, float sampleRate) override;

  private:
    juce::dsp::IIR::Filter<float> attack_shelf;
    float attack_shelf_freq = 2800.0f;
    float attack_shelf_gain = juce::Decibels::decibelsToGain(12.0f);

    juce::dsp::IIR::Filter<float> dc_hpf;
    float dc_hpf_cutoff = 10.0f;

    juce::dsp::IIR::Filter<float> pre_hpf;
    float pre_hpf_cutoff = 72.0f;

    juce::dsp::IIR::Filter<float> post_lpf;
    float post_lpf_cutoff = 1600.0f;

    juce::dsp::IIR::Filter<float> post_lpf2;
    float post_lpf2_cutoff = 10000.0f;

    float padding = 5.0f;

    GermaniumDiode diode = GermaniumDiode(44100.0f);

    juce::dsp::Oversampling<float> oversampler2x{
        2, 2,
        juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
        true, false
    };
};

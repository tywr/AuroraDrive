#pragma once

#include "../circuits/triode.h"
#include "overdrive.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class HeliosOverdrive : public Overdrive
{
  public:
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    float driveToGain(float) override;
    float charToFreq(float);
    void applyOverdrive(float& sample, float sampleRate) override;

  private:
    juce::dsp::IIR::Filter<float> pre_hpf;
    float pre_hpf_cutoff = 30.0f;

    juce::dsp::IIR::Filter<float> mid_scoop;
    float mid_scoop_frequency = 600.0f;
    float mid_scoop_q = 0.5f;
    float mid_scoop_gain = juce::Decibels::decibelsToGain(-3.0f);

    juce::dsp::IIR::Filter<float> tone_lpf;
    float tone_lpf_cutoff = 1.0f;

    juce::dsp::IIR::Filter<float> dc_hpf;
    float dc_hpf_cutoff = 20.0f;

    juce::dsp::IIR::Filter<float> dc_hpf2;
    float dc_hpf2_cutoff = 20.0f;

    juce::dsp::IIR::Filter<float> post_lpf;
    float post_lpf_cutoff = 3400.0f;

    // triode parameters

    float padding = juce::Decibels::decibelsToGain(-16.0f);
    Triode triode = Triode(44100);
    Triode triode2 = Triode(44100);
    Triode triode_pre = Triode(44100);
    Triode triode_pre2 = Triode(44100);

    juce::dsp::Oversampling<float> oversampler2x{
        2, 2,
        juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
        true, false
    };
};

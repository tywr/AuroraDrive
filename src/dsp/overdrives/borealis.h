#pragma once

#include "../circuits/bjt.h"
#include "../circuits/germanium_diode.h"
#include "overdrive.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class BorealisOverdrive : public Overdrive
{
  public:
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void setCoefficients();
    float charToGain(float);
    float driveToGain(float) override;
    float driveToFrequency(float);
    void applyOverdrive(float& sample, float sampleRate) override;

  private:
    juce::dsp::IIR::Filter<float> ff1_lpf;
    float ff1_lpf_cutoff = 106.0f;

    juce::dsp::IIR::Filter<float> ff2_hpf;
    float smoothed_ff2_frequency = 454.0f;

    juce::dsp::IIR::Filter<float> ff2_lpf;
    float ff2_lpf_cutoff = 272.0f;

    juce::dsp::IIR::Filter<float> attack_shelf;
    float attack_shelf_freq = 800.0f;
    float smoothed_attack_shelf_gain = 1.0f;

    juce::dsp::IIR::Filter<float> pre_hpf;
    float pre_hpf_cutoff = 129.0f;

    juce::dsp::IIR::Filter<float> pre_lpf;
    float pre_lpf_cutoff = 967.0f;

    juce::dsp::IIR::Filter<float> post_lpf;
    float post_lpf_cutoff = 3400.0f;
    float post_lpf_q = 0.57;

    float padding = 5.0f;

    GermaniumDiode diode = GermaniumDiode(44100.0f);

    juce::dsp::Oversampling<float> oversampler2x{
        2, 2,
        juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
        true, false
    };

    float smoothing_factor = 0.1f;
};

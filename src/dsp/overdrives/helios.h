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
    juce::dsp::IIR::Filter<float> tone_lpf;
    float tone_lpf_cutoff = 1.0f;

    juce::dsp::IIR::Filter<float> post_lpf;
    float post_lpf_cutoff = 6000.0f;

    // internal parameters
    float padding = 1 / 60.0f;

    // triode parameters
    float kp = 1.014e-5;
    float kp2 = 5.498e-8;
    float kpg = 1.076e-5;
    float E = 250;
    float Ri = 1e6;
    float Rg = 20e3;
    float Ck = 10e-6;
    float Co = 10e-9;
    float Rp = 100e3;
    float Ro = 1e6;
    float Rk = 1e3;
    float Ci = 100e-9;

    Triode triode =
        Triode(44100, kp, kp2, kpg, E, Ci, Co, Ck, Ri, Ro, Rp, Rk, Rg);

    juce::dsp::Oversampling<float> oversampler2x{
        2, 2,
        juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
        true, false
    };
};

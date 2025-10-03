#pragma once

#include "../circuits/diode.h"
#include "overdrive.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class BorealisOverdrive : public Overdrive
{
  public:
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    float driveToGain(float) override;
    void applyOverdrive(float& sample, float sampleRate) override;

  private:
    juce::dsp::IIR::Filter<float> high_pass_filter;
    juce::dsp::IIR::Filter<float> low_pass_filter;

    // hardcoded parameters for each overdrive type
    struct
    {
        float hpf_cutoff = 120.0f;
        float lpf_cutoff = 8000.0f;
    } borealis_params;

    // internal parameters
    float padding = 1 / 50.0f;

    float c = 1e-8;
    float r = 2200;
    float i_s = 200e-9;
    float v_t = 0.02585;

    Diode diode = Diode(44100.0f, i_s, r, c, v_t);

    juce::dsp::Oversampling<float> oversampler2x{
        2, 2,
        juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
        true, false
    };
};

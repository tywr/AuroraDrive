#include "Helios.h"
#include "../circuits/triode.h"

#include <juce_dsp/juce_dsp.h>

void HeliosOverdrive::prepare(const juce::dsp::ProcessSpec& spec)
{
    juce::dsp::ProcessSpec oversampled_spec = spec;
    oversampled_spec.sampleRate *= 2;
    processSpec = oversampled_spec;

    oversampler2x.reset();
    oversampler2x.initProcessing(static_cast<size_t>(spec.maximumBlockSize));

    tone_lpf.prepare(oversampled_spec);
    auto tone_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            oversampled_spec.sampleRate, tone_lpf_cutoff
        );
    *tone_lpf.coefficients = *tone_lpf_coefficients;

    post_lpf.prepare(oversampled_spec);
    auto post_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            oversampled_spec.sampleRate, post_lpf_cutoff
        );
    *post_lpf.coefficients = *post_lpf_coefficients;

    triode = Triode(
        oversampled_spec.sampleRate, kp, kp2, kpg, E, Ci, Co, Ck, Ri, Ro, Rp,
        Rk, Rg
    );
    triode.initializeState();
}

float HeliosOverdrive::driveToGain(float d)
{
    float t = d / 10.0f;
    return 1.0f + std::pow(t, 2) * 20.0f;
}

float HeliosOverdrive::charToFreq(float c)
{
    float t = character / 10.0f;
    float max_value = 15000.0f;
    float min_value = 500.0f;
    return min_value + std::pow(t, 2) * (max_value - min_value);
}

void HeliosOverdrive::process(juce::AudioBuffer<float>& buffer)
{
    if (bypass)
    {
        return;
    }
    float drive_gain = driveToGain(drive);
    applyGain(buffer, previous_drive_gain, drive_gain);
    float sampleRate = static_cast<float>(processSpec.sampleRate);
    // Update tone cutoff
    float new_tone_lpf_cutoff = charToFreq(character);

    if (!juce::approximatelyEqual(tone_lpf_cutoff, new_tone_lpf_cutoff))
    {
        tone_lpf_cutoff = new_tone_lpf_cutoff;
        DBG("Borealis tone cutoff: " << tone_lpf_cutoff);
        auto tone_lpf_coefficients =
            juce::dsp::IIR::Coefficients<float>::makeLowPass(
                sampleRate, tone_lpf_cutoff
            );
        *tone_lpf.coefficients = *tone_lpf_coefficients;
    }

    juce::dsp::AudioBlock<float> block(buffer);
    auto oversampledBlock = oversampler2x.processSamplesUp(block);

    auto* channelData = oversampledBlock.getChannelPointer(0);
    for (size_t i = 0; i < oversampledBlock.getNumSamples(); ++i)
    {
        float sample = channelData[i];
        applyOverdrive(sample, sampleRate);
        channelData[i] = sample;
    }
    oversampler2x.processSamplesDown(block);

    applyGain(buffer, previous_level, level);
};

void HeliosOverdrive::applyOverdrive(float& sample, float sampleRate)
{
    juce::ignoreUnused(sampleRate);

    float filtered = tone_lpf.processSample(sample);
    float distorded = triode.processSample(filtered);
    float out = padding * post_lpf.processSample(distorded);

    // Apply mix and low pass filter
    sample = out * mix + sample * (1 - mix);
}

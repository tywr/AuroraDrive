#include "amp_eq.h"

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

void AmpEQ::prepare(const juce::dsp::ProcessSpec& spec)
{
    processSpec = spec;
    bass_shelf.prepare(processSpec);
    low_mid_peak.prepare(processSpec);
    high_mid_peak.prepare(processSpec);
    treble_peak.prepare(processSpec);
}

void AmpEQ::setCoefficients()
{
    if (!juce::approximatelyEqual(smoothed_bass_gain, bass_gain))
    {
        smoothed_bass_gain +=
            (bass_gain - smoothed_bass_gain) * smoothing_factor;
        auto bass_shelf_coefficients =
            juce::dsp::IIR::Coefficients<float>::makeLowShelf(
                processSpec.sampleRate, bass_shelf_frequency, bass_shelf_q,
                smoothed_bass_gain
            );
        *bass_shelf.coefficients = *bass_shelf_coefficients;
    }
    if (!juce::approximatelyEqual(smoothed_low_mid_gain, low_mid_gain))
    {
        smoothed_low_mid_gain +=
            (low_mid_gain - smoothed_low_mid_gain) * smoothing_factor;
        auto low_mid_peak_coefficients =
            juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                processSpec.sampleRate, low_mid_peak_frequency, low_mid_peak_q,
                smoothed_low_mid_gain
            );
        *low_mid_peak.coefficients = *low_mid_peak_coefficients;
    }
    if (!juce::approximatelyEqual(smoothed_high_mid_gain, high_mid_gain))
    {
        smoothed_high_mid_gain +=
            (high_mid_gain - smoothed_high_mid_gain) * smoothing_factor;
        auto high_mid_peak_coefficients =
            juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                processSpec.sampleRate, high_mid_peak_frequency,
                high_mid_peak_q, smoothed_high_mid_gain
            );
        *high_mid_peak.coefficients = *high_mid_peak_coefficients;
    }
    if (!juce::approximatelyEqual(smoothed_treble_gain, treble_gain))
    {
        smoothed_treble_gain +=
            (treble_gain - smoothed_treble_gain) * smoothing_factor;
        auto treble_peak_coefficients =
            juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                processSpec.sampleRate, treble_peak_frequency, treble_peak_q,
                smoothed_treble_gain
            );
        *treble_peak.coefficients = *treble_peak_coefficients;
    }
}

void AmpEQ::process(juce::AudioBuffer<float>& buffer)
{
    if (bypass)
    {
        return;
    }
    float sampleRate = static_cast<float>(processSpec.sampleRate);
    setCoefficients();

    auto* channelData = buffer.getWritePointer(0);
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        float sample = channelData[i];
        applyEQ(sample, sampleRate);
        channelData[i] = sample;
    }
}

void AmpEQ::applyEQ(float& sample, float sampleRate)
{
    juce::ignoreUnused(sampleRate);
    sample = bass_shelf.processSample(sample);
    sample = low_mid_peak.processSample(sample);
    sample = high_mid_peak.processSample(sample);
    sample = treble_peak.processSample(sample);
}

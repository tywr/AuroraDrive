#include "amp_eq.h"

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

void AmpEQ::prepare(const juce::dsp::ProcessSpec& spec)
{
    processSpec = spec;
    bass_shelf.prepare(processSpec);
    low_mid_peak.prepare(processSpec);
    high_mid_peak.prepare(processSpec);
    treble_shelf.prepare(processSpec);
}

void AmpEQ::setBassShelfCoefficients()
{
    if (!juce::approximatelyEqual(smoothed_bass_gain, bass_gain))
    {
        DBG("Updating bass shelf gain to " << bass_gain);
        smoothed_bass_gain +=
            (bass_gain - smoothed_bass_gain) * smoothing_factor;
        auto bass_shelf_coefficients =
            juce::dsp::IIR::Coefficients<float>::makeLowShelf(
                processSpec.sampleRate, bass_shelf_frequency, bass_shelf_q,
                smoothed_bass_gain
            );
        *bass_shelf.coefficients = *bass_shelf_coefficients;
    }
}

void AmpEQ::process(juce::AudioBuffer<float>& buffer)
{
    if (bypass)
    {
        return;
    }
    float sampleRate = static_cast<float>(processSpec.sampleRate);
    setBassShelfCoefficients();

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
}

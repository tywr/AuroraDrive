#include "plugin_editor.h"

#include "parameters.h"
#include "plugin_audio_processor.h"
#include <juce_dsp/juce_dsp.h>

//==============================================================================
PluginAudioProcessor::PluginAudioProcessor()
    : AudioProcessor(
          BusesProperties()
              .withInput("Input", juce::AudioChannelSet::mono(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
      ),
      parameters(
          *this, nullptr, juce::Identifier("PluginParameters"),
          createParameterLayout()
      )
{
    parameters.state.setProperty("ir_filepath", juce::String(""), nullptr);

    inputGainParameter = parameters.getRawParameterValue("input_gain_db");
    outputGainParameter = parameters.getRawParameterValue("output_gain_db");

    parameters.addParameterListener("input_gain_db", this);
    parameters.addParameterListener("output_gain_db", this);
    parameters.addParameterListener("compressor_bypass", this);
    parameters.addParameterListener("compressor_threshold", this);
    parameters.addParameterListener("compressor_ratio", this);
    parameters.addParameterListener("compressor_level_db", this);
    parameters.addParameterListener("compressor_type", this);
    parameters.addParameterListener("compressor_mix", this);
    parameters.addParameterListener("amp_type", this);
    parameters.addParameterListener("overdrive_bypass", this);
    parameters.addParameterListener("overdrive_level_db", this);
    parameters.addParameterListener("overdrive_drive", this);
    parameters.addParameterListener("overdrive_character", this);
    parameters.addParameterListener("overdrive_mix", this);
    parameters.addParameterListener("ir_bypass", this);
    parameters.addParameterListener("ir_mix", this);
    parameters.addParameterListener("ir_filepath", this);
    parameters.addParameterListener("ir_gain_db", this);
}

PluginAudioProcessor::~PluginAudioProcessor()
{
}

//==============================================================================
const juce::String PluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginAudioProcessor::acceptsMidi() const
{
    return false;
}

bool PluginAudioProcessor::producesMidi() const
{
    return false;
}

bool PluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are
              // 0 programs, so this should be at least 1, even if you're not
              // really implementing programs.
}

int PluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PluginAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String PluginAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void PluginAudioProcessor::changeProgramName(
    int index, const juce::String& newName
)
{
    juce::ignoreUnused(index, newName);
}
//==============================================================================
void PluginAudioProcessor::parameterChanged(
    const juce::String& parameterID, float newValue
)
{
    if (parameterID == "compressor_bypass")
    {
        compressor.setBypass(newValue >= 0.5f);
    }
    else if (parameterID == "compressor_ratio")
    {
        const float values[] = {2.0f, 4.0f, 8.0f, 12.0f, 20.0f};
        int index = static_cast<int>(newValue);
        compressor.setRatio(values[index]);
    }
    else if (parameterID == "compressor_threshold")
    {
        compressor.setThreshold(juce::Decibels::decibelsToGain(newValue));
    }
    else if (parameterID == "compressor_level_db")
    {
        compressor.setLevel(juce::Decibels::decibelsToGain(newValue));
    }
    else if (parameterID == "compressor_type")
    {
        compressor.setTypeFromIndex(static_cast<int>(newValue));
    }
    else if (parameterID == "compressor_mix")
    {
        compressor.setMix(static_cast<int>(newValue) / 100.0f);
    }
    // Amp type
    if (parameterID == "amp_type")
    {
        int index = static_cast<int>(newValue);
        current_overdrive = overdrives[index];
    }
    // Overdrive
    if (parameterID == "overdrive_bypass")
    {
        for (auto& overdrive : overdrives)
        {
            overdrive->setBypass(newValue >= 0.5f);
        }
    }
    else if (parameterID == "overdrive_mix")
    {
        for (auto& overdrive : overdrives)
        {
            overdrive->setMix(newValue);
        }
    }
    else if (parameterID == "overdrive_level_db")
    {
        for (auto& overdrive : overdrives)
        {
            overdrive->setLevel(juce::Decibels::decibelsToGain(newValue));
        }
    }
    else if (parameterID == "overdrive_drive")
    {
        for (auto& overdrive : overdrives)
        {
            overdrive->setDrive(newValue);
        }
    }
    else if (parameterID == "overdrive_character")
    {
        for (auto& overdrive : overdrives)
        {
            overdrive->setCharacter(newValue);
        }
    }
    // Impulse Response Convolver
    else if (parameterID == "ir_bypass")
    {
        irConvolver.setBypass((newValue < 0.5f) ? true : false);
    }
    else if (parameterID == "ir_mix")
    {
        irConvolver.setMix(newValue);
    }
    else if (parameterID == "ir_gain_db")
    {
        irConvolver.setGain(juce::Decibels::decibelsToGain(newValue));
    }
    else if (parameterID == "ir_filepath")
    {
        // Load IR from the new filepath
        juce::String newFilepath =
            parameters.state.getProperty("ir_filepath").toString();
        irConvolver.setFilepath(newFilepath);
        irConvolver.loadIR();
    }
}

//==============================================================================
void PluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused(sampleRate, samplesPerBlock);
    previousInputGainLinear =
        juce::Decibels::decibelsToGain(inputGainParameter->load());
    previousOutputGainLinear =
        juce::Decibels::decibelsToGain(outputGainParameter->load());

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (juce::uint32)samplesPerBlock;
    spec.numChannels = (juce::uint32)getTotalNumOutputChannels();

    compressor.prepare(spec);

    // Set all initial values from compressor
    compressor.setBypass(
        parameters.getRawParameterValue("compressor_bypass")->load() >= 0.5f
    );
    const float values[] = {2.0f, 4.0f, 8.0f, 12.0f, 20.0f};
    int index = static_cast<int>(
        parameters.getRawParameterValue("compressor_ratio")->load()
    );
    compressor.setRatio(values[index]);
    compressor.setThreshold(
        juce::Decibels::decibelsToGain(
            parameters.getRawParameterValue("compressor_threshold")->load()
        )
    );
    compressor.setLevel(
        juce::Decibels::decibelsToGain(
            parameters.getRawParameterValue("compressor_level_db")->load()
        )
    );
    compressor.setTypeFromIndex(
        static_cast<int>(
            parameters.getRawParameterValue("compressor_type")->load()
        )
    );

    int amp_index =
        static_cast<int>(parameters.getRawParameterValue("amp_type")->load());
    current_overdrive = overdrives[amp_index];

    // Set all initial values from overdrive
    for (auto& overdrive : overdrives)
    {
        overdrive->prepare(spec);
        overdrive->setBypass(
            parameters.getRawParameterValue("overdrive_bypass")->load() < 0.5f
        );
        overdrive->setMix(
            parameters.getRawParameterValue("overdrive_mix")->load()
        );
        overdrive->setLevel(
            juce::Decibels::decibelsToGain(
                parameters.getRawParameterValue("overdrive_level_db")->load()
            )
        );
        overdrive->setDrive(
            parameters.getRawParameterValue("overdrive_drive")->load()
        );
        overdrive->setCharacter(
            parameters.getRawParameterValue("overdrive_character")->load()
        );
    }

    // Set all initial values for IR convolution
    irConvolver.prepare(spec);
    irConvolver.setBypass(
        parameters.getRawParameterValue("ir_bypass")->load() < 0.5f
    );
    irConvolver.setMix(parameters.getRawParameterValue("ir_mix")->load());
    irConvolver.setFilepath(
        parameters.state.getProperty("ir_filepath").toString()
    );
    irConvolver.loadIR();
}

void PluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool PluginAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts
) const
{
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

//==============================================================================
// Main Proces Block Function !
//==============================================================================

void PluginAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages
)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    applyInputGain(buffer);
    updateInputLevel(buffer);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        juce::ignoreUnused(channelData);
    }

    compressor.process(buffer);
    compressorGainReductionDb.setValue(compressor.getGainReductionDb());

    current_overdrive->process(buffer);

    irConvolver.process(buffer);

    applyOutputGain(buffer);
    updateOutputLevel(buffer);

    // Convert mono to stereo if needed
    const float* input = buffer.getReadPointer(0);
    float* left = buffer.getWritePointer(0);
    float* right = buffer.getWritePointer(1);
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        float mono_sample = input[i];
        left[i] = mono_sample;
        right[i] = mono_sample;
    }
}

//==============================================================================
// Process Block Helper functions
//==============================================================================

void PluginAudioProcessor::updateInputLevel(juce::AudioBuffer<float>& buffer)
{
    // Set inputLevel value for metering
    double peakInput = smoothLevel(
        buffer.getMagnitude(0, 0, buffer.getNumSamples()), inputLevel.getValue()
    );
    inputLevel.setValue(peakInput);
}

void PluginAudioProcessor::updateOutputLevel(juce::AudioBuffer<float>& buffer)
{
    // Set outputLevel value for metering
    double peakOutput = smoothLevel(
        buffer.getMagnitude(0, 0, buffer.getNumSamples()),
        outputLevel.getValue()
    );
    outputLevel.setValue(peakOutput);
}

void PluginAudioProcessor::applyInputGain(juce::AudioBuffer<float>& buffer)
{
    auto currentInputGainLinear =
        juce::Decibels::decibelsToGain(inputGainParameter->load());
    if (juce::approximatelyEqual(
            currentInputGainLinear, previousInputGainLinear
        ))
    {
        buffer.applyGain(currentInputGainLinear);
    }
    else
    {
        buffer.applyGainRamp(
            0, buffer.getNumSamples(), previousInputGainLinear,
            currentInputGainLinear
        );
        previousInputGainLinear = currentInputGainLinear;
    }
}

void PluginAudioProcessor::applyOutputGain(juce::AudioBuffer<float>& buffer)
{
    // Apply output gain with smoothing
    auto currentOutputGainLinear =
        juce::Decibels::decibelsToGain(outputGainParameter->load());
    if (juce::approximatelyEqual(
            currentOutputGainLinear, previousOutputGainLinear
        ))
    {
        buffer.applyGain(currentOutputGainLinear);
    }
    else
    {
        buffer.applyGainRamp(
            0, buffer.getNumSamples(), previousOutputGainLinear,
            currentOutputGainLinear
        );
        previousOutputGainLinear = currentOutputGainLinear;
    }
}

double PluginAudioProcessor::smoothLevel(double newLevel, double currentLevel)
{
    if (newLevel > currentLevel)
        return newLevel;
    else
        return decayFactor * currentLevel;
}

//==============================================================================

bool PluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginAudioProcessor::createEditor()
{
    return new PluginEditor(*this, parameters);
}

void PluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PluginAudioProcessor::setStateInformation(
    const void* data, int sizeInBytes
)
{
    std::unique_ptr<juce::XmlElement> xmlState(
        getXmlFromBinary(data, sizeInBytes)
    );
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginAudioProcessor();
}

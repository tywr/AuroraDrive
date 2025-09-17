#include "plugin_editor.h"

#include "plugin_audio_processor.h"

//==============================================================================
PluginAudioProcessor::PluginAudioProcessor()
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
              ),
      parameters(
          *this, nullptr, juce::Identifier("PluginParameters"),
          {std::make_unique<juce::AudioParameterFloat>(
               "input_gain_db", "Input Gain dB",
               juce::NormalisableRange<float>(-48.0f, 6.0f, 0.01f, 3.0f), 0.0f),
           std::make_unique<juce::AudioParameterFloat>(
               "output_gain_db", "Output Gain dB",
               juce::NormalisableRange<float>(-48.0f, 6.0f, 0.01f, 3.0f),
               0.0f)}) {
    inputGainParameter = parameters.getRawParameterValue("input_gain_db");
    outputGainParameter = parameters.getRawParameterValue("output_gain_db");
}

PluginAudioProcessor::~PluginAudioProcessor() {}

//==============================================================================
const juce::String PluginAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool PluginAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PluginAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool PluginAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PluginAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int PluginAudioProcessor::getNumPrograms() {
    return 1; // NB: some hosts don't cope very well if you tell them there are
              // 0 programs, so this should be at least 1, even if you're not
              // really implementing programs.
}

int PluginAudioProcessor::getCurrentProgram() { return 0; }

void PluginAudioProcessor::setCurrentProgram(int index) {
    juce::ignoreUnused(index);
}

const juce::String PluginAudioProcessor::getProgramName(int index) {
    juce::ignoreUnused(index);
    return {};
}

void PluginAudioProcessor::changeProgramName(int index,
                                             const juce::String& newName) {
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void PluginAudioProcessor::prepareToPlay(double sampleRate,
                                         int samplesPerBlock) {
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused(sampleRate, samplesPerBlock);
    previousInputGainLinear =
        juce::Decibels::decibelsToGain(inputGainParameter->load());
    previousOutputGainLinear =
        juce::Decibels::decibelsToGain(outputGainParameter->load());
}

void PluginAudioProcessor::releaseResources() {
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool PluginAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

void PluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                        juce::MidiBuffer& midiMessages) {
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Set inputLevel value for metering
    double smoothedInput =
        smoothLevel(buffer.getRMSLevel(0, 0, buffer.getNumSamples()),
                    inputLevel.getValue());
    inputLevel.setValue(smoothedInput);

    // Apply input gain with smoothing
    auto currentInputGainLinear =
        juce::Decibels::decibelsToGain(inputGainParameter->load());
    if (juce::approximatelyEqual(currentInputGainLinear,
                                 previousInputGainLinear)) {
        buffer.applyGain(currentInputGainLinear);
    } else {
        buffer.applyGainRamp(0, buffer.getNumSamples(), previousInputGainLinear,
                             currentInputGainLinear);
        previousInputGainLinear = currentInputGainLinear;
    }

    // Main processing goes here
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        auto* channelData = buffer.getWritePointer(channel);
        juce::ignoreUnused(channelData);
    }

    // Set outputLevel value for metering
    double smoothedOutput =
        smoothLevel(buffer.getRMSLevel(0, 0, buffer.getNumSamples()),
                    outputLevel.getValue());
    outputLevel.setValue(smoothedOutput);

    // Apply output gain with smoothing
    auto currentOutputGainLinear =
        juce::Decibels::decibelsToGain(outputGainParameter->load());
    if (juce::approximatelyEqual(currentOutputGainLinear,
                                 previousOutputGainLinear)) {
        buffer.applyGain(currentOutputGainLinear);
    } else {
        buffer.applyGainRamp(0, buffer.getNumSamples(),
                             previousOutputGainLinear, currentOutputGainLinear);
        previousOutputGainLinear = currentOutputGainLinear;
    }
}

double PluginAudioProcessor::smoothLevel(double newLevel, double currentLevel) {
    if (newLevel > currentLevel)
        return newLevel;
    else
        return decayFactor * currentLevel;
}

bool PluginAudioProcessor::hasEditor() const {
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginAudioProcessor::createEditor() {
    return new PluginEditor(*this, parameters);
}

//==============================================================================
void PluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PluginAudioProcessor::setStateInformation(const void* data,
                                               int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(
        getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new PluginAudioProcessor();
}

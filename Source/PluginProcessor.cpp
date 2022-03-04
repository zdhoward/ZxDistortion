/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ZxDistortionAudioProcessor::ZxDistortionAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    apvts.state.setProperty(PresetManager::presetNameProperty, "", nullptr);
    apvts.state.setProperty("version", ProjectInfo::versionString, nullptr);

    presetManager = std::make_unique<PresetManager>(apvts);

    using namespace Params;
    const auto& params = GetParams();

    auto floatHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<AudioParameterFloat*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };

    floatHelper(drive, Names::Drive);
    floatHelper(range, Names::Range);
    floatHelper(blend, Names::Blend);
    floatHelper(gain, Names::Gain);

}

ZxDistortionAudioProcessor::~ZxDistortionAudioProcessor()
{
    
}

//==============================================================================
const juce::String ZxDistortionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ZxDistortionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ZxDistortionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ZxDistortionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ZxDistortionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ZxDistortionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ZxDistortionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ZxDistortionAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ZxDistortionAudioProcessor::getProgramName (int index)
{
    return {};
}

void ZxDistortionAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ZxDistortionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;

    dspGain.prepare(spec);
    dspGain.setRampDurationSeconds(0.05); //50ms

    dspBlend.prepare(spec);
}

void ZxDistortionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ZxDistortionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ZxDistortionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    updateState();

    auto settings = getSettings(apvts);

    dspBlend.pushDrySamples(buffer);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        // Distort
        for (int sample = 0; sample < buffer.getNumSamples(); sample++)
        {
            float cleanSignal = *channelData;

            *channelData *= settings.drive * settings.range;

            *channelData = ((2.f / float_Pi) * atan(*channelData));

            channelData++;
        }
    }

    dspBlend.setWetMixProportion(settings.blend);
    dspBlend.mixWetSamples(buffer);

    applyGain(buffer, dspGain);
}

void ZxDistortionAudioProcessor::updateState()
{
    dspGain.setGainDecibels(gain->get());
}

//==============================================================================
bool ZxDistortionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ZxDistortionAudioProcessor::createEditor()
{
    //return new GenericAudioProcessorEditor(*this);
    return new ZxDistortionAudioProcessorEditor (*this);
}

//==============================================================================
void ZxDistortionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void ZxDistortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto tree = ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
    {
        apvts.replaceState(tree);
    }

    themeToLoad = (int)apvts.state.getProperty("Theme", 0);
    oversamplingToLoad = (int)apvts.state.getProperty("Oversampling", 0);
}

AudioProcessorValueTreeState::ParameterLayout ZxDistortionAudioProcessor::createParameterLayout() {
    APVTS::ParameterLayout layout;

    using namespace Params;
    const auto& params = GetParams();

    auto driveRange = NormalisableRange<float>(0.f, 1.f, 0.001f, .8f);
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Drive), params.at(Names::Drive), driveRange, 1.f));

    auto rangeRange = NormalisableRange<float>(0.f, 3000.f, 0.001f, .8f);
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Range), params.at(Names::Range), rangeRange, 1000.f));

    auto blendRange = NormalisableRange<float>(0.f, 1.f, 0.001f, 1.f);
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Blend), params.at(Names::Blend), blendRange, 1.f));

    auto gainRange = NormalisableRange<float>(-12.f, 12.f, 0.5f, 1.f);
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Gain), params.at(Names::Gain), gainRange, 0.f));

    return layout;
}

Settings ZxDistortionAudioProcessor::getSettings(AudioProcessorValueTreeState& apvts)
{
    Settings settings;

    settings.drive = apvts.getRawParameterValue("Drive")->load();
    settings.range = apvts.getRawParameterValue("Range")->load();
    settings.blend = apvts.getRawParameterValue("Blend")->load();
    settings.gain = apvts.getRawParameterValue("Gain")->load();

    return settings;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ZxDistortionAudioProcessor();
}
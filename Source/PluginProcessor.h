/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../Shared Resources/Library/Services/PresetManager.h"

namespace Params
{
    enum Names
    {
        Drive,
        Range,
        Blend,
        Gain,
    };

    inline const std::map<Names, String>& GetParams()
    {
        static std::map<Names, String> params =
        {
            { Drive, "Drive" },
            { Range, "Range" },
            { Blend, "Blend" },
            { Gain, "Gain" },
        };

        return params;
    };
}

struct Settings {
    float drive{ 0 }, range{ 0 }, blend{ 0 }, gain{ 0 };
};

//==============================================================================
/**
*/
class ZxDistortionAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ZxDistortionAudioProcessor();
    ~ZxDistortionAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    void updateState();

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    Settings getSettings(AudioProcessorValueTreeState& apvts);

    using APVTS = AudioProcessorValueTreeState;
    APVTS::ParameterLayout createParameterLayout();
    APVTS apvts{ *this, nullptr, "Parameters", createParameterLayout() };

    PresetManager& getPresetManager() { return *presetManager;  }

    int getSavedTheme();
    int themeToLoad{ 0 };
    //int getSavedOversampling();
    int oversamplingToLoad{ 0 };

private:
    std::unique_ptr<PresetManager> presetManager;

    AudioParameterFloat* drive;
    AudioParameterFloat* range;
    AudioParameterFloat* blend;
    AudioParameterFloat* gain;

    dsp::Gain<float> dspGain;

    template<typename T, typename U>
    void applyGain(T& buffer, U& gain)
    {
        auto block = dsp::AudioBlock<float>(buffer);
        auto ctx = dsp::ProcessContextReplacing<float>(block);
        gain.process(ctx);
    }

    dsp::DryWetMixer<float> dspBlend;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ZxDistortionAudioProcessor)
};

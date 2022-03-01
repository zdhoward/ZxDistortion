/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "../../Shared Resources/Library/LookAndFeel/ZxLookAndFeel.h"
#include "../../Shared Resources/Library/Components/PresetsBar.h"


//==============================================================================
/**
*/
class ZxDistortionAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ZxDistortionAudioProcessorEditor (ZxDistortionAudioProcessor&);
    ~ZxDistortionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ZxDistortionAudioProcessor& audioProcessor;

    ZxLookAndFeel lnf;

    PresetsBar presetsBar;

    Image img;

    Label pluginName;
    Label title;

    Label pluginVersion;

    GroupComponent knobGroup;

    Slider driveKnob;
    Slider rangeKnob;
    Slider blendKnob;
    Slider gainKnob;
    
    using APVTS = AudioProcessorValueTreeState;

    APVTS::SliderAttachment driveKnobAttachment;
    APVTS::SliderAttachment rangeKnobAttachment;
    APVTS::SliderAttachment blendKnobAttachment;
    APVTS::SliderAttachment gainKnobAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ZxDistortionAudioProcessorEditor)
};

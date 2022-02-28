/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ZxDistortionAudioProcessorEditor::ZxDistortionAudioProcessorEditor(ZxDistortionAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
    driveKnobAttachment(audioProcessor.apvts, "Drive", driveKnob),
    rangeKnobAttachment(audioProcessor.apvts, "Range", rangeKnob),
    blendKnobAttachment(audioProcessor.apvts, "Blend", blendKnob),
    gainKnobAttachment(audioProcessor.apvts, "Gain", gainKnob),

    presetsBar(p.getPresetManager(), lnf.theme)
{
    lnf.setTheme(Themes::InkPink);
    setLookAndFeel(&lnf);

    PresetManager& pm = p.getPresetManager();

    addAndMakeVisible(presetsBar);

    img = ImageCache::getFromMemory(BinaryData::seamlesscubelines_png, BinaryData::seamlesscubelines_pngSize);

    addAndMakeVisible(pluginName);
    Font fontTitle = lnf.getTitleFont();
    fontTitle.setTypefaceName("Blackgrounds-Regular");
    fontTitle.setHeight(80.f);
    pluginName.setFont(fontTitle);
    pluginName.setText("ZxTools", NotificationType::dontSendNotification);
    pluginName.setJustificationType(Justification::centredLeft);
    pluginName.setColour(pluginName.textColourId, lnf.theme.compFace);

    addAndMakeVisible(title);
    title.setFont(fontTitle);
    title.setText("Distortion", NotificationType::dontSendNotification);
    title.setJustificationType(Justification::centredLeft);
    title.setColour(title.textColourId, lnf.theme.compHighlight);

    addAndMakeVisible(knobGroup);
    //knobGroup.setName("KNOBS");

    addAndMakeVisible(driveKnob);
    driveKnob.setName("DRIVE");
    driveKnob.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    driveKnob.setTextValueSuffix("%");

    addAndMakeVisible(rangeKnob);
    rangeKnob.setName("RANGE");
    rangeKnob.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    rangeKnob.setTextValueSuffix("Hz");

    addAndMakeVisible(blendKnob);
    blendKnob.setName("DRY/WET");
    blendKnob.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    blendKnob.setTextValueSuffix("%");

    addAndMakeVisible(gainKnob);
    gainKnob.setName("GAIN");
    gainKnob.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    gainKnob.setTextValueSuffix("dB");

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 280);
}

ZxDistortionAudioProcessorEditor::~ZxDistortionAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void ZxDistortionAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.setColour(lnf.theme.bgMain);
    g.fillAll();

    //Rectangle<float> r = Rectangle<float>(0.f, 0.f, (float)getWidth(), (float)getHeight());
    g.setTiledImageFill(img,0, 0, .1f);
    g.fillAll();
}

void ZxDistortionAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();

    auto presetBounds = bounds.removeFromBottom(30);
    presetsBar.setBounds(presetBounds);

    auto margins = 10;
    bounds.removeFromLeft(margins);
    bounds.removeFromRight(margins);
    bounds.removeFromBottom(margins);

    auto headerBounds = bounds.removeFromTop(100);
    pluginName.setBounds(headerBounds.removeFromLeft(headerBounds.getWidth() * 0.5f));
    title.setBounds(headerBounds);

    auto knobBounds = bounds.removeFromTop(150);
    knobGroup.setBounds(knobBounds);
    knobBounds.removeFromTop(14);
    knobBounds.removeFromBottom(10);

    driveKnob.setBounds(knobBounds.removeFromLeft(knobBounds.getWidth() * 0.25f));
    rangeKnob.setBounds(knobBounds.removeFromLeft(knobBounds.getWidth() * 0.33f));
    blendKnob.setBounds(knobBounds.removeFromLeft(knobBounds.getWidth() * 0.5f));
    gainKnob.setBounds(knobBounds);
}

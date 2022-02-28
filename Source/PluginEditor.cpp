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

    //prevPresetButton("prevPresetButton", 0.5f, Colours::white),
    //nextPresetButton("nextPresetButton", 0.f, Colours::white)
    presetsBar(p.getPresetManager(), lnf.theme)
{
    lnf.setTheme(Themes::InkPink);
    setLookAndFeel(&lnf);

    PresetManager& pm = p.getPresetManager();

    addAndMakeVisible(presetsBar);

    //addAndMakeVisible(savePresetButton);
    //savePresetButton.setButtonText("Save");
    //savePresetButton.onClick = [&] { 
    //    // needs to get user input
    //    pm.savePreset(presetsComboBox.getText()); 
    //    presetsComboBox.clear();
    //    StringArray allPresets = pm.getAllPresets();
    //    presetsComboBox.addItemList(allPresets, 1);
    //};
    //presetsComboBox.setEditableText(true);

    //loadPresetButton.setButtonText("Load");
    //loadPresetButton.onClick = [&] { 
    //    // needs to browse presets
    //    pm.loadPreset("Default");
    //    presetsComboBox.setText("Default");
    //};

    //addAndMakeVisible(deletePresetButton);
    //deletePresetButton.setButtonText("Delete");
    //deletePresetButton.onClick = [&] { 
    //    auto preset = pm.getCurrentPreset();
    //    pm.deletePreset(preset); 
    //    presetsComboBox.clear();
    //    StringArray allPresets = pm.getAllPresets();
    //    presetsComboBox.addItemList(allPresets, 1);
    //    presetsComboBox.setText("");
    //};

    //addAndMakeVisible(prevPresetButton);
    //prevPresetButton.onClick = [&] {
    //    pm.loadNextPreset();
    //    presetsComboBox.setText(pm.getCurrentPreset());
    //};

    //addAndMakeVisible(nextPresetButton);
    //nextPresetButton.onClick = [&] {
    //    pm.loadPreviousPreset();
    //    presetsComboBox.setText(pm.getCurrentPreset());
    //};

    //addAndMakeVisible(presetsComboBox);
    //StringArray allPresets = pm.getAllPresets();
    //presetsComboBox.addItemList(allPresets, 1);
    //presetsComboBox.onChange = [&]() {
    //    pm.loadPreset(presetsComboBox.getText());
    //};

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
    //g.drawImage(img, r);
    g.setTiledImageFill(img,0, 0, .1f);
    g.fillAll();

    // Draw presets bar
    //Rectangle<int> presetsBar = Rectangle<int>(0,getHeight() - 30, getWidth(), 30);
    //g.setColour(lnf.theme.bgGroup);
    //g.fillRect(presetsBar);
}

void ZxDistortionAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();

    auto presetBounds = bounds.removeFromBottom(30);
    presetsBar.setBounds(presetBounds);
    //auto presetLeftBounds = presetBounds.removeFromLeft(presetBounds.getWidth() * 0.25);
    //presetLeftBounds.removeFromTop(5);
    //presetLeftBounds.removeFromBottom(5);
    //savePresetButton.setBounds(presetLeftBounds.removeFromLeft(60));
    //loadPresetButton.setBounds(presetLeftBounds.removeFromLeft(60));

    //auto presetCentreBounds = presetBounds.removeFromLeft(presetBounds.getWidth() * 0.6666);
    //presetCentreBounds.removeFromTop(5);
    //presetCentreBounds.removeFromBottom(5);
    //prevPresetButton.setBounds(presetCentreBounds.removeFromLeft(20));
    //nextPresetButton.setBounds(presetCentreBounds.removeFromRight(20));
    //presetsComboBox.setBounds(presetCentreBounds);

    //auto presetRightBounds = presetBounds;
    //presetRightBounds.removeFromTop(5);
    //presetRightBounds.removeFromBottom(5);
    //deletePresetButton.setBounds(presetRightBounds.removeFromRight(80));

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

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GreyBoxDRCAudioProcessorEditor::GreyBoxDRCAudioProcessorEditor (GreyBoxDRCAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    // set slider and add listener
    compAmount.setSliderStyle(juce::Slider::LinearHorizontal);
    compAmount.setRange(0.0, 100.0, 0.0);
    compAmount.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    compAmount.setPopupDisplayEnabled(true, false, this);
    compAmount.setValue(0.0);
    addAndMakeVisible(&compAmount);

    // Display labels
    thresholdLabel.setText("T: 0 db", juce::dontSendNotification);
    thresholdLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(thresholdLabel);

    ratioLabel.setText("R: 1:1", juce::dontSendNotification);
    ratioLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(ratioLabel);

    kneeWidthLabel.setText("W: 0 db", juce::dontSendNotification);
    kneeWidthLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(kneeWidthLabel);

    // Poll processor for updated values at 30 Hz
    startTimerHz(30);

    compAmount.addListener(&audioProcessor);

}

GreyBoxDRCAudioProcessorEditor::~GreyBoxDRCAudioProcessorEditor()
{
    compAmount.addListener(&audioProcessor);
    stopTimer();
}

// update text boxes 30 times per second
void GreyBoxDRCAudioProcessorEditor::timerCallback()
{
    float T = audioProcessor.currentT.load();
    float R = audioProcessor.currentR.load();
    float W = audioProcessor.currentW.load();

    thresholdLabel.setText("Threshold: " + juce::String(T, 1) + " dB",
        juce::dontSendNotification);
    ratioLabel.setText("Ratio: " + juce::String(R, 1) + ":1",
        juce::dontSendNotification);
    kneeWidthLabel.setText("Knee Width: " + juce::String(W, 1) + " dB",
        juce::dontSendNotification);
}

//==============================================================================
void GreyBoxDRCAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("LA-2A Test", getLocalBounds(), juce::Justification::centred, 1);
}

void GreyBoxDRCAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto area = getLocalBounds().reduced(10);

    compAmount.setBounds(area.removeFromTop(20));
    compAmount.setBounds(area.removeFromTop(30));

    area.removeFromTop(10);

    thresholdLabel.setBounds(area.removeFromTop(25));
    ratioLabel.setBounds(area.removeFromTop(25));
    kneeWidthLabel.setBounds(area.removeFromTop(25));
}

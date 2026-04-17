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

    compAmount.setSliderStyle(juce::Slider::LinearBarVertical);
    compAmount.setRange(0.0, 100.0, 0.0);
    compAmount.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    compAmount.setPopupDisplayEnabled(true, false, this);
    compAmount.setValue(0.0);
    addAndMakeVisible(&compAmount);

    compAmount.addListener(&audioProcessor);

}

GreyBoxDRCAudioProcessorEditor::~GreyBoxDRCAudioProcessorEditor()
{
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
    compAmount.setBounds(100, 10, 20, getHeight() - 60);
}

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class GreyBoxDRCAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    GreyBoxDRCAudioProcessorEditor (GreyBoxDRCAudioProcessor&);
    ~GreyBoxDRCAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GreyBoxDRCAudioProcessor& audioProcessor;
    juce::Slider compAmount;

    juce::Label thresholdLabel;
    juce::Label ratioLabel;
    juce::Label kneeWidthLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GreyBoxDRCAudioProcessorEditor)
};

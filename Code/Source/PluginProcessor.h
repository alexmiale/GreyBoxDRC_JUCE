/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Compressor/CompModel.h"

//==============================================================================
/**
*/
class GreyBoxDRCAudioProcessor  : public juce::AudioProcessor, public juce::Slider::Listener
{
public:
    //==============================================================================
    GreyBoxDRCAudioProcessor();
    ~GreyBoxDRCAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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

    void sliderValueChanged(juce::Slider* slider) override;

    // Atomic so editor can safely read from the GUI thread
    // Also need to be made public
    std::atomic<float> currentT{ 0.0f };
    std::atomic<float> currentR{ 1.0f };
    std::atomic<float> currentW{ 0.0f };

private:
    //==============================================================================
    CompModel compressor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GreyBoxDRCAudioProcessor)

    std::string modelFilePath;
    float param = 0.0f;
};

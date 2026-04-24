/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GreyBoxDRCAudioProcessor::GreyBoxDRCAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

GreyBoxDRCAudioProcessor::~GreyBoxDRCAudioProcessor()
{
}

//==============================================================================
const juce::String GreyBoxDRCAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GreyBoxDRCAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GreyBoxDRCAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GreyBoxDRCAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GreyBoxDRCAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GreyBoxDRCAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GreyBoxDRCAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GreyBoxDRCAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GreyBoxDRCAudioProcessor::getProgramName (int index)
{
    return {};
}

void GreyBoxDRCAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GreyBoxDRCAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    modelFilePath = "Models/hcmlp_weights.json";

    std::cout << "Loading model from path: " << modelFilePath << std::endl;
    std::ifstream jsonStream(modelFilePath, std::ifstream::binary);
    if (!jsonStream.is_open()) {
        std::cerr << "ERROR: Failed to open model file: " << modelFilePath << std::endl;
        std::cerr << "Current working directory might be different during build process" << std::endl;
        // Don't crash - the plugin can still be instantiated without the model
        return;
    }

    // pass weights to CompModel
    compressor.loadModel(jsonStream);
    
    compressor.prepare(sampleRate);
    compressor.reset();

    // start at 0, update when slider is changed
    compressor.setThreshold(0.0f);       // dB
    compressor.setRatio(1.0f);
    compressor.setWidth(0.0f);              // soft-knee width in dB

    // values taken from Models/dsp_params.json 
    compressor.setTimeConstant(0.069f);     // ? in seconds
    compressor.setMakeUpGainDB(0.0f);       // dB
    
}

void GreyBoxDRCAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GreyBoxDRCAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

/** update the compressor when the slider has changed **/
void GreyBoxDRCAudioProcessor::sliderValueChanged(juce::Slider* slider) {
    param = slider->getValue(); // get slider value
    float normalized = (param / 50.0f) - 1.0f; // change from 0-100 to -1-1
    const float* outputs = compressor.forward(normalized); // pass to model

    // update params
    compressor.setAllCompFromModel(outputs);
    
    // update text boxes
    auto staticComp = compressor.getStaticComp();
    currentT.store(staticComp.getThreshold());
    currentR.store(staticComp.getRatio());
    currentW.store(staticComp.getWidth());
}

void GreyBoxDRCAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, numSamples);

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        compressor.processBlock(channelData, numSamples);

        // ..do something to the data...

    }
}

//==============================================================================
bool GreyBoxDRCAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GreyBoxDRCAudioProcessor::createEditor()
{
    return new GreyBoxDRCAudioProcessorEditor (*this);
}

//==============================================================================
void GreyBoxDRCAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GreyBoxDRCAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GreyBoxDRCAudioProcessor();
}

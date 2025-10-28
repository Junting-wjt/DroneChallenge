/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DroneChallengeAudioProcessor::DroneChallengeAudioProcessor()
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
}

DroneChallengeAudioProcessor::~DroneChallengeAudioProcessor()
{
}

//==============================================================================
const juce::String DroneChallengeAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DroneChallengeAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DroneChallengeAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DroneChallengeAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DroneChallengeAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DroneChallengeAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DroneChallengeAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DroneChallengeAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DroneChallengeAudioProcessor::getProgramName (int index)
{
    return {};
}

void DroneChallengeAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DroneChallengeAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
   
    // initialize the reverb
    Reverb::Parameters reverbParams;
    reverbParams.dryLevel = 0.5f;
    reverbParams.wetLevel = 0.5f;
    reverbParams.roomSize = 0.99f;
    reverb.setParameters(reverbParams);
    reverb.reset();

    sr = sampleRate;
    // initialize synthesis
    modulationSynth.prepareFilterSynth(sampleRate);
    modulationSynth.prepareModulationSynth(sampleRate);

    // initialize envolope sin wave
    envolopeSinOne.setSampleRate(sampleRate);
    envolopeSinOne.setFrequency(0.0083); // switch audio every one minute
    envolopeSinTwo.setSampleRate(sampleRate);
    envolopeSinTwo.setFrequency(0.0083);

    //initialize panning sin wave
    panningSin.setSampleRate(sampleRate);
    panningSin.setFrequency(0.5);
}

void DroneChallengeAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DroneChallengeAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void DroneChallengeAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    int numSamples = buffer.getNumSamples();
    float* leftChannel = buffer.getWritePointer(0); // left channel
    float* rightChannel = buffer.getWritePointer(1);//right channel

    for (int i = 0; i < numSamples; i++)
    {
        // use sin wave to automate the panning position
        float panningSinValue = panningSin.process();
        float leftGain = (panningSinValue * 0.5f) + 0.5f;  // 0 - 1
        float rightGain = 1.0f - leftGain;                             // 1 - 0

        // use two sin waves to switch the music
        float envolopeSinOneValue = envolopeSinOne.process();
        float envolopeSinTwoValue = envolopeSinTwo.process();
        if (envolopeSinOneValue < 0)
            envolopeSinOneValue *= 0;
        else
            envolopeSinTwoValue = 0;
        
        // output
        float msOutBeat = modulationSynth.BeatSynth(sr);
        float msOut = modulationSynth.filterSynthProcess()* envolopeSinOneValue + (msOutBeat+modulationSynth.modulationSynthProcess())* envolopeSinTwoValue*0.1;
        leftChannel[i] = msOut* leftGain;
        rightChannel[i] = msOut* rightGain;
    }
   reverb.processStereo(leftChannel, rightChannel, numSamples);
}

//==============================================================================
bool DroneChallengeAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DroneChallengeAudioProcessor::createEditor()
{
    return new DroneChallengeAudioProcessorEditor (*this);
}

//==============================================================================
void DroneChallengeAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DroneChallengeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DroneChallengeAudioProcessor();
}

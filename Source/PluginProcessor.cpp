/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginTestAudioProcessor::PluginTestAudioProcessor()
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

PluginTestAudioProcessor::~PluginTestAudioProcessor()
{
}

//==============================================================================
const juce::String PluginTestAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginTestAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginTestAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginTestAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginTestAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginTestAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginTestAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PluginTestAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PluginTestAudioProcessor::getProgramName (int index)
{
    return {};
}

void PluginTestAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PluginTestAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    squareOsc.setSampleRate(sampleRate);
    squareOsc.setFrequency(440.0f);
    sinOsc.setSampleRate(sampleRate);
    
    // lfo for square filter
    lfo.setSampleRate(sampleRate);
    lfo.setFrequency(0.05);
 
    // lfo for sine wave
    lfo2.setSampleRate(sampleRate);
    lfo2.setFrequency(1.0);
    
    sr = sampleRate;
    //filter for harmonic
    filterout.prepared(sampleRate);
    
    //noise prepare
    Noiseout.setSampleRate(sampleRate);
   
    // reverb
    Reverb::Parameters reverbParams;
    reverbParams.dryLevel = 0.3f;
    reverbParams.wetLevel = 0.7f;
    reverbParams.roomSize = 0.55f;
    reverb.setParameters(reverbParams);
    reverb.reset();
    
    // change base frequency of harmonic
    float lfoValue = 110*lfo.process()+120;
    harmonicSeries.prepare(sampleRate,lfoValue);
   
    //PM
    phaseModulation.PMprepare(sampleRate);
    
    //panning
    panGain.setSampleRate(sampleRate);
    panGain.setFrequency(0.9);
    
    //envelope
    timeControl.setSampleRate(sampleRate);
    timeControl.setFrequency(0.09f);
    timeControl.setWidth(0.5f);
    squareOscEvn.setSampleRate(sampleRate);
    squareOscEvn.setFrequency(0.09f);
    squareOscEvn.setPulseWidth(0.5f);
    
    sinEnv1.setSampleRate(sampleRate);
    sinEnv1.setFrequency(0.5f);
    sinEnv2.setSampleRate(sampleRate);
    sinEnv2.setFrequency(0.9f);
    sinEnv3.setSampleRate(sampleRate);
    sinEnv3.setFrequency(0.1f);
    sinEnv4.setSampleRate(sampleRate);
    sinEnv4.setFrequency(0.05f);
    sinEnv5.setSampleRate(sampleRate);
    sinEnv5.setFrequency(0.003f);
    sinEnv6.setSampleRate(sampleRate);
    sinEnv6.setFrequency(0.006f);
    TriEnv1.setSampleRate(sampleRate);
    TriEnv1.setFrequency(0.05f);
    TriEnv2.setSampleRate(sampleRate);
    TriEnv2.setFrequency(0.004f);
    TriEnv3.setSampleRate(sampleRate);
    TriEnv3.setFrequency(0.002f);
}

void PluginTestAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PluginTestAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void PluginTestAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; i++)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    int numSamples = buffer.getNumSamples();
    float* leftChannel = buffer.getWritePointer(0);//left channel
    float* rightChannel = buffer.getWritePointer(1);//right channel
   
    //DSP LOOP!
    for (int i=0; i<numSamples; i++)
    {
        //frequencyChange sine
        float frequencyValueChange = (lfo2.process()+2)*500;
        sinOsc.setFrequency(frequencyValueChange);
        float sinWave = sinOsc.process();
        
        //filter square
        float cutoff = lfo.process() * 330.0f+440.0f;
        filter. setCoefficients ( juce::IIRCoefficients::makeLowPass(sr, cutoff, 0.8) );
        float rawSquareWave = squareOsc.process () * 0.9f;
        float filterSquare = filter.processSingleSampleRaw (rawSquareWave);
        
        //filter harmonic
        float HarmonicOutput = filterout.processHarmonic();
        
        //filter noise
        float NoiseOutput2 = Noiseout.process();
        
        //modulation
        float PM = phaseModulation.PMprocess();
        
        //panning
        float rightPanning = (panGain.process()+1)/2;
        float leftPanning = 1-rightPanning;
        
        //envelope
        float gainVol = timeControl.process();
        //float gainVol = squareOscEvn.process()+0.5;
        
        float gain1 = sinEnv1.process();
        float gain2 = sinEnv2.process();
        float gain3 = sinEnv3.process();
        float gain4 = sinEnv4.process();
        float gain5 = sinEnv5.process();
        float gain6 = sinEnv6.process();
        float gain7 = TriEnv1.process();
        float gain8 = TriEnv2.process();
        float gain9 = TriEnv3.process();
        
        //output
        float sinWaveOut = sinWave * gainVol;
        float Mix = HarmonicOutput * gain6 * 0.5 + PM * gain9 * 0.5 + NoiseOutput2 * gain5 * 0.1 + sinWaveOut * gain8 * 0.3 + filterSquare * gain9 * 0.4;
        //float Mix = filterSquare * gain6 ;
        //rightChannel[i] = (PM*0.09 + leftMix)*rightPanning+filterSquare;
        //leftChannel[i] = (PM*0.09 + rightMix)*leftPanning+filterSquare;
        
        rightChannel[i] = Mix*rightPanning;
        leftChannel[i] = Mix*leftPanning;
    }
    reverb.processStereo(leftChannel, rightChannel, numSamples);
}


//==============================================================================
bool PluginTestAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginTestAudioProcessor::createEditor()
{
    return new PluginTestAudioProcessorEditor (*this);
}

//==============================================================================
void PluginTestAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PluginTestAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginTestAudioProcessor();
}

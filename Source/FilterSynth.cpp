/*
  ==============================================================================

    FilterSynth.cpp
    Created: 2 Mar 2024 8:18:09pm
    Author:  Junting

  ==============================================================================
*/

#include "FilterSynth.h"

FilterSynth::FilterSynth()
{

}
FilterSynth::~FilterSynth()
{

}
void FilterSynth::setSampleRate(float sr)
{
    sampleRate = sr;

    // square wave prepare
    squareOsc.setSampleRate(sr);
    squareOsc.setFrequency(440.0f);

    // sin wave prepare
    sinOsc.setSampleRate(sr);
    sinOsc.setFrequency(220.0f);

    // triangular wave prepare
    triOsc.setSampleRate(sr);
    triOsc.setFrequency(660.0f);

    // lfo prepare
    lfo.setSampleRate(sr);
    lfo.setFrequency(0.05);
    Qlfo.setSampleRate(sr);
    Qlfo.setFrequency(0.05);
}

float FilterSynth::processSquare()
{
    float cutoff = lfo.process() * 660.0f + 770.0f;                   // value change from 110-1430
    float Q = Qlfo.process() * 10 + 16;                                     // value change from 6-26
    filter.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, cutoff, Q));
    float rawSquareWave = squareOsc.process()*0.05;       // reduce amplitude
    float sample = filter.processSingleSampleRaw(rawSquareWave);
    return sample;
}

float FilterSynth::processSin()
{
    float cutoff = lfo.process() * 110.0f + 220.0f;                  // value change from 110-330
    float Q = Qlfo.process() * 10 + 16;
    filter.setCoefficients(juce::IIRCoefficients::makeHighPass(sampleRate, cutoff,Q));
    filter.reset();
    float rawSinWave = sinOsc.process()*0.5;
    float sample = filter.processSingleSampleRaw(rawSinWave);
    return sample;
}

float FilterSynth::processTri()
{
    float cutoff = lfo.process() * 990.0f + 1000.0f;                // value change from 10-1990
    float Q = Qlfo.process() * 10 + 16;
    filter.setCoefficients(juce::IIRCoefficients::makeBandPass(sampleRate, cutoff, Q));
    filter.reset();
    float rawTriWave = triOsc.process();
    float sample = filter.processSingleSampleRaw(rawTriWave);
    return sample;
}

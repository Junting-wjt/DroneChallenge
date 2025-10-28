/*
  ==============================================================================

    Filter.h
    Created: 4 Mar 2024 10:25:58pm
    Author:   Yuqiao

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Oscillator.h"
#include"HarmonicSeries.h"

using namespace juce;

class Filter
{
public:
    void prepared(float sr)
    {
        sampleRate = sr;
        lfo.setSampleRate(sr);
        lfo.setFrequency(0.05f);
        sinOsc.setSampleRate(sr);
        sinOsc.setFrequency(60 * lfo.process() + 320);
        harmonicSeries.prepare(sr,220.0f);
        
    }
    float processHarmonic()
    {
        float cutoff = 300 * lfo.process()+320;
        float Q = lfo.process() + 1.1;
        filter.setCoefficients(IIRCoefficients::makeLowPass(sampleRate,cutoff,Q));
        float rawHarmonicWave = harmonicSeries.process();
        float sample = filter.processSingleSampleRaw(rawHarmonicWave);
        return sample;
    }
    
private:
    IIRFilter filter;
    SinOsc lfo;
    float sampleRate;
    SinOsc sinOsc;
    HarmonicSeries harmonicSeries;
};

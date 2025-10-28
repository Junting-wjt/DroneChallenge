/*
  ==============================================================================

    PhaseModulation.h
    Created: 8 Mar 2024 3:55:15pm
    Author: Yuqiao

  ==============================================================================
*/

#pragma once
#include"Oscillator.h"
#include<cmath>
class PhaseModulation
{
public:
    void PMprepare(float sr)
    {
        sampleRate = sr;
        carrier.setSampleRate(sr);
        Modulation.setSampleRate(sr);
        Modulation.setFrequency(55.0f);
        lfoDepth.setSampleRate(sr);
        lfoDepth.setFrequency(0.03f);
    }
    float PMprocess()
    {
        float modulateDepth = lfoDepth.process();
        float lfoOut = modulateDepth*Modulation.process();
        carrier.setFrequency(220+(sampleRate*lfoOut/(2*M_PI)));
        float PMout = carrier.process();
        return PMout;
    }
private:
    SinOsc carrier;
    SinOsc Modulation;
    SinOsc lfoDepth;
    float sampleRate;
};

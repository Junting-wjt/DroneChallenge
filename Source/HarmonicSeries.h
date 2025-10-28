/*
  ==============================================================================

    HarmonicSeries.h
    Created: 8 Mar 2024 1:25:59pm
    Author:  Yuqiao

  ==============================================================================
*/

#pragma once
#include "Oscillator.h"

class HarmonicSeries
{
public:
    void prepare(float sr, float freq)
    {
        for (int i=0; i<4; i++)
        {
            oscs.push_back(SinOsc());
            oscs[i].setSampleRate(sr);
            oscs[i].setFrequency(freq * (i+1));
        }
    }
    float process()
    {
        float outputVal= 0.0f;
        for (int i=0; i<4; i++)
        {
            float amplitude = 1/pow(i+1,2);
            float sampleValue = oscs[i].process();
            sampleValue =amplitude*sampleValue ;
            outputVal += sampleValue;
        }
        outputVal /= 4.0;
        return outputVal;
    }
private:
    float freq;
    std::vector<Phasor> oscs;
    SinOsc sinOsc;
};

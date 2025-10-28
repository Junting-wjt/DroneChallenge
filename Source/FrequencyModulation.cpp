/*
  ==============================================================================

    ModulationSynth.cpp
    Created: 2 Mar 2024 8:54:06pm
    Author:  Junting

  ==============================================================================
*/

#include "FrequencyModulation.h"

FrequencyModulation::FrequencyModulation()
{

}

FrequencyModulation::~FrequencyModulation()
{

}

void FrequencyModulation::prepareTOFM(float sr)
{
    sampleRate = sr;

    // prepare for FM sin
    carrierSin.setSampleRate(sr);
    modulateSin.setSampleRate(sr);
    modulateSin.setFrequency(55.0f);

    // prepare for lfo to change modulation depth
    lfoSin.setSampleRate(sr);
    lfoSin.setFrequency(0.05f);
}

float FrequencyModulation::FMprocessSin(float carrierSinFrequency, float modulateDepth)
{
    float modulateIndex = modulateDepth *lfoSin.process();
    float modulateOut = modulateIndex*modulateSin.process();
    carrierSin.setFrequency(carrierSinFrequency + (sampleRate * modulateOut/ (2 * M_PI * 55.0f)));
    float FMout = carrierSin.process();
    return FMout;
}

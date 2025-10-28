/*
  ==============================================================================

    ModulationSynth.h
    Created: 2 Mar 2024 8:53:40pm
    Author:  Junting

  ==============================================================================
*/

#pragma once
#include "Oscillators.h"

class FrequencyModulation
{
public:
    FrequencyModulation::FrequencyModulation();
    FrequencyModulation::~FrequencyModulation();
    void FrequencyModulation::prepareTOFM(float sampleRate);
    float FrequencyModulation::FMprocessSin(float carrierSinFrequency, float modulateDepth);
 
private:
    //=== Carrier oscillator ===
    SinOsc carrierSin;
     //=== Modulating oscillator ===
    SinOsc modulateSin;
    //=== LFO for modulation index ===
    SinOsc lfoSin;

    float sampleRate;
};
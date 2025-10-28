/*
  ==============================================================================

    FilterSynth.h
    Created: 1 Mar 2024 2:12:07am
    Author:  Junting

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include"Oscillators.h"
using namespace juce;

/*
* Create three different types of filters to 
* process three different waveforms with frequencies close to the 
* harmonic series, and use LFO to change the cutoff frequency and
* resonance of the filters
*/
class FilterSynth
{
public:
    FilterSynth::FilterSynth();
    FilterSynth::~FilterSynth();
    void FilterSynth::setSampleRate(float sr);
    float FilterSynth::processSquare();
    float FilterSynth::processSin();
    float FilterSynth::processTri();
    
private:
    float sampleRate;
    //====== Filtered Square wave ======
    SquareOsc squareOsc;
    //====== Filtered Sin wave ======
    SinOsc sinOsc;
    //====== Filtered Tri wave ======
    TriOsc triOsc;
    //====== LFO for cutoff and resonance ======
    SinOsc lfo;             // LFO for cutoff
    SinOsc Qlfo;          // LFO for resonance
    IIRFilter filter;
};
/*
  ==============================================================================

    ModulationSynth.h
    Created: 3 Mar 2024 3:37:22pm
    Author:  Junting

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include"FrequencyModulation.h"
#include"FilterSynth.h"

/*
* Package the filter and frequency modulation
* add the Beat frequencies effect
*/
class ModulationSynth
{
public:
    ModulationSynth::ModulationSynth();
    ModulationSynth::~ModulationSynth();
    void ModulationSynth::prepareModulationSynth(float sampleRate);
    float ModulationSynth::modulationSynthProcess();
    void ModulationSynth::prepareFilterSynth(float sampleRate);
    float ModulationSynth::filterSynthProcess();
    float  ModulationSynth::BeatSynth(float sampleRate);

private:
    float sr;
    std::vector<FilterSynth> filterSynth;
    FrequencyModulation frequencyModulation;

    // for Beat frequencies
    SinOsc BeatSinOne;
    SinOsc BeatSinTwo;

    // use sin wave to achieve alternating amplitude changes
    SinOsc Amchange;
};
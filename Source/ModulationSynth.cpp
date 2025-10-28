/*
  ==============================================================================

    ModulationSynth.cpp
    Created: 3 Mar 2024 3:37:54pm
    Author:  Junting

  ==============================================================================
*/

#include "ModulationSynth.h"

ModulationSynth::ModulationSynth()
{

}

ModulationSynth::~ModulationSynth()
{

}

void ModulationSynth::prepareFilterSynth(float sampleRate)
{
    sr = sampleRate;
    //=== storing instances for vectors ===
    filterSynth.push_back(FilterSynth());
    filterSynth[0].setSampleRate(sampleRate);
    filterSynth.push_back(FilterSynth());
    filterSynth[1].setSampleRate(sampleRate);
    filterSynth.push_back(FilterSynth());
    filterSynth[2].setSampleRate(sampleRate);
    //=== sin wave for changing amplitude prepare ===
    Amchange.setSampleRate(sampleRate);
}

void ModulationSynth::prepareModulationSynth(float sampleRate)
{
    sr = sampleRate;
    frequencyModulation.prepareTOFM(sampleRate);
}

/*
* Use vector to call three different waveforms 
* and storing them in an array to add them together
*/
float ModulationSynth::filterSynthProcess()
{
    float filterSynthOut[3];

    filterSynthOut[0] = filterSynth[0].FilterSynth::processSin();
    filterSynthOut[1] = filterSynth[1].FilterSynth::processSquare();
    filterSynthOut[2] = filterSynth[2].FilterSynth::processTri();

    float MSout = filterSynthOut[0]*0.05 + filterSynthOut[1]*0.1 + filterSynthOut[2]*0.5;
 
    return MSout;
}

/*
* obtain the effect of beat frequencies
* vary its amplitude between 0.1 to 1.1
*/
float  ModulationSynth::BeatSynth(float sampleRate)
{
    sr = sampleRate;
    Amchange.setFrequency(0.1);
    float AmchangeValue = Amchange.process();
    float Gain = (AmchangeValue * 0.5f) + 0.7f;  // 0.2 - 1.2
    float BeatGain = 1.3 - Gain;                              // 1.1 - 0.1
    BeatSinOne.setSampleRate(sampleRate);
    BeatSinOne.setFrequency(440);
    BeatSinTwo.setSampleRate(sampleRate);
    BeatSinTwo.setFrequency(441);
    float BeatSinOneOut = BeatSinOne.process();
    float BeatSinTwoOut = BeatSinTwo.process();
    float BeatOut = (BeatSinTwoOut + BeatSinOneOut)*BeatGain*0.1;
    return BeatOut;
}

/*
* obtain the effect of frequency modulation
* of a sinwave, vary its amplitude between 0.2 to 1.2
* alternating amplitude changes with beat frequencies
*/
float ModulationSynth::modulationSynthProcess()
{
    Amchange.setFrequency(0.1);
    float AmchangeValue = Amchange.process();
    float Gain = (AmchangeValue * 0.5f) + 0.7f;  // 0.2 - 1.2
    float MSsynthOut = frequencyModulation.FMprocessSin(220, 5.0f); 
    float MSout = MSsynthOut*Gain*0.5;
    return MSout;
}

/*
  ==============================================================================

    Time2.h
    Created: 5 Mar 2024 5:14:51pm
    Author:   Yuqiao

  ==============================================================================
*/

#pragma once
#include "Oscillator.h"

class  Noise{
public:
    void setSampleRate(float _sampleRate)
    {
        sampleRate = _sampleRate;
        osc1.setSampleRate(sampleRate);
        numOfDuration = duration * sampleRate;
        randNum = randomParameter.nextFloat();
    }
    
    void setFrequency(float _freq)
    {
        osc1.setFrequency(_freq);
    }
    
    float process()
    {
        float oscSample = noise.nextFloat();
        
        float filterCutoffVal = randNum * 700 + 350;
        
        if(sampleProcessed >= numOfDuration)
        {
            sampleProcessed = 0;
            randNum = randomParameter.nextFloat();
            std::cout << filterCutoffVal << std::endl;
        }
        
        auto filterCoefficients = juce::IIRCoefficients::makeLowPass(sampleRate, filterCutoffVal, 10.0);
        filter.setCoefficients(filterCoefficients);
        
        float filteredOsc = filter.processSingleSampleRaw(oscSample);
        
        sampleProcessed++;
        
        return filteredOsc;
    }
    
private:
    SinOsc osc1;
    juce::IIRFilter filter;
    juce::Random randomParameter;
    juce::Random noise;
    float sampleRate;
    int sampleProcessed;
    int numOfDuration;
    float duration = 0.2f;
    float randNum;
};

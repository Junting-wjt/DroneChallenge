//
//  Oscillator.h
// 


#ifndef Oscillator_h
#define Oscillator_h
#define _USE_MATH_DEFINES
#include <cmath>

/**
Oscillator class with three wave shapes (processSine, processSquare, processTriangle)
 */
class Phasor
{
public:
    
    // Our parent oscillator class does the key things required for most oscillators:
    // -- handles phase
    // -- handles setters and getters for frequency and samplerate
    
    /**
     update the phase and output as a sinusoidal signal
     */
    float process()
    {
        phase += phaseDelta;
        
        if (phase > 1.0f)
            phase -= 1.0f;
        
        return output(phase);
    }
    
    /**
     update the phase and output as a square wave signal
     */
    virtual float output(float p)
    {
        
        return p;
    }
    
    /**
     update the phase and output as a triangular signal
     */
   
    void setSampleRate(float SR)
    {
        sampleRate = SR;
    }
    
    /**
     set the oscillator frequency - MAKE SURE YOU HAVE SET THE SAMPLE RATE FIRST
     
     @param freq oscillator frequency in Hz
     */
    void setFrequency(float freq)
    {
        frequency = freq;
        phaseDelta = frequency / sampleRate;
    }
    
private:
    float frequency;
    float sampleRate;
    float phase = 0.0f;
    float phaseDelta;
};
//==========================================

class TriOsc : public Phasor
{
    float output(float p) override
    {
        return fabsf(p - 0.5f) - 0.5f;
    }
};

class SawOsc : public Phasor
{
    float output(float p) override
    {
        return fabsf(p - 0.5f) - 0.5f;
    }
};

class SinOsc : public Phasor
{
    float output(float p) override
    {
        return std::sin(p * 2.0 * 3.14159);
    }
};

class SquareOsc : public Phasor
{
public:
    float output(float p) override
    {
        float outVal = 0.5;
        if(p > pulseWidth)
            outVal = -0.5;
        return outVal;
    }
    void setPulseWidth(float pw)
    {
        pulseWidth = pw;
    }
private:
    float pulseWidth = 0.5f;
};

class TimeControl: public Phasor
{
public:
    void setWidth(float zeroWidth)
    {
        width = zeroWidth;
    }
    void setFrequency(float freq)
    {
        frequency = freq;
    }
    float output(float p) override
    {
        float adjustPhase = std::pow(p*2.0f*M_PI,width);
        return (std::sin(adjustPhase)+1.0f)/2.0f;
    }
private:
    float width;
    float frequency;
};

#endif /* Oscillators_h */

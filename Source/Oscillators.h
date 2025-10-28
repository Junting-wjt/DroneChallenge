#pragma once
#define _USE_MATH_DEFINES
#ifndef Oscillators_h
#define Oscillators_h

#include <cmath>

//parent phasor class
class  Phasor
{

public:
	 float process()
	 {
		 phase += phaseDelta;

		 if (phase > 1.0f)
			 phase -= 1.0f;

		 return output(phase);
	 }
	 virtual float output(float p)
	 {
		 return p;
	 }
	 void setSampleRate(float SR)
	 {
		 sampleRate = SR;
	 }
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

//==============================

// CHID Class
class TriOsc : public Phasor
{

	float output(float p) override
	{
		return fabsf(p - 0.5f) - 0.5f;
	}

};


// CHID Class
class SinOsc : public Phasor
{

	float output(float p) override
	{
		return std::sin(p*2.0*3.14159);
	}

};

//CHILD Class
class SquareOsc : public Phasor
{
public:
	float output(float p)override
	{
		float outVal = 0.5;
		if (p > pulseWidth)
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

#endif // Oscillators_h
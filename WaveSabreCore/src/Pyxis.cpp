/**
Pyxis, an elaborate reverb.

Based on the Clouds texture synthesizer,
https://pichenettes.github.io/mutable-instruments-documentation/modules/clouds/

Copyright 2014 Emilie Gillet.
Author: Emilie Gillet (emilie.o.gillet@gmail.com)

Copyright for portions of project WaveSabre (forked by Inque) are held by WabeSabre Team 2012-2019 as part of project WaveSabre.

All other copyright for project WaveSabre (forked by Inque) are held by Inque, 2020.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

See http://creativecommons.org/licenses/MIT/ for more information.
**/

#include <WaveSabreCore/Pyxis.h>
#include <WaveSabreCore/Helpers.h>

#include <math.h>



namespace WaveSabreCore
{
	Pyxis::Pyxis()
		: Device((int)ParamIndices::NumParams)
	{
		buffer = new float[16384];
		for (int i = 0; i < 16384; ++i)
			buffer[i] = 0;

		bufferWritePtr = 0;
		lfo[0].Init(16.0f / 32000.0f);
		lfo[1].Init(9.6f / 32000.0f);

		amount = 0.5f;
		inputGain = 0.2f;
		reverbTime = 0.5f;
		lp = 0.7f;
		diffusion = 0.625f;

		lpDecay1 = 0.0f;
		lpDecay2 = 0.0f;
	}

	Pyxis::~Pyxis()
	{
		delete[] buffer;
	}

	void Pyxis::Run(double songPosition, float **inputs, float **outputs, int numSamples)
	{
		// This is the Griesinger topology described in the Dattorro paper
		// (4 AP diffusers on the input, then a loop of 2x 2AP+1Delay).
		// Modulation is applied in the loop of the first diffuser AP for additional
		// smearing; and to the two long delays for a slow shimmer/chorus effect.


		float lpState1 = lpDecay1;
		float lpState2 = lpDecay2;

		// ap1:		base =     0		length = 113
		// ap2:		base =   113		length = 162
		// ap3:		base =   275		length = 241	
		// ap4:		base =   516		length = 399
		// dap1a:	base =   915		length = 1653
		// dap1b:	base =  2568		length = 2038
		// del1:	base =  4606		length = 3411
		// dap2a:	base =  8017		length = 1913
		// dap2b:	base =  9930		length = 1663
		// del2:	base = 11593		length = 4782
		// end: 16375


		for (int i = 0; i < numSamples; i++)
		{			
			float inputL = inputs[0][i];
			float inputR = inputs[1][i];


			float lfoValue[2];

			// Start
			--bufferWritePtr;
			if (bufferWritePtr < 0)
				bufferWritePtr += 16384;

			// Init LFOs
			if ((bufferWritePtr & 31) == 0) 
			{
				lfoValue[0] = lfo[0].Next();
				lfoValue[1] = lfo[1].Next();
			}
			else 
			{
				lfoValue[0] = lfo[0].Value();
				lfoValue[1] = lfo[1].Value();
			}

			// --------------------------------------------
			// Smear AP1 inside the loop.
			// --------------------------------------------
			
			float offset = 10.0f + 60.0f * lfoValue[0];
			int offset_integral = static_cast<int>(offset);
			float offset_fractional = offset - static_cast<float>(offset_integral);

			float a = buffer[(bufferWritePtr + offset_integral) & 16383];
			float b = buffer[(bufferWritePtr + offset_integral + 1) & 16383];
			float previous_read = a + (b - a) * offset_fractional;

			buffer[(bufferWritePtr + 100) & 16383] = previous_read;

			float accumulator = (inputL + inputR) * inputGain;

			// --------------------------------------------
			// Diffuse through 4 allpasses.
			// --------------------------------------------
				
			previous_read = buffer[(bufferWritePtr + 112) & 16383];
			accumulator += previous_read * diffusion;

			buffer[(bufferWritePtr) & 16383] = accumulator;
			accumulator *= -diffusion;
			accumulator += previous_read;

			previous_read = buffer[(bufferWritePtr + 274) & 16383];
			accumulator += previous_read * diffusion;

			buffer[(bufferWritePtr + 113) & 16383] = accumulator;
			accumulator *= -diffusion;
			accumulator += previous_read;

			previous_read = buffer[(bufferWritePtr + 515) & 16383];
			accumulator += previous_read * diffusion;

			buffer[(bufferWritePtr + 275) & 16383] = accumulator;
			accumulator *= -diffusion;
			accumulator += previous_read;

			previous_read = buffer[(bufferWritePtr + 914) & 16383];
			accumulator += previous_read * diffusion;

			buffer[(bufferWritePtr + 516) & 16383] = accumulator;
			accumulator *= -diffusion;
			accumulator += previous_read;

			float allpassOutput = accumulator;

			// --------------------------------------------
			// Main reverb loop.
			// --------------------------------------------
				
			// Left output

			offset = 4680.0f + 100.0f * lfoValue[1];
			offset_integral = static_cast<int>(offset);
			offset_fractional = offset - static_cast<float>(offset_integral);

			a = buffer[(bufferWritePtr + offset_integral + 11593) & 16383];
			b = buffer[(bufferWritePtr + offset_integral + 11594) & 16383];
			previous_read = a + (b - a) * offset_fractional;
			accumulator += previous_read * reverbTime;

			lpState1 += lp * (accumulator - lpState1);
			accumulator = lpState1;

			previous_read = buffer[(bufferWritePtr + 2567) & 16383];
			accumulator += previous_read * -diffusion;

			buffer[(bufferWritePtr + 915) & 16383] = accumulator;
			accumulator *= diffusion;
			accumulator += previous_read;

			previous_read = buffer[(bufferWritePtr + 4605) & 16383];
			accumulator += previous_read * diffusion;

			buffer[(bufferWritePtr + 2568) & 16383] = accumulator;
			accumulator *= -diffusion;
			accumulator += previous_read;

			buffer[(bufferWritePtr + 4606) & 16383] = accumulator;
			accumulator *= 2.0f;

			outputs[0][i] = inputL + (accumulator - inputL) * amount;

			// Right output

			accumulator = allpassOutput;

			previous_read = buffer[(bufferWritePtr + 8016) & 16383];
			accumulator += previous_read * reverbTime;

			lpState2 += lp * (accumulator - lpState2);
			accumulator = lpState2;

			previous_read = buffer[(bufferWritePtr + 9929) & 16383];
			accumulator += previous_read * diffusion;

			buffer[(bufferWritePtr + 8017) & 16383] = accumulator;
			accumulator *= -diffusion;
			accumulator += previous_read;

			previous_read = buffer[(bufferWritePtr + 11592) & 16383];
			accumulator += previous_read * -diffusion;

			buffer[(bufferWritePtr + 9930) & 16383] = accumulator;
			accumulator *= diffusion;
			accumulator += previous_read;

			buffer[(bufferWritePtr + 11593) & 16383] = accumulator;
			accumulator *= 2.0f;

			outputs[1][i] = inputR + (accumulator - inputR) * amount;
		}

		lpDecay1 = lpState1;
		lpDecay2 = lpState2;
	}

	void Pyxis::SetParam(int index, float value)
	{
		switch ((ParamIndices)index)
		{
		case ParamIndices::Amount:		amount = value; break;
		case ParamIndices::InputGain:	inputGain = value; break;
		case ParamIndices::Time:		reverbTime = value; break;
		case ParamIndices::Diffusion:	diffusion = value; break;
		case ParamIndices::LP:			lp = value; break;
		}
	}

	float Pyxis::GetParam(int index) const
	{
		switch ((ParamIndices)index)
		{
		default:
		case ParamIndices::Amount:		return amount;
		case ParamIndices::InputGain:	return inputGain;
		case ParamIndices::Time:		return reverbTime;
		case ParamIndices::Diffusion:	return diffusion;
		case ParamIndices::LP:			return lp;
		}

		return 0.0f;
	}


	void Pyxis::LFO::Init(float frequency)
	{
		float sign = 16.0f;
		frequency -= 0.25f;

		if (frequency < 0.0f) 
			frequency = -frequency;
		else if (frequency > 0.5f)
			frequency -= 0.5f;
		else
			sign = -16.0f;

		iirCoefficient = sign * frequency * (1.0f - 2.0f * frequency);
		initialAmplitude = iirCoefficient * 0.25f;

		Start();
	}

	void Pyxis::LFO::Start()
	{
		y1 = initialAmplitude;
		y0 = 0.5f;
	}

	float Pyxis::LFO::Value() const
	{
		return y1 + 0.5f;
	}

	float Pyxis::LFO::Next()
	{
		float temp = y0;
		y0 = iirCoefficient * y0 - y1;
		y1 = temp;

		return temp + 0.5f;
	}
}

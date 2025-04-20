/**
Perseus, just a resonator.

Based on the Rings eurorack module,
https://pichenettes.github.io/mutable-instruments-documentation/modules/rings/

Copyright 2015 Emilie Gillet.

Author: Emilie Gillet (emilie.o.gillet@gmail.com)

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

#ifndef __WAVESABRECORE_PERSEUS_H__
#define __WAVESABRECORE_PERSEUS_H__

#include "Device.h"
#include "DelayBuffer.h"
#include "StateVariableFilter.h"

namespace WaveSabreCore
{
	class Perseus : public Device
	{
	public:
		enum class ParamIndices
		{
			Amount,
			InputGain,
			Time,
			Diffusion,
			LP,

			NumParams,
		};

		Perseus();
		virtual ~Perseus();

		virtual void Run(double songPosition, float **inputs, float **outputs, int numSamples);

		virtual void SetParam(int index, float value);
		virtual float GetParam(int index) const;

	private:
		// Parameters
		float amount;
		float inputGain;
		float reverbTime;
		float diffusion;
		float lp;

        class LFO 
        {
        public:
			void Init(float frequency);
			void Start();
			float Value() const;
			float Next();

        private:
            float y1;
            float y0;
            float iirCoefficient;
            float initialAmplitude;
        };

		// State
		float* buffer;
		LFO lfo[2];
		int bufferWritePtr;
		float lpDecay1;
		float lpDecay2;
	};
}

#endif

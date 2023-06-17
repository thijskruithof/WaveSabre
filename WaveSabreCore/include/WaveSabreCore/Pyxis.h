#ifndef __WAVESABRECORE_PYXIS_H__
#define __WAVESABRECORE_PYXIS_H__

#include "Device.h"
#include "DelayBuffer.h"
#include "StateVariableFilter.h"

namespace WaveSabreCore
{
	/**
	Pyxis, an elaborate reverb.
	Based on the Clouds texture synthesizer,
	https://pichenettes.github.io/mutable-instruments-documentation/modules/clouds/
	**/
	class Pyxis : public Device
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

		Pyxis();
		virtual ~Pyxis();

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

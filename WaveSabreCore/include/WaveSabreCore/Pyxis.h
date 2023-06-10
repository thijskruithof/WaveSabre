#ifndef __WAVESABRECORE_PYXIS_H__
#define __WAVESABRECORE_PYXIS_H__

#include "Device.h"
#include "DelayBuffer.h"
#include "StateVariableFilter.h"

#include "Pyxis/fx_engine.h"


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
		typedef FxEngine<16384, FORMAT_32_BIT> E;
		E engine_;
		char* buffer;

		float amount_;
		float inputGain_;
		float reverbTime_;
		float diffusion_;
		float lp_;

		float lp_decay_1_;
		float lp_decay_2_;
	};
}

#endif

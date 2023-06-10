#include <WaveSabreCore/Pyxis.h>
#include <WaveSabreCore/Helpers.h>

#include <math.h>

__int32 FxEngineClip16(__int32 x) {
	if (x < -32768) {
		return -32768;
	}
	else if (x > 32767) {
		return 32767;
	}
	else {
		return x;
	}
}

unsigned __int16 FxEngineClipU16(__int32 x) {
	if (x < 0) {
		return 0;
	}
	else if (x > 65535) {
		return 65535;
	}
	else {
		return x;
	}
}


namespace WaveSabreCore
{
	Pyxis::Pyxis()
		: Device((int)ParamIndices::NumParams)
	{
		buffer = new char[16384 * 4];
		engine_.Init((float*)buffer);
		engine_.SetLFOFrequency(LFO_1, 0.5f / 32000.0f);
		engine_.SetLFOFrequency(LFO_2, 0.3f / 32000.0f);


		amount_ = 0.5f;
		inputGain_ = 0.2f;
		reverbTime_ = 0.5f;
		lp_ = 0.7f;
		diffusion_ = 0.625f;

		lp_decay_1_ = 0.0f;
		lp_decay_2_ = 0.0f;
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
		typedef E::Reserve<113,
			E::Reserve<162,
			E::Reserve<241,
			E::Reserve<399,
			E::Reserve<1653,
			E::Reserve<2038,
			E::Reserve<3411,
			E::Reserve<1913,
			E::Reserve<1663,
			E::Reserve<4782> > > > > > > > > > Memory;
		E::DelayLine<Memory, 0> ap1;
		E::DelayLine<Memory, 1> ap2;
		E::DelayLine<Memory, 2> ap3;
		E::DelayLine<Memory, 3> ap4;
		E::DelayLine<Memory, 4> dap1a;
		E::DelayLine<Memory, 5> dap1b;
		E::DelayLine<Memory, 6> del1;
		E::DelayLine<Memory, 7> dap2a;
		E::DelayLine<Memory, 8> dap2b;
		E::DelayLine<Memory, 9> del2;
		E::Context c;

		const float kap = diffusion_;
		const float klp = lp_;
		const float krt = reverbTime_;
		const float amount = amount_;
		const float gain = inputGain_;

		float lp_1 = lp_decay_1_;
		float lp_2 = lp_decay_2_;

		for (int i = 0; i < numSamples; i++)
		{
			float wet;
			float apout = 0.0f;
			engine_.Start(&c);

			// Smear AP1 inside the loop.
			c.Interpolate(ap1, 10.0f, LFO_1, 60.0f, 1.0f);
			c.Write(ap1, 100, 0.0f);

			c.Read(inputs[0][i] + inputs[1][i], gain);

			// Diffuse through 4 allpasses.
			c.Read(ap1 TAIL, kap);
			c.WriteAllPass(ap1, -kap);
			c.Read(ap2 TAIL, kap);
			c.WriteAllPass(ap2, -kap);
			c.Read(ap3 TAIL, kap);
			c.WriteAllPass(ap3, -kap);
			c.Read(ap4 TAIL, kap);
			c.WriteAllPass(ap4, -kap);
			c.Write(apout);

			// Main reverb loop.
			c.Load(apout);
			c.Interpolate(del2, 4680.0f, LFO_2, 100.0f, krt);
			c.Lp(lp_1, klp);
			c.Read(dap1a TAIL, -kap);
			c.WriteAllPass(dap1a, kap);
			c.Read(dap1b TAIL, kap);
			c.WriteAllPass(dap1b, -kap);
			c.Write(del1, 2.0f);
			c.Write(wet, 0.0f);

			outputs[0][i] = inputs[0][i] + (wet - inputs[0][i]) * amount;

			c.Load(apout);
			// c.Interpolate(del1, 4450.0f, LFO_1, 50.0f, krt);
			c.Read(del1 TAIL, krt);
			c.Lp(lp_2, klp);
			c.Read(dap2a TAIL, kap);
			c.WriteAllPass(dap2a, -kap);
			c.Read(dap2b TAIL, -kap);
			c.WriteAllPass(dap2b, kap);
			c.Write(del2, 2.0f);
			c.Write(wet, 0.0f);

			outputs[1][i] = inputs[1][i] + (wet - inputs[1][i]) * amount;
		}

		lp_decay_1_ = lp_1;
		lp_decay_2_ = lp_2;
	}

	void Pyxis::SetParam(int index, float value)
	{
		switch ((ParamIndices)index)
		{
		case ParamIndices::Amount:		amount_ = value; break;
		case ParamIndices::InputGain:	inputGain_ = value; break;
		case ParamIndices::Time:		reverbTime_ = value; break;
		case ParamIndices::Diffusion:	diffusion_ = value; break;
		case ParamIndices::LP:			lp_ = value; break;
		}
	}

	float Pyxis::GetParam(int index) const
	{
		switch ((ParamIndices)index)
		{
		default:
		case ParamIndices::Amount:		return amount_;
		case ParamIndices::InputGain:	return inputGain_;
		case ParamIndices::Time:		return reverbTime_;
		case ParamIndices::Diffusion:	return diffusion_;
		case ParamIndices::LP:			return lp_;
		}

		return 0.0f;
	}
}

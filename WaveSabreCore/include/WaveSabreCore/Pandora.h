#pragma once

#ifndef __WAVESABRECORE_PANDORA_H__
#define __WAVESABRECORE_PANDORA_H__

#include "SynthDevice.h"
#include "Envelope.h"

namespace WaveSabreCore
{
	class Pandora : public SynthDevice
	{
	public:
		enum class ParamIndices
		{
			MasterLevel,

			NumParams,
		};

		Pandora();

		virtual void SetParam(int index, float value);
		virtual float GetParam(int index) const;

	protected:
		class PandoraVoice : public Voice
		{
		public:
			PandoraVoice(Pandora* pandora);
			virtual WaveSabreCore::SynthDevice* SynthDevice() const;

			virtual void Run(double songPosition, float** outputs, int numSamples);

			virtual void NoteOn(int note, int velocity, float detune, float pan);
			virtual void NoteOff();

		private:
			Pandora* pandora;

			double oscPhase;
			double oscOutput;
		};

		float masterLevel;
	};
}

#endif

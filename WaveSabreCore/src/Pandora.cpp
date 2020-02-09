#include <WaveSabreCore/Pandora.h>
#include <WaveSabreCore/Helpers.h>

#include <math.h>

namespace WaveSabreCore
{
	Pandora::Pandora()
		: SynthDevice((int)ParamIndices::NumParams)
	{
		for (int i = 0; i < maxVoices; i++) voices[i] = new PandoraVoice(this);

		masterLevel = .8f;
	}

	void Pandora::SetParam(int index, float value)
	{
		switch ((ParamIndices)index)
		{


		case ParamIndices::MasterLevel: masterLevel = value; break;
		}
	}

	float Pandora::GetParam(int index) const
	{
		switch ((ParamIndices)index)
		{
		case ParamIndices::MasterLevel:
		default:
			return masterLevel;

		}
	}

	Pandora::PandoraVoice::PandoraVoice(Pandora* pandora)
	{
		this->pandora = pandora;
	}

	SynthDevice* Pandora::PandoraVoice::SynthDevice() const
	{
		return pandora;
	}

	void Pandora::PandoraVoice::Run(double songPosition, float** outputs, int numSamples)
	{
		float masterLevelScalar = Helpers::VolumeToScalar(pandora->masterLevel);

		float leftPanScalar = Helpers::PanToScalarLeft(Pan);
		float rightPanScalar = Helpers::PanToScalarRight(Pan);

		for (int i = 0; i < numSamples; i++)
		{
			double baseNote = GetNote() + Detune;

			double oscInput = oscPhase / Helpers::CurrentSampleRate * 2.0 * 3.141592;
			oscOutput = Helpers::FastSin(oscInput) * 13.25;

			float finalOutput = (float)oscOutput * masterLevelScalar;
			outputs[0][i] += finalOutput * leftPanScalar;
			outputs[1][i] += finalOutput * rightPanScalar;

			double freq1 = Helpers::NoteToFreq(baseNote);
			oscPhase += freq1;
		}
	}

	void Pandora::PandoraVoice::NoteOn(int note, int velocity, float detune, float pan)
	{
		Voice::NoteOn(note, velocity, detune, pan);

		oscPhase = (double)Helpers::RandFloat();
		oscOutput = 0.0;
	}

	void Pandora::PandoraVoice::NoteOff()
	{
		IsOn = false;
	}
}

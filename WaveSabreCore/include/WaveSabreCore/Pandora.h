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
			Osc1waveform,
			Osc2waveform,
			Osc3waveform,
			Osc1baseToneTranspose,
			Osc2baseToneTranspose,
			Osc3baseToneTranspose,
			Osc1finetune,
			Osc2finetune,
			Osc3finetune,
			Osc1pulseWidth,
			Osc2pulseWidth,
			Osc3pulseWidth,
			MixAmountOsc1,
			MixAmountOsc2,
			MixAmountOsc3,
			StringDamping,
			StringFeedbackAmount,
			StringFeedbackDelay,
			StringThickness,
			StringLevel,
			OscStringMix,
			Osc2sync,
			Lfo1rate,
			Lfo2rate,
			Lfo3rate,
			Lfo1waveform,
			Lfo2waveform,
			Lfo3waveform,
			Lfo1keysync,
			Lfo2keysync,
			Lfo3keysync,
			Lfo1positive,
			Lfo2positive,
			Lfo3positive,
			Envelope1attackRate,
			Envelope1decayRate,
			Envelope1sustainLevel,
			Envelope1releaseRate,
			Envelope2attackRate,
			Envelope2decayRate,
			Envelope2sustainLevel,
			Envelope2releaseRate,
			Envelope3attackRate,
			Envelope3decayRate,
			Envelope3sustainLevel,
			Envelope3releaseRate,
			Envelope4attackRate,
			Envelope4decayRate,
			Envelope4sustainLevel,
			Envelope4releaseRate,
			VcfRouting,
			Vcf1type,
			Vcf1Cutoff,
			Vcf1Resonance,
			Vcf2type,
			Vcf2Cutoff,
			Vcf2Resonance,
			VcfCtrlBalance,
			Vcf2CutoffRelative,
			VcfDistType,
			FilterDistDrive,
			FilterDistShape,
			DoSlide,
			SlideSpeed,
			NumUnisonVoices,
			UnisonSpread,
			ArpeggioType,
			ArpeggioNumOctaves,
			ArpeggioInterval,
			ArpeggioNoteDuration,

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


	public:
		// These internal parameters are made public, so that the PandoraVST can directly read them to display the correct internal value.

		enum class OscWaveformType
		{
			OFF = 0,
			SINE,
			SQUARE,
			SAW,
			TRIANGLE,
			NOISE,
			BISQUARE,
			NOISE_LOOP,

			COUNT
		};

		enum class LfoWaveformType
		{
			SINE = 0,
			SQUARE,
			BISQUARE,
			SAW,
			TRIANGLE,
			NOISEHOLD,
			NOISEGLIDE,

			COUNT
		};

		enum class FilterType
		{
			LPF = 0,
			BPF,
			HPF,
			ALLPASS,
			NOTCH,

			COUNT
		};

		enum class FilterRoutingType
		{
			NONE = 0,
			SINGLE,
			SERIAL,
			PARALLEL,

			COUNT
		};

		enum class LfoSyncType
		{
			OFF = 0,
			KEY,
			GATE,

			COUNT
		};

		struct Envelope
		{
			float attackRate;
			float decayRate;
			float sustainLevel;
			float releaseRate;

			Envelope() {}

			Envelope(float a, float d, float s, float r)
				: attackRate(a), decayRate(d), sustainLevel(s), releaseRate(r)
			{}
		};

		enum class ArpeggioType
		{
			OFF = 0,
			UP,
			DOWN,
			UPDOWN,

			COUNT
		};


		// Oscs:
		OscWaveformType osc1waveform;				// type of waveform for oscillator
		OscWaveformType osc2waveform;				// type of waveform for oscillator
		OscWaveformType osc3waveform;				// type of waveform for oscillator

		int osc1baseToneTranspose;		// semi-tone transposing (0 = stay normal)
		int osc2baseToneTranspose;		// semi-tone transposing (0 = stay normal)
		int osc3baseToneTranspose;		// semi-tone transposing (0 = stay normal)
		float osc1finetune;				// -1..1
		float osc2finetune;				// -1..1
		float osc3finetune;				// -1..1

		float osc1pulseWidth;			// -1..1
		float osc2pulseWidth;			// -1..1
		float osc3pulseWidth;			// -1..1

		float mixAmountOsc1;
		float mixAmountOsc2;
		float mixAmountOsc3;

		float stringDamping; // 0..1
		float stringFeedbackAmount; // 0..1
		int stringFeedbackDelay; // 1..inf.
		float stringThickness; // 0..1
		float stringLevel; // 0..1
		float oscStringMix; // -1..1 (-1=100% osc, 1=100% string, 0=100% osc and 100% string)

		bool osc2sync; // when true, we sync to osc1

		// Lfos:
		float lfo1rate;
		float lfo2rate;
		float lfo3rate;
		LfoWaveformType lfo1waveform; // type of waveform for lfo
		LfoWaveformType lfo2waveform; // type of waveform for lfo
		LfoWaveformType lfo3waveform; // type of waveform for lfo
		LfoSyncType lfo1keysync; // type of keysync for lfo 
		LfoSyncType lfo2keysync; // type of keysync for lfo 
		LfoSyncType lfo3keysync; // type of keysync for lfo 
		bool lfo1positive;
		bool lfo2positive;
		bool lfo3positive;

		// Envelopes:
		Envelope envelope1;
		Envelope envelope2;
		Envelope envelope3;
		Envelope envelope4;

		// Filters:
		FilterRoutingType vcfRouting;
		FilterType vcf1type;
		float vcf1Cutoff;
		float vcf1Resonance;
		FilterType vcf2type;
		float vcf2Cutoff;
		float vcf2Resonance;
		float vcf1amountParallel;
		float vcf2amountParallel;
		bool vcf2CutoffRelative;

		bool doFilterDist;
		bool filterDistPassive;
		float filterDistDrive;
		float filterDistShape;

		// Sliding:
		bool doSlide;
		float slideSpeed; // special multiplier factor (~0.9995 is nice)

		// Unison:
		int numUnisonVoices;
		float unisonSpread; // in semi-tones

		// Arpeggio:
		ArpeggioType arpeggioType;
		int arpeggioNumOctaves;
		__int64 arpeggioInterval;
		__int64 arpeggioNoteDuration;
	};
}

#endif

#pragma once

#ifndef __WAVESABRECORE_PANDORA_H__
#define __WAVESABRECORE_PANDORA_H__

#include "SynthDevice.h"
#include "Envelope.h"
#include "FixedSizeList.h"

#define PANDORA_MAX_MODULATORS_PER_DEST 8
#define PANDORA_NUM_MODULATOR_DEST 22

namespace WaveSabreCore
{
	class Pandora : public SynthDevice
	{
	public:
		enum class ModulatorParamIndices
		{
			Source = 0,
			DepthSource,
			ConstantDepth,
			ConstantDepthRange,

			COUNT
		};

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
			Pan,

			ModulatorFirstParam,
			ModulatorLastParam = ModulatorFirstParam + (PANDORA_NUM_MODULATOR_DEST * PANDORA_MAX_MODULATORS_PER_DEST * (int)ModulatorParamIndices::COUNT) - 1,

			NumParams,
		};

		Pandora();

		virtual void SetParam(int index, float value);
		virtual float GetParam(int index) const;

		virtual void Run(double songPosition, float** inputs, float** outputs, int numSamples);

		static void sParamIndexToModulatorIndices(int index, int* modParamIndex, int* modIndex, int* destIndex);

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

		enum class ArpeggioType
		{
			OFF = 0,
			UP,
			DOWN,
			UPDOWN,

			COUNT
		};


		struct EnvelopeSettings
		{
			float attackRate;
			float decayRate;
			float sustainLevel;
			float releaseRate;

			EnvelopeSettings() {}

			EnvelopeSettings(float a, float d, float s, float r)
				: attackRate(a), decayRate(d), sustainLevel(s), releaseRate(r)
			{}
		};

		// modulation sources
		// when adding new ones:
		//		- add to this ModulationSourceType struct
		//		- add to Voice::ResolvePatchModulations() a switch case
		//		- add a modulation source option to the pulldown (in modulationpanelimpl.cpp)
		enum class ModulationSourceType
		{
			NONE = 0,
			ENV1,
			ENV2,
			ENV3,
			ENV4,
			LFO1,
			LFO2,
			LFO3,
			OSC1,
			OSC2,
			OSC3,
			MIDICTRL_A,
			MIDICTRL_B,
			MIDICTRL_C,
			MIDICTRL_D,

			COUNT
		};

		enum class ModulationDepthSourceType
		{
			CONSTANT = 0,
			A,
			B,
			C,
			D,

			COUNT
		};

		enum class ModulationDepthRange
		{
			ONE = 0,
			SIXTEEN,
			SIXTYFOUR,

			COUNT
		};

		static float sModulationDepthRangeFactor[3];

		struct UnresolvedModulationType
		{
			ModulationSourceType source					= ModulationSourceType::NONE;			// ModulationSourceType
			float constantDepth							= 0.0f;									// constant amount (-1..+1)
			ModulationDepthRange constantDepthRange		= ModulationDepthRange::ONE;			// range			
			ModulationDepthSourceType depthSource		= ModulationDepthSourceType::CONSTANT;	// -1 for none, 0..3 for modDepthA..D
		};

		struct ResolvedModulationType
		{
			float* resolvedSource; // if NULL then not in use
			float* resolvedDepth;
			float constantDepth; // resolvedDepth points to this one when using constant modulation depth
		};

		enum class ModulationDestType
		{
			OSC1TUNE = 0,
			OSC2TUNE,
			VCF1CUTOFF,
			VCF1RESONANCE,
			VCF2CUTOFF,
			VCF2RESONANCE,
			VCA,
			OSC3TUNE,
			OSC1PULSEWIDTH,
			OSC2PULSEWIDTH,
			OSC3PULSEWIDTH,
			MODDEPTHA,
			MODDEPTHB,
			MODDEPTHC,
			MODDEPTHD,
			OSC1LEVEL,
			OSC2LEVEL,
			OSC3LEVEL,
			STRINGLEVEL,
			LFO1RATE,
			LFO2RATE,
			LFO3RATE,
			COUNT
		};

		static_assert((int)ModulationDestType::COUNT == PANDORA_NUM_MODULATOR_DEST, "Detected a mismatch between the number of entries in ModulationDestType and the defined number of destinations PANDORA_NUM_MODULATOR_DEST!");

		// modulations,
		// when adding a new destination:
		//		- add a consumer code block to Voice::Render()
		//		- add entry to ModulationDestType enum (at bottom)
		//		- increase PANDORA_NUM_MODULATOR_DEST by 1
		// when adding a new source:
		//		- add a modulator code block to Voice::Render()
		//		- add entry to ModulationSourceType enum (at bottom)
		template <typename T>
		struct AllModulationsType
		{
			bool IsDependingOn(ModulationSourceType src) const
			{
				ASSERT(src != ModulationSourceType::NONE);
				return (usedSourcesMask & (1 << ((int)src-1))) != 0;
			}

			bool IsAffecting(ModulationDestType dest) const
			{
				return modulationsPerDest[(int)dest].usedModulatorsMask != 0;
			}			

			struct ModulationsType
			{
				bool IsUsed(int index) const
				{
					return (usedModulatorsMask & (1 << index)) != 0;
				}
				 
				void SetUsed(int index, bool used)
				{
					if (used)
						usedModulatorsMask = usedModulatorsMask | (1 << index);
					else
						usedModulatorsMask = usedModulatorsMask & ~(1 << index);
				}

				T modulations[PANDORA_MAX_MODULATORS_PER_DEST];
				unsigned int usedModulatorsMask = 0; //< Bitmask indicating which items in modulations[] are in use
				unsigned int usedSourcesMask = 0;  //< Bitmask of which sources were used by these modulations
			};

			ModulationsType modulationsPerDest[(int)ModulationDestType::COUNT];
			unsigned int usedSourcesMask = 0; //< Bitmask of which sources were used by all modulations in total
		};


		// 
		// Parameters:
		//

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
		EnvelopeSettings envelope1;
		EnvelopeSettings envelope2;
		EnvelopeSettings envelope3;
		EnvelopeSettings envelope4;

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
		float unisonSpread; // in semi-tones

		// Arpeggio:
		ArpeggioType arpeggioType;
		int arpeggioNumOctaves;
		__int64 arpeggioInterval;
		__int64 arpeggioNoteDuration;

		// Modulation:
		AllModulationsType<UnresolvedModulationType> modulations;

		// Stuff that won't be serialized:
		float midiControlledSettingA;
		float midiControlledSettingB;
		float midiControlledSettingC;
		float midiControlledSettingD;

	private:
		void RecalculateUsedModulationSourcesMasks(AllModulationsType<UnresolvedModulationType>& allModulations, AllModulationsType<UnresolvedModulationType>::ModulationsType& mods);

		class LFO
		{
		public:
			LFO();
			~LFO();


			void SetRate(float rate)
			{
				this->rate = rate;
			}

			void SetWaveform(LfoWaveformType waveform)
			{
				this->waveform = waveform;
			}

			void SetPositive(bool positive)
			{
				this->positive = positive;
			}

			void Update(int numSamples);

			float* GetBuffer()
			{
				return buffer;
			}

			void SyncTo(LFO* other)
			{
				this->time = other->time;
			}

		private:
			float* buffer;

			float time;

			float rate;
			LfoWaveformType waveform;

			bool positive; // only positive results?

			bool noiseFlip;
			float prevNoise, glideSpeed;

			float output;
		};

		class Envelope
		{
		public:
			enum class EnvelopeStage
			{
				ATTACK,
				DECAY,
				SUSTAIN,
				RELEASE
			};

			Envelope(const EnvelopeSettings& settings) :
				stage(EnvelopeStage::ATTACK), hasEnded(false), level(0), settings(settings)
			{}

			bool HasEnded() const
			{
				return hasEnded;
			}

			void Step(bool gate);

			float GetLevel() const
			{
				return level;
			}

			float* GetLevelPtr()
			{
				return &level;
			}

			void SetStage(EnvelopeStage stage)
			{
				this->stage = stage;
			}

			void SetLevel(float level)
			{
				this->level = level;
			}


		private:
			EnvelopeStage stage;
			bool hasEnded;
			float level;

			const EnvelopeSettings& settings;
		};

		class PandoraVoice : public Voice
		{
		public:
			PandoraVoice(Pandora* pandora);
			virtual ~PandoraVoice();
			virtual WaveSabreCore::SynthDevice* SynthDevice() const;

			virtual void Run(double songPosition, float** outputs, int numSamples);

			virtual void NoteOn(int note, int velocity, float detune, float pan);
			virtual void NoteOff();

		private:
			void Terminate();
			float GetModulationAmountSummed(Pandora::ModulationDestType dest) const;
			float GetModulationAmountMultiplied(Pandora::ModulationDestType dest) const;
			void ResolveModulation(ResolvedModulationType& dest, const UnresolvedModulationType& src);
			void ResolveModulations(AllModulationsType<ResolvedModulationType>::ModulationsType& dest, const AllModulationsType<UnresolvedModulationType>::ModulationsType& src);
			void ResolveAllModulations();

			Pandora* pandora;

			bool gate; ///< current gate signal for this voice
			float inputVelocity; ///< velocity used to trigger this

			double osc1time; ///< time spent in this voice, on osc1
			double osc2time; ///< time spent in this voice, on osc2
			double osc3time; ///< time spent in this voice, on osc3

			double osc1timestep;
			double osc2timestep;
			double osc3timestep;

			int noiseLoopOffset;

			float osc1level, osc2level, osc3level;

			AllModulationsType<ResolvedModulationType> resolvedModulations;

			float currentLfo1Amount; // placed here, so we can modulate it.
			float currentLfo2Amount; // placed here, so we can modulate it.
			float currentLfo3Amount; // placed here, so we can modulate it.

			float modulatedModulationDepth[4];

			LFO* lfo[3]; // voice-specific lfo.
			bool hasOwnLFO[3];

			float vcaLevel;

			//double slideAmount;
			//double slideInitialModifierOsc1;
			//double slideInitialModifierOsc2;
			//double slideInitialModifierOsc3;
			//double slideScaleFactorPerSample;

			// filter state variables
			float filter1low, filter1band;
			float filter2low, filter2band;

			//VoiceStringHelper stringHelper;

			__inline float SampleOscillator(double time, OscWaveformType waveform, float pulseWidth);

			// envelope states
			Envelope envelope1;
			Envelope envelope2;
			Envelope envelope3;
			Envelope envelope4;

			// filter distortion
			//EffectDistortion filterDistortion;

			//int unisonVoiceIndex; // index in the set of unison voices	
		};


		LFO lfo[3];
	};
}

#endif

#include <WaveSabreCore/Pandora.h>
#include <WaveSabreCore/Helpers.h>

#include <math.h>

namespace WaveSabreCore
{
	Pandora::Pandora()
		: SynthDevice((int)ParamIndices::NumParams)
	{
		for (int i = 0; i < maxVoices; i++) voices[i] = new PandoraVoice(this);

		osc1waveform = OscWaveformType::SAW;
		osc2waveform = OscWaveformType::OFF;
		osc3waveform = OscWaveformType::OFF;
		osc1baseToneTranspose = 0;
		osc2baseToneTranspose = 0;
		osc3baseToneTranspose = 0;
		osc1finetune = 0;
		osc2finetune = 0;
		osc3finetune = 0;
		osc1pulseWidth = 0;
		osc2pulseWidth = 0;
		osc3pulseWidth = 0;
		mixAmountOsc1 = 1;
		mixAmountOsc2 = 0;
		mixAmountOsc3 = 0;
		lfo1rate = 0.0001f;
		lfo1waveform = LfoWaveformType::SINE;
		lfo1keysync = LfoSyncType::OFF;
		lfo2rate = 0.0001f;
		lfo2waveform = LfoWaveformType::SINE;
		lfo2keysync = LfoSyncType::OFF;
		lfo3rate = 0.0001f;
		lfo3waveform = LfoWaveformType::SINE;
		lfo3keysync = LfoSyncType::OFF;
		lfo1positive = false;
		lfo2positive = false;
		lfo3positive = false;
		envelope1 = Envelope(0.00005f, 0.00005f, 0.7f, 0.00005f);
		envelope2 = Envelope(0.00005f, 0.00005f, 0.7f, 0.00005f);
		envelope3 = Envelope(0.00005f, 0.00005f, 0.7f, 0.00005f);
		envelope4 = Envelope(0.00005f, 0.00005f, 0.7f, 0.00005f);
		vcfRouting = FilterRoutingType::NONE;
		vcf1type = FilterType::LPF;
		vcf1Cutoff = 0.49f;
		vcf1Resonance = 1.0f;
		vcf2type = FilterType::LPF;
		vcf2Cutoff = 0.49f;
		vcf2Resonance = 1.00f;
		vcf2CutoffRelative = false;
		vcf1amountParallel = 1;
		vcf2amountParallel = 1;
		stringDamping = 0.5f;
		stringFeedbackDelay = 10;
		stringFeedbackAmount = 0;
		stringThickness = 1;
		stringLevel = 0;
		oscStringMix = 0;
		doSlide = false;
		slideSpeed = 0.9f;
		doFilterDist = false;
		filterDistPassive = false;
		filterDistDrive = 0.0f;
		filterDistShape = 0.0f;
		osc2sync = false;
		numUnisonVoices = 1;
		unisonSpread = 0.05f;
		arpeggioType = ArpeggioType::OFF;
		arpeggioNumOctaves = 1;
		arpeggioInterval = 16;
		arpeggioNoteDuration = 8;
	}


	static float sLfoRateLinearToExponential(float value)
	{
		//  LFOrate:
		//
		// 	value = 0 .. 1  0 = slow, 1 = fast

		// 	lforate(0) = 0.0000001f
		// 	lforate(1) = 1

		// 	lforate(x) = a * b^x
		// 	lforate(0) = a --> a = 0.0000001f
		// 	lforate(1) = a * b --> b = 1 / 0.0000001f = 10000000
		//
		// 	lforate(x) = 0.0000001f * 10000000^x
		//
		//  which equals: (10^7)^(x-1)

		return powf(10000000.0f, value - 1);
	}


	static float sLfoRateExponentialToLinear(float value)
	{
		// inverse of above formula:
		//
		// value(x) = 0.0000001f * 10000000^x
		// value(x) / 0.0000001f = 10000000^x
		// log10(value(x) / 0.0000001f) / log10(10000000) = x

		return (float)(log10f(value / 0.0000001f) / 7.0f); //log10(10000000) = 7
	}

	static float sEnvelopeLinearToExponential(float value)
	{
		// envelope rates:
		//   
		// min: 4 second slide                  = 0.0000056689342   
		// max: 0.0000226757 sec = 0.0226757 ms = 1
		//
		// value = a * b^x   met x=0 voor min en x=1 voor max
		// value(0) = a --> a = 0.0000056689342
		// value(1) = a * b = 1 --> b = 1 / a = 1 / 0.0000056689342 = 176400
		//
		// value(x) = 0.0000056689342 * 176400^x

		// This mapping is also part of Pandora itself, do not simply modify...

		return 0.0000056689342f * powf(176400.0f, value);
	}

	static float sEnvelopeExponentialToLinear(float value)
	{
		// inverse of above formula:
		//
		// value(x) = 0.0000056689342 * 176400^x
		// value(x) / 0.0000056689342 = 176400^x
		// log10(value(x) / 0.0000056689342) / log10(176400) = x

		return (float)(log10f(value / 0.0000056689342f) / log10f(176400.0f));
	}


	static float sSlideLinearToExponential(float value)
	{
		return 1.0f - powf(10.0f, -(4.0f - value * 3.0f));
	}


	static float sSlideExponentialToLinear(float value)
	{
		// inverse of above formula:

		float v = 1.0f - value;


		return (4.0f + log10f(v)) / 3.0f;
	}

	// 0..1 --> 1..12 
	static int sNumUnisonVoicesLinearToExponential(float value)
	{
		return (1 << (int)(value * 5.0f));
	}

	// 1..12 --> 0..1
	static float sNumUnisonVoicesExponentialToLinear(int value)
	{
		return Helpers::Clamp(floorf(log2f((float)value)) / 5.0f, 0.0f, 1.0f);
	}

	// 0..1 --> 1..2^48
	static int sArpeggioIntervalLinearToExponential(float value)
	{
		return (1 << (int)(value * 48.0f));
	}

	// 1..2^48 --> 0..1
	static float sArpeggioIntervalExponentialToLinear(int value)
	{
		return Helpers::Clamp(floorf(log2f((float)value)) / 48.0f, 0.0f, 1.0f);
	}



	void Pandora::SetParam(int index, float value)
	{
		switch ((ParamIndices)index)
		{
		case ParamIndices::Osc1waveform:				osc1waveform = Helpers::ParamToEnum<OscWaveformType>(value); break;
		case ParamIndices::Osc2waveform:				osc2waveform = Helpers::ParamToEnum<OscWaveformType>(value); break;
		case ParamIndices::Osc3waveform:				osc3waveform = Helpers::ParamToEnum<OscWaveformType>(value); break;
		case ParamIndices::Osc1baseToneTranspose:		osc1baseToneTranspose = Helpers::ParamToRangedInt(value, -24, 24); break;
		case ParamIndices::Osc2baseToneTranspose:		osc2baseToneTranspose = Helpers::ParamToRangedInt(value, -24, 24); break;
		case ParamIndices::Osc3baseToneTranspose:		osc3baseToneTranspose = Helpers::ParamToRangedInt(value, -24, 24); break;
		case ParamIndices::Osc1finetune:				osc1finetune = Helpers::ParamToRangedFloat(value, -1.0f, 1.0f); break;
		case ParamIndices::Osc2finetune:				osc2finetune = Helpers::ParamToRangedFloat(value, -1.0f, 1.0f); break;
		case ParamIndices::Osc3finetune:				osc3finetune = Helpers::ParamToRangedFloat(value, -1.0f, 1.0f); break;
		case ParamIndices::Osc1pulseWidth:				osc1pulseWidth = Helpers::ParamToRangedFloat(value, -1.0f, 1.0f); break;
		case ParamIndices::Osc2pulseWidth:				osc2pulseWidth = Helpers::ParamToRangedFloat(value, -1.0f, 1.0f); break;
		case ParamIndices::Osc3pulseWidth:				osc3pulseWidth = Helpers::ParamToRangedFloat(value, -1.0f, 1.0f); break;
		case ParamIndices::MixAmountOsc1:				mixAmountOsc1 = value; break;
		case ParamIndices::MixAmountOsc2:				mixAmountOsc2 = value; break;
		case ParamIndices::MixAmountOsc3:				mixAmountOsc3 = value; break;
		case ParamIndices::StringDamping:				stringDamping = value; break;
		case ParamIndices::StringFeedbackAmount:		stringFeedbackAmount = value; break;
		case ParamIndices::StringFeedbackDelay:			stringFeedbackDelay = Helpers::ParamToRangedInt(value, 0, 100); break;
		case ParamIndices::StringThickness:				stringThickness = value; break;
		case ParamIndices::StringLevel:					stringLevel = value; break;
		case ParamIndices::OscStringMix:				oscStringMix = Helpers::ParamToRangedFloat(value, -1.0f, 1.0f); break;
		case ParamIndices::Osc2sync:					osc2sync = Helpers::ParamToBoolean(value); break;
		case ParamIndices::Lfo1rate:					lfo1rate = sLfoRateLinearToExponential(value); break;
		case ParamIndices::Lfo2rate:					lfo2rate = sLfoRateLinearToExponential(value); break;
		case ParamIndices::Lfo3rate:					lfo3rate = sLfoRateLinearToExponential(value); break;
		case ParamIndices::Lfo1waveform:				lfo1waveform = Helpers::ParamToEnum<LfoWaveformType>(value); break;
		case ParamIndices::Lfo2waveform:				lfo2waveform = Helpers::ParamToEnum<LfoWaveformType>(value); break;
		case ParamIndices::Lfo3waveform:				lfo3waveform = Helpers::ParamToEnum<LfoWaveformType>(value); break;
		case ParamIndices::Lfo1keysync:					lfo1keysync = Helpers::ParamToEnum<LfoSyncType>(value); break;
		case ParamIndices::Lfo2keysync:					lfo2keysync = Helpers::ParamToEnum<LfoSyncType>(value); break;
		case ParamIndices::Lfo3keysync:					lfo3keysync = Helpers::ParamToEnum<LfoSyncType>(value); break;
		case ParamIndices::Lfo1positive:				lfo1positive = Helpers::ParamToBoolean(value); break;
		case ParamIndices::Lfo2positive:				lfo2positive = Helpers::ParamToBoolean(value); break;
		case ParamIndices::Lfo3positive:				lfo3positive = Helpers::ParamToBoolean(value); break;
		case ParamIndices::Envelope1attackRate:			envelope1.attackRate = sEnvelopeLinearToExponential(value); break;
		case ParamIndices::Envelope1decayRate:			envelope1.decayRate = sEnvelopeLinearToExponential(value); break;
		case ParamIndices::Envelope1sustainLevel:		envelope1.sustainLevel = value; break;
		case ParamIndices::Envelope1releaseRate:		envelope1.releaseRate = sEnvelopeLinearToExponential(value); break;
		case ParamIndices::Envelope2attackRate:			envelope2.attackRate = sEnvelopeLinearToExponential(value); break;
		case ParamIndices::Envelope2decayRate:			envelope2.decayRate = sEnvelopeLinearToExponential(value); break;
		case ParamIndices::Envelope2sustainLevel:		envelope2.sustainLevel = value; break;
		case ParamIndices::Envelope2releaseRate:		envelope2.releaseRate = sEnvelopeLinearToExponential(value); break;
		case ParamIndices::Envelope3attackRate:			envelope3.attackRate = sEnvelopeLinearToExponential(value); break;
		case ParamIndices::Envelope3decayRate:			envelope3.decayRate = sEnvelopeLinearToExponential(value); break;
		case ParamIndices::Envelope3sustainLevel:		envelope3.sustainLevel = value; break;
		case ParamIndices::Envelope3releaseRate:		envelope3.releaseRate = sEnvelopeLinearToExponential(value); break;
		case ParamIndices::Envelope4attackRate:			envelope4.attackRate = sEnvelopeLinearToExponential(value); break;
		case ParamIndices::Envelope4decayRate:			envelope4.decayRate = sEnvelopeLinearToExponential(value); break;
		case ParamIndices::Envelope4sustainLevel:		envelope4.sustainLevel = value; break;
		case ParamIndices::Envelope4releaseRate:		envelope4.releaseRate = sEnvelopeLinearToExponential(value); break;
		case ParamIndices::VcfRouting:					vcfRouting = Helpers::ParamToEnum<FilterRoutingType>(value); break;
		case ParamIndices::Vcf1type:					vcf1type = Helpers::ParamToEnum<FilterType>(value); break;
		case ParamIndices::Vcf1Cutoff:					vcf1Cutoff = Helpers::ParamToRangedFloat(value, 0.002f, 0.7f); break;
		case ParamIndices::Vcf1Resonance:				vcf1Resonance = Helpers::ParamToRangedFloat(value, 0.0f, 0.9f); break;
		case ParamIndices::Vcf2type:					vcf2type = Helpers::ParamToEnum<FilterType>(value); break;
		case ParamIndices::Vcf2Cutoff:					vcf2Cutoff = Helpers::ParamToRangedFloat(value, 0.002f, 0.7f); break;
		case ParamIndices::Vcf2Resonance:				vcf2Resonance = Helpers::ParamToRangedFloat(value, 0.0f, 0.9f); break;
		case ParamIndices::VcfCtrlBalance:				
			if (value < 0.5f) 
			{
				vcf1amountParallel = 1.0f; 
				vcf2amountParallel = value*2.0f; // 0..1
			} 
			else
			{
				vcf1amountParallel = 1.0f - ((value - 0.5f) * 2.0f); // 1..0
				vcf2amountParallel = 1.0f;
			}
			break;
		case ParamIndices::Vcf2CutoffRelative:			vcf2CutoffRelative = Helpers::ParamToBoolean(value); break;
		case ParamIndices::VcfDistType:				
			doFilterDist = Helpers::ParamToRangedInt(value, 0, 2) != 0;
			filterDistPassive = Helpers::ParamToRangedInt(value, 0, 2) == 1;
			break;
		case ParamIndices::FilterDistDrive:				filterDistDrive = value; break;
		case ParamIndices::FilterDistShape:				filterDistShape = value; break;
		case ParamIndices::DoSlide:						doSlide = Helpers::ParamToBoolean(value); break;
		case ParamIndices::SlideSpeed:					slideSpeed = sSlideLinearToExponential(value); break;
		case ParamIndices::NumUnisonVoices:				numUnisonVoices = sNumUnisonVoicesLinearToExponential(value);  break;
		case ParamIndices::UnisonSpread:				unisonSpread = Helpers::ParamToRangedFloat(value, 0.0f, 0.5f); break;
		case ParamIndices::ArpeggioType:				arpeggioType = Helpers::ParamToEnum<ArpeggioType>(value); break;
		case ParamIndices::ArpeggioNumOctaves:			arpeggioNumOctaves = Helpers::ParamToRangedInt(value, 1, 8); break;
		case ParamIndices::ArpeggioInterval:			arpeggioInterval = sArpeggioIntervalLinearToExponential(value); break;
		case ParamIndices::ArpeggioNoteDuration:		arpeggioNoteDuration = sArpeggioIntervalLinearToExponential(value); break;
		}
	}

	float Pandora::GetParam(int index) const
	{
		switch ((ParamIndices)index)
		{
		case ParamIndices::Osc1waveform:				return Helpers::EnumToParam<OscWaveformType>(osc1waveform);
		case ParamIndices::Osc2waveform:				return Helpers::EnumToParam<OscWaveformType>(osc2waveform);
		case ParamIndices::Osc3waveform:				return Helpers::EnumToParam<OscWaveformType>(osc3waveform);
		case ParamIndices::Osc1baseToneTranspose:		return Helpers::RangedIntToParam(osc1baseToneTranspose, -24, 24);
		case ParamIndices::Osc2baseToneTranspose:		return Helpers::RangedIntToParam(osc2baseToneTranspose, -24, 24);
		case ParamIndices::Osc3baseToneTranspose:		return Helpers::RangedIntToParam(osc3baseToneTranspose, -24, 24);
		case ParamIndices::Osc1finetune:				return Helpers::RangedFloatToParam(osc1finetune, -1.0f, 1.0f); 
		case ParamIndices::Osc2finetune:				return Helpers::RangedFloatToParam(osc2finetune, -1.0f, 1.0f); 
		case ParamIndices::Osc3finetune:				return Helpers::RangedFloatToParam(osc3finetune, -1.0f, 1.0f); 
		case ParamIndices::Osc1pulseWidth:				return Helpers::RangedFloatToParam(osc1pulseWidth, -1.0f, 1.0f); break;
		case ParamIndices::Osc2pulseWidth:				return Helpers::RangedFloatToParam(osc2pulseWidth, -1.0f, 1.0f); break;
		case ParamIndices::Osc3pulseWidth:				return Helpers::RangedFloatToParam(osc3pulseWidth, -1.0f, 1.0f); break;
		case ParamIndices::MixAmountOsc1:				return mixAmountOsc1;
		case ParamIndices::MixAmountOsc2:				return mixAmountOsc2;
		case ParamIndices::MixAmountOsc3:				return mixAmountOsc3;
		case ParamIndices::StringDamping:				return stringDamping;
		case ParamIndices::StringFeedbackAmount:		return stringFeedbackAmount;
		case ParamIndices::StringFeedbackDelay:			return Helpers::RangedIntToParam(stringFeedbackDelay, 0, 100);
		case ParamIndices::StringThickness:				return stringThickness;
		case ParamIndices::StringLevel:					return stringLevel;
		case ParamIndices::OscStringMix:				return Helpers::RangedFloatToParam(oscStringMix, -1.0f, 1.0f);
		case ParamIndices::Osc2sync:					return Helpers::BooleanToParam(osc2sync);
		case ParamIndices::Lfo1rate:					return sLfoRateExponentialToLinear(lfo1rate);
		case ParamIndices::Lfo2rate:					return sLfoRateExponentialToLinear(lfo2rate);
		case ParamIndices::Lfo3rate:					return sLfoRateExponentialToLinear(lfo3rate);
		case ParamIndices::Lfo1waveform:				return Helpers::EnumToParam<LfoWaveformType>(lfo1waveform);
		case ParamIndices::Lfo2waveform:				return Helpers::EnumToParam<LfoWaveformType>(lfo2waveform);
		case ParamIndices::Lfo3waveform:				return Helpers::EnumToParam<LfoWaveformType>(lfo3waveform);
		case ParamIndices::Lfo1keysync:					return Helpers::EnumToParam<LfoSyncType>(lfo1keysync);
		case ParamIndices::Lfo2keysync:					return Helpers::EnumToParam<LfoSyncType>(lfo2keysync);
		case ParamIndices::Lfo3keysync:					return Helpers::EnumToParam<LfoSyncType>(lfo3keysync);
		case ParamIndices::Lfo1positive:				return Helpers::BooleanToParam(lfo1positive);
		case ParamIndices::Lfo2positive:				return Helpers::BooleanToParam(lfo2positive);
		case ParamIndices::Lfo3positive:				return Helpers::BooleanToParam(lfo3positive);
		case ParamIndices::Envelope1attackRate:			return sEnvelopeExponentialToLinear(envelope1.attackRate);
		case ParamIndices::Envelope1decayRate:			return sEnvelopeExponentialToLinear(envelope1.decayRate);
		case ParamIndices::Envelope1sustainLevel:		return envelope1.sustainLevel;
		case ParamIndices::Envelope1releaseRate:		return sEnvelopeExponentialToLinear(envelope1.releaseRate);
		case ParamIndices::Envelope2attackRate:			return sEnvelopeExponentialToLinear(envelope2.attackRate);
		case ParamIndices::Envelope2decayRate:			return sEnvelopeExponentialToLinear(envelope2.decayRate);
		case ParamIndices::Envelope2sustainLevel:		return envelope2.sustainLevel;
		case ParamIndices::Envelope2releaseRate:		return sEnvelopeExponentialToLinear(envelope2.releaseRate);
		case ParamIndices::Envelope3attackRate:			return sEnvelopeExponentialToLinear(envelope3.attackRate);
		case ParamIndices::Envelope3decayRate:			return sEnvelopeExponentialToLinear(envelope3.decayRate);
		case ParamIndices::Envelope3sustainLevel:		return envelope3.sustainLevel;
		case ParamIndices::Envelope3releaseRate:		return sEnvelopeExponentialToLinear(envelope3.releaseRate);
		case ParamIndices::Envelope4attackRate:			return sEnvelopeExponentialToLinear(envelope4.attackRate);
		case ParamIndices::Envelope4decayRate:			return sEnvelopeExponentialToLinear(envelope4.decayRate);
		case ParamIndices::Envelope4sustainLevel:		return envelope4.sustainLevel;
		case ParamIndices::Envelope4releaseRate:		return sEnvelopeExponentialToLinear(envelope4.releaseRate);
		case ParamIndices::VcfRouting:					return Helpers::EnumToParam<FilterRoutingType>(vcfRouting);
		case ParamIndices::Vcf1type:					return Helpers::EnumToParam<FilterType>(vcf1type);
		case ParamIndices::Vcf1Cutoff:					return Helpers::RangedFloatToParam(vcf1Cutoff, 0.002f, 0.7f); 
		case ParamIndices::Vcf1Resonance:				return Helpers::RangedFloatToParam(vcf1Resonance, 0.0f, 0.9f);
		case ParamIndices::Vcf2type:					return Helpers::EnumToParam<FilterType>(vcf2type);
		case ParamIndices::Vcf2Cutoff:					return Helpers::RangedFloatToParam(vcf2Cutoff, 0.002f, 0.7f);
		case ParamIndices::Vcf2Resonance:				return Helpers::RangedFloatToParam(vcf2Resonance, 0.0f, 0.9f);
		case ParamIndices::VcfCtrlBalance:
			if (vcf1amountParallel >= vcf2amountParallel)
				return vcf2amountParallel * 0.5f;
			else
				return 1.0f - 0.5f * vcf1amountParallel;
		case ParamIndices::Vcf2CutoffRelative:			return Helpers::BooleanToParam(vcf2CutoffRelative);
		case ParamIndices::VcfDistType:					return Helpers::RangedIntToParam(doFilterDist ? (filterDistPassive ? 1 : 2) : 0, 0, 2);
		case ParamIndices::FilterDistDrive:				return filterDistDrive;
		case ParamIndices::FilterDistShape:				return filterDistShape;
		case ParamIndices::DoSlide:						return Helpers::BooleanToParam(doSlide);
		case ParamIndices::SlideSpeed:					return sSlideExponentialToLinear(slideSpeed);
		case ParamIndices::NumUnisonVoices:				return sNumUnisonVoicesExponentialToLinear(numUnisonVoices);
		case ParamIndices::UnisonSpread:				return Helpers::RangedFloatToParam(unisonSpread, 0.0f, 0.5f); 
		case ParamIndices::ArpeggioType:				return Helpers::EnumToParam<ArpeggioType>(arpeggioType);
		case ParamIndices::ArpeggioNumOctaves:			return Helpers::RangedIntToParam(arpeggioNumOctaves, 1, 8);
		case ParamIndices::ArpeggioInterval:			return sArpeggioIntervalExponentialToLinear(arpeggioInterval);
		case ParamIndices::ArpeggioNoteDuration:		return sArpeggioIntervalExponentialToLinear(arpeggioNoteDuration);
		}

		return 0.0f;
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
		float masterLevelScalar = Helpers::VolumeToScalar(0.8f);

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

#include <WaveSabreCore/Pandora.h>
#include <WaveSabreCore/Helpers.h>

#include <math.h>

#define M_PI 3.14159265358979323846f
#define M_2_PI 2.0f*3.14159265358979323846f
#define M_PI_2 0.5f*3.14159265358979323846f
#define M_PI_4 0.25f*3.14159265358979323846f

namespace WaveSabreCore
{
	static int gPandoraRandSeed = 1;

	// Returns a random float in -1..1
	static float gPandoraRandFloat()
	{
		gPandoraRandSeed *= 16807;
		return ((float)gPandoraRandSeed) / (float)0x80000000;
	}

	// Returns a random float in 0..1
	static float gPandoraRandFloatNormalized()
	{
		return gPandoraRandFloat() * 0.5f + 0.5f;
	}


	// calculate two amounts that together form a crossfade:
	// - when balance =  0 --> amountA = 1, amountB = 1
	// - when balance = -1 --> amountA = 1, amountB = 0
	// - when balance =  1 --> amountA = 0, amountB = 1
	static void gCrossFade(float balance, float* amountA, float* amountB)
	{
		*amountA = (balance >= 0) ? (1 - balance) : 1;
		*amountB = (balance <= 0) ? (balance + 1) : 1;
	}



	// Crappy pow function,
	// uses 1,0594630943592952645618252949463 as its base.
	// is actually a bit faster than pow(), and gives exactly the same results.
	static __forceinline double gPandoraFastPow(double x)
	{
		//
		// power(base, exponent) = Exp(exponent * Ln(base))
		//
		// base = 1,0594630943592952645618252949463
		// ln(base) = 0,057762265046662109118102676787859
		//
		// power = exp(x * 0,057762265046662109118102676787859);
		//

		x *= 0.057762265046662109118102676787859;

		// e^x = 2^(x*log2(e))   


		__asm
		{
			FLD[x]
			FLDL2E              // y := x*log2e
			FMUL
			FLD		ST(0)       // i := round(y)
			FRNDINT
			FSUB    ST(1), ST   // f := y - i  --> f = fraction of x
			FXCH    ST(1)       // z := 2^f          
			F2XM1
			FLD1
			FADD
			FSCALE              // result := z * 2^i
			FSTP    ST(1)
			FSTP[x]
		}

		return x;
	}


#define PANDORA_VOICE_NOISE_LOOP_SIZE	1024	// in samples (44 samples ~= 1 ms)

	static bool gPandoraNoiseLoopBufferInitialized = false;
	static float gPandoraNoiseLoopBuffer[PANDORA_VOICE_NOISE_LOOP_SIZE];


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
		envelope1 = EnvelopeSettings(0.00005f, 0.00005f, 0.7f, 0.00005f);
		envelope2 = EnvelopeSettings(0.00005f, 0.00005f, 0.7f, 0.00005f);
		envelope3 = EnvelopeSettings(0.00005f, 0.00005f, 0.7f, 0.00005f);
		envelope4 = EnvelopeSettings(0.00005f, 0.00005f, 0.7f, 0.00005f);
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
		unisonSpread = 0.05f;
		arpeggioType = ArpeggioType::OFF;
		arpeggioNumOctaves = 1;
		arpeggioInterval = 16;
		arpeggioNoteDuration = 8;
		midiControlledSettingA = 0.0f;
		midiControlledSettingB = 0.0f;
		midiControlledSettingC = 0.0f;
		midiControlledSettingD = 0.0f;


		VoicesDetune = 1.0f; //< Always use a detune of 1, as that will give each voice a unique detune amount when unisoning. We apply proper spread in the voice.
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
	static __int64 sArpeggioIntervalLinearToExponential(float value)
	{
		return ((__int64)1 << (__int64)(value * 48.0));
	}

	// 1..2^48 --> 0..1
	static float sArpeggioIntervalExponentialToLinear(__int64 value)
	{
		return Helpers::Clamp(floorf(log2f((float)value)) / 48.0f, 0.0f, 1.0f);
	}


	float Pandora::sModulationDepthRangeFactor[3] = { 1.0f, 16.0f, 64.0f };


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
		case ParamIndices::NumUnisonVoices:				VoicesUnisono = sNumUnisonVoicesLinearToExponential(value);  break;
		case ParamIndices::UnisonSpread:				unisonSpread = Helpers::ParamToRangedFloat(value, 0.0f, 0.5f); break;
		case ParamIndices::ArpeggioType:				arpeggioType = Helpers::ParamToEnum<ArpeggioType>(value); break;
		case ParamIndices::ArpeggioNumOctaves:			arpeggioNumOctaves = Helpers::ParamToRangedInt(value, 1, 8); break;
		case ParamIndices::ArpeggioInterval:			arpeggioInterval = sArpeggioIntervalLinearToExponential(value); break;
		case ParamIndices::ArpeggioNoteDuration:		arpeggioNoteDuration = sArpeggioIntervalLinearToExponential(value); break;
		case ParamIndices::Pan:							VoicesPan = value; break;
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
		case ParamIndices::NumUnisonVoices:				return sNumUnisonVoicesExponentialToLinear(VoicesUnisono);
		case ParamIndices::UnisonSpread:				return Helpers::RangedFloatToParam(unisonSpread, 0.0f, 0.5f); 
		case ParamIndices::ArpeggioType:				return Helpers::EnumToParam<ArpeggioType>(arpeggioType);
		case ParamIndices::ArpeggioNumOctaves:			return Helpers::RangedIntToParam(arpeggioNumOctaves, 1, 8);
		case ParamIndices::ArpeggioInterval:			return sArpeggioIntervalExponentialToLinear(arpeggioInterval);
		case ParamIndices::ArpeggioNoteDuration:		return sArpeggioIntervalExponentialToLinear(arpeggioNoteDuration);
		case ParamIndices::Pan:							return VoicesPan;
		}

		return 0.0f;
	}

	void Pandora::Run(double songPosition, float** inputs, float** outputs, int numSamples)
	{
		// render our channel's LFOs
		if (lfo1keysync == LfoSyncType::OFF)
		{
			lfo[0].SetRate(lfo1rate);
			lfo[0].SetWaveform(lfo1waveform);
			lfo[0].SetPositive(lfo1positive);
			lfo[0].Update(numSamples);
		}

		if (lfo2keysync == LfoSyncType::OFF)
		{
			lfo[1].SetRate(lfo2rate);
			lfo[1].SetWaveform(lfo2waveform);
			lfo[1].SetPositive(lfo2positive);
			lfo[1].Update(numSamples);
		}

		if (lfo3keysync == LfoSyncType::OFF)
		{
			lfo[2].SetRate(lfo3rate);
			lfo[2].SetWaveform(lfo3waveform);
			lfo[2].SetPositive(lfo3positive);
			lfo[2].Update(numSamples);
		}

		SynthDevice::Run(songPosition, inputs, outputs, numSamples);
	}


	// LFO

	Pandora::LFO::LFO()
		: time(0), noiseFlip(false), glideSpeed(0), positive(false)
	{
		buffer = new float[(int)Helpers::CurrentSampleRate];

		// randomize output for sample&glide
		output = gPandoraRandFloat();
	}

	Pandora::LFO::~LFO()
	{
		delete[] buffer;
	}


	void Pandora::LFO::Update(int numSamples)
	{
		float* finger = buffer;


		float numSamplesForGlide = 0.5f / rate;

		for (int i = 0; i < numSamples; i++)
		{
			time += rate;

			// normalize time to 0..1 (1 period)
			time -= floorf(time);

			switch (waveform)
			{
			case LfoWaveformType::SQUARE:
				output = (time >= 0.5f) ? -1.0f : 1.0f;
				break;

			case LfoWaveformType::BISQUARE:
			{
				if (time < 0.25f)
					output = 1;
				else if (time >= 0.5f && time < 0.75f)
					output = -1;
				else
					output = 0;
			}
			break;

			case LfoWaveformType::SAW:
				output = (float)(1 - time * 2.0);
				break;

			case LfoWaveformType::TRIANGLE:
			{
				if (time < 0.5)
					output = 1 - (float)(time * 4.0);
				else
					output = -1 + (float)((time - 0.5) * 4.0);
			}
			break;

			case LfoWaveformType::NOISEHOLD:
			case LfoWaveformType::NOISEGLIDE: // no gliding yet
				if ((!noiseFlip && (time < 0.5f)) ||
					(noiseFlip && (time >= 0.5f)))
				{
					noiseFlip = !noiseFlip;
					prevNoise = gPandoraRandFloat();

					// we want to glide from "output" to "prevNoise" in 0.5 time. 
					// since we get "rate" time per sample, we know that 0.5 time units equals 0.5 * (1 /
					// rate) samples = 0.5 / rate
					glideSpeed = (prevNoise - output) / numSamplesForGlide;
				}

				if (waveform == LfoWaveformType::NOISEHOLD)
				{
					output = prevNoise;
				}
				else
				{
					// glide from output to prevNoise. this is unrestricted! 
					output += glideSpeed;
				}

				break;

			case LfoWaveformType::SINE:
			default:
			{
				// cheap sinusoidal LFO
				// from: http://www.musicdsp.org/showArchiveComment.php?ArchiveID=167

				float intermediate = time * 2.0f - 1.0f;

				float absedLFOintermediate = intermediate;

				if (absedLFOintermediate < 0)
					absedLFOintermediate = -absedLFOintermediate;

				output = intermediate * (1.0f - absedLFOintermediate);
			}
			break;
			}

			if (positive)
				*(finger++) = 0.5f * output + 0.5f;
			else
				*(finger++) = output;
		}
	}


	// Envelope

	void Pandora::Envelope::Step(bool gate)
	{
		switch (stage)
		{
		case EnvelopeStage::ATTACK:
		{
			if (!gate)
			{
				stage = EnvelopeStage::RELEASE;
				break;
			}

			level += settings.attackRate;
			if (level >= 1)
			{
				level = 1;
				stage = EnvelopeStage::DECAY;
			}
		}
		break;
		case EnvelopeStage::DECAY:
		{
			if (!gate)
			{
				stage = EnvelopeStage::RELEASE;
				break;
			}

			level -= settings.decayRate;
			if (level <= settings.sustainLevel)
			{
				level = settings.sustainLevel;
				stage = EnvelopeStage::SUSTAIN;
			}
		}
		break;
		case EnvelopeStage::SUSTAIN:
		{
			if (!gate)
				stage = EnvelopeStage::RELEASE;
		}
		break;
		case EnvelopeStage::RELEASE:
		{
			if (!hasEnded)
			{
				level -= settings.releaseRate;
				if (level < 0)
				{
					level = 0;
					hasEnded = true;
				}
			}
		}
		break;
		}
	}


	// Voice

	Pandora::PandoraVoice::PandoraVoice(Pandora* pandora) :
		envelope1(pandora->envelope1),
		envelope2(pandora->envelope2),
		envelope3(pandora->envelope3),
		envelope4(pandora->envelope4)
	{
		this->pandora = pandora;

		if (!gPandoraNoiseLoopBufferInitialized)
		{
			gPandoraNoiseLoopBufferInitialized = true;
			for (int i = 0; i < PANDORA_VOICE_NOISE_LOOP_SIZE; ++i)
				gPandoraNoiseLoopBuffer[i] = gPandoraRandFloat();
		}
	}

	Pandora::PandoraVoice::~PandoraVoice()
	{
		Terminate();
	}

	SynthDevice* Pandora::PandoraVoice::SynthDevice() const
	{
		return pandora;
	}

	float Pandora::PandoraVoice::GetModulationAmountSummed(Pandora::ModulationDestType dest) const
	{
		float result = 0.0f;
		const AllModulationsType<ResolvedModulationType>::ModulationsType& mods = resolvedModulations.modulationsPerDest[(int)dest];
		for (int i = 0; i < mods.Size(); ++i)
			result += (*mods[i].resolvedSource) * (*mods[i].resolvedDepth);
		return result;
	}

	float Pandora::PandoraVoice::GetModulationAmountMultiplied(Pandora::ModulationDestType dest) const
	{
		float result = 1.0f;
		const AllModulationsType<ResolvedModulationType>::ModulationsType& mods = resolvedModulations.modulationsPerDest[(int)dest];
		for (int i = 0; i < mods.Size(); ++i)
			result *= (*mods[i].resolvedSource) * (*mods[i].resolvedDepth);
		return result;
	}

	void Pandora::PandoraVoice::Run(double songPosition, float** outputs, int numSamples)
	{
		float masterLevelScalar = Helpers::VolumeToScalar(0.8f);

		float leftPanScalar = Helpers::PanToScalarLeft(Pan);
		float rightPanScalar = Helpers::PanToScalarRight(Pan);

		bool should_be_on = IsOn;


		float currentVolumeLevel = masterLevelScalar * inputVelocity;

		float filteroutput, filter2output, vcaoutput;

		// Determine lfo source buffers
		LFO* lfo1, *lfo2, *lfo3;

		if (hasOwnLFO[0])
		{
			lfo1 = lfo[0];
			lfo1->SetWaveform(pandora->lfo1waveform);
			lfo1->SetPositive(pandora->lfo1positive);

			if (!resolvedModulations.IsAffecting(ModulationDestType::LFO1RATE))
			{
				lfo1->SetRate(pandora->lfo1rate);
				lfo1->Update(numSamples);
			}
		}
		else
		{
			lfo1 = &pandora->lfo[0];
		}

		if (hasOwnLFO[1])
		{
			lfo2 = lfo[1];
			lfo2->SetWaveform(pandora->lfo2waveform);
			lfo2->SetPositive(pandora->lfo2positive);

			if (!resolvedModulations.IsAffecting(ModulationDestType::LFO2RATE))
			{
				lfo2->SetRate(pandora->lfo2rate);
				lfo2->Update(numSamples);
			}
		}
		else
		{
			lfo2 = &pandora->lfo[1];
		}

		if (hasOwnLFO[2])
		{
			lfo3 = lfo[2];
			lfo3->SetWaveform(pandora->lfo3waveform);
			lfo3->SetPositive(pandora->lfo3positive);

			if (!resolvedModulations.IsAffecting(ModulationDestType::LFO3RATE))
			{
				lfo3->SetRate(pandora->lfo3rate);
				lfo3->Update(numSamples);
			}
		}
		else
		{
			lfo3 = &pandora->lfo[2];
		}

		float* lfoBuffer1 = lfo1->GetBuffer();
		float* lfoBuffer2 = lfo2->GetBuffer();
		float* lfoBuffer3 = lfo3->GetBuffer();

		// filter helper variables
		const float mq1 = cosf(powf(pandora->vcf1Resonance, 0.1f) * (float)M_PI_2);
		const float mq1nrm = sqrtf(mq1 + 0.01f);
		const float mq2 = cosf(powf(pandora->vcf2Resonance, 0.1f) * (float)M_PI_2);
		const float mq2nrm = sqrtf(mq2 + 0.01f);

		// calculate weights for osc and string
		float submixWeightOsc, submixWeightString;
		gCrossFade(pandora->oscStringMix, &submixWeightOsc, &submixWeightString);


		for (int i = 0; i < numSamples; i++)
		{
			//
			// general flow:
			// 
			// osc1 --+
			//        |--> filters (2 max) --> vca --> out
			// osc2 --+
			//

			// -- update modulators --

			// ENVELOPES
			{
				if (resolvedModulations.IsDependingOn(ModulationSourceType::ENV1))
					envelope1.Step(gate);

				if (resolvedModulations.IsDependingOn(ModulationSourceType::ENV2))
					envelope2.Step(gate);

				if (resolvedModulations.IsDependingOn(ModulationSourceType::ENV3))
					envelope3.Step(gate);

				if (resolvedModulations.IsDependingOn(ModulationSourceType::ENV4))
					envelope4.Step(gate);
			}

			// LFOs
			{
				// LFO1
				if (resolvedModulations.IsDependingOn(ModulationSourceType::LFO1))
				{
					// LFO1 RATE MODULATION
					if (resolvedModulations.IsAffecting(ModulationDestType::LFO1RATE))
					{
						float modulation = GetModulationAmountSummed(ModulationDestType::LFO1RATE);

						lfo1->SetRate(pandora->lfo1rate * powf(10000000.0f, modulation));
						lfo1->Update(1);

						currentLfo1Amount = *lfoBuffer1;
					}
					else
					{
						currentLfo1Amount = *lfoBuffer1;
						++lfoBuffer1;
					}
				}

				// LFO2
				if (resolvedModulations.IsDependingOn(ModulationSourceType::LFO2))
				{
					// LFO2 RATE MODULATION
					if (resolvedModulations.IsAffecting(ModulationDestType::LFO2RATE))
					{
						float modulation = GetModulationAmountSummed(ModulationDestType::LFO2RATE);

						lfo2->SetRate(pandora->lfo2rate * powf(10000000.0f, modulation));
						lfo2->Update(1);

						currentLfo2Amount = *lfoBuffer2;
					}
					else
					{
						currentLfo2Amount = *lfoBuffer2;
						++lfoBuffer2;
					}
				}

				// LFO3
				if (resolvedModulations.IsDependingOn(ModulationSourceType::LFO3))
				{
					// LFO2 RATE MODULATION
					if (resolvedModulations.IsAffecting(ModulationDestType::LFO3RATE))
					{
						float modulation = GetModulationAmountSummed(ModulationDestType::LFO3RATE);

						lfo3->SetRate(pandora->lfo3rate * powf(10000000.0f, modulation));
						lfo3->Update(1);

						currentLfo3Amount = *lfoBuffer3;
					}
					else
					{
						currentLfo3Amount = *lfoBuffer3;
						++lfoBuffer3;
					}
				}
			}

			// Modulated modulation depth
			modulatedModulationDepth[0] = GetModulationAmountMultiplied(ModulationDestType::MODDEPTHA);
			modulatedModulationDepth[1] = GetModulationAmountMultiplied(ModulationDestType::MODDEPTHB);
			modulatedModulationDepth[2] = GetModulationAmountMultiplied(ModulationDestType::MODDEPTHC);
			modulatedModulationDepth[3] = GetModulationAmountMultiplied(ModulationDestType::MODDEPTHD);

			// -- update modulation consumers --

			// OSC 1
			{
				float pulseWidth = pandora->osc1pulseWidth + GetModulationAmountSummed(ModulationDestType::OSC1PULSEWIDTH);

				osc1level = SampleOscillator(osc1time, pandora->osc1waveform, pulseWidth);
			}

			// OSC1 LEVEL MODULATION
			{
				osc1level *= GetModulationAmountMultiplied(ModulationDestType::OSC1LEVEL);
			}

			// OSC MIXER
			float submix = pandora->mixAmountOsc1 * osc1level;

			if (pandora->osc2waveform != OscWaveformType::OFF)
			{
				// OSC 2
				{
					float pulseWidth = pandora->osc2pulseWidth + GetModulationAmountSummed(ModulationDestType::OSC2PULSEWIDTH);

					osc2level = SampleOscillator(osc2time, pandora->osc2waveform, pulseWidth);
				}

				// OSC2 LEVEL MODULATION
				{
					osc2level *= GetModulationAmountMultiplied(ModulationDestType::OSC2LEVEL);
				}

				submix += pandora->mixAmountOsc2 * osc2level;
			}

			if (pandora->osc3waveform != OscWaveformType::OFF)
			{
				// OSC 3
				{
					float pulseWidth = pandora->osc3pulseWidth + GetModulationAmountSummed(ModulationDestType::OSC3PULSEWIDTH);

					osc3level = SampleOscillator(osc3time, pandora->osc3waveform, pulseWidth);
				}

				// OSC3 LEVEL MODULATION
				{
					osc3level *= GetModulationAmountMultiplied(ModulationDestType::OSC3LEVEL);
				}

				submix += pandora->mixAmountOsc3 * osc3level;
			}

			// STRING
			float stringLevelModulation = submixWeightString * pandora->stringLevel;

			if (stringLevelModulation > 0)
			{
				// STRING LEVEL MODULATION
				stringLevelModulation *= GetModulationAmountMultiplied(ModulationDestType::STRINGLEVEL);
			}

			bool enableString = (stringLevelModulation > 0.0f);

			// FILTERING
			{
				if (pandora->vcfRouting != FilterRoutingType::NONE)
				{
					float vcf1cutoff;

					// FILTER 1
					{
						// cutoff should be (0, 1]
						vcf1cutoff = pandora->vcf1Cutoff;
						float resonance = pandora->vcf1Resonance;

						// VCF1 CUTOFF MODULATION
						vcf1cutoff += GetModulationAmountSummed(ModulationDestType::VCF1CUTOFF);
						
						// VCF1 RESO MODULATION
						resonance += GetModulationAmountSummed(ModulationDestType::VCF1RESONANCE);

						vcf1cutoff = Helpers::Clamp(vcf1cutoff, 0.002f, 0.7f);

						float mf = 2.0f * sinf(M_PI_4 * vcf1cutoff);

						float filter1high = submix * mq1nrm - filter1low - resonance * filter1band;
						filter1band += mf * filter1high;
						filter1low += mf * filter1band;

						switch (pandora->vcf1type)
						{
						case FilterType::LPF:
							filteroutput = filter1low;
							break;
						case FilterType::BPF:
							filteroutput = filter1band;
							break;
						case FilterType::HPF:
							filteroutput = filter1high;
							break;
						case FilterType::ALLPASS:
							filteroutput = filter1low - filter1high;
							break;
						case FilterType::NOTCH:
							filteroutput = filter1low + filter1high;
							break;
						}
					}

					//// FILTER DISTORTION
					//if (pandora->doFilterDist)
					//{
					//	float resultL;

					//	filterDistortion.drive = pandora->filterDistDrive;
					//	filterDistortion.shape = pandora->filterDistShape;
					//	filterDistortion.onlySoftClip = pandora->filterDistPassive;

					//	// Hook up quite inefficient distortion...
					//	filterDistortion.RenderToBuffer(1, &filteroutput, &resultL);

					//	filteroutput = resultL;
					//}


					if ((int)pandora->vcfRouting > (int)FilterRoutingType::SINGLE)
					{
						float filterInput;

						if (pandora->vcfRouting == FilterRoutingType::SERIAL)
						{
							filterInput = filteroutput;

							//// Render our string with the result of the first filter and add it to the input of our second filter!
							//if (enableString)
							//	filterInput = filterInput * submixWeightOsc + stringHelper.RenderSample(filteroutput) * stringLevelModulation;
						}
						else
						{
							filterInput = submix;
						}

						// FILTER 2
						{
							// cutoff should be (0, 1]
							float cutoff = pandora->vcf2Cutoff;
							float resonance = pandora->vcf2Resonance;

							// use relative cutoff ?
							if (pandora->vcf2CutoffRelative)
							{
								cutoff += vcf1cutoff;
							}


							// VCF2 CUTOFF MODULATION
							cutoff += GetModulationAmountSummed(ModulationDestType::VCF2CUTOFF);

							// VCF2 RESO MODULATION
							resonance += GetModulationAmountSummed(ModulationDestType::VCF2RESONANCE);


							cutoff = Helpers::Clamp(vcf1cutoff, 0.002f, 0.7f);


							float mf = 2.0f * sinf(M_PI_4 * cutoff);

							float filter2high = filterInput * mq2nrm - filter2low - resonance * filter2band;
							filter2band += mf * filter2high;
							filter2low += mf * filter2band;

							switch (pandora->vcf2type)
							{
							case FilterType::LPF:
								filter2output = filter2low;
								break;
							case FilterType::BPF:
								filter2output = filter2band;
								break;
							case FilterType::HPF:
								filter2output = filter2high;
								break;
							case FilterType::ALLPASS:
								filter2output = filter2low - filter2high;
								break;
							case FilterType::NOTCH:
								filter2output = filter2low + filter2high;
								break;
							}
						}

						if (pandora->vcfRouting == FilterRoutingType::SERIAL)
						{
							filteroutput = filter2output;
						}
						else
						{
							filteroutput = pandora->vcf1amountParallel * filteroutput +
								pandora->vcf2amountParallel * filter2output;
						}
					}


				}
				else // no filters at all
				{
					filteroutput = submix;
				}
			}

			// Render string with results from filters (either none, just one, or two in parallel)
			//if (enableString && pandora->vcfRouting != FilterRoutingType::SERIAL)
			//	filteroutput = filteroutput * submixWeightOsc + stringHelper.RenderSample(filteroutput) * stringLevelModulation;

			// VCA
			{
				vcaoutput = filteroutput;
				vcaLevel = 0;

				// VCA MODULATION
				if (resolvedModulations.IsAffecting(ModulationDestType::VCA))
					vcaoutput *= GetModulationAmountSummed(ModulationDestType::VCA);
			}

			double osc1timeModifier = 1, osc2timeModifier = 1, osc3timeModifier = 1;

			// OSC1 TUNE MODULATION
			{
				if (resolvedModulations.IsAffecting(ModulationDestType::OSC1TUNE))
				{
					float modulation = GetModulationAmountSummed(ModulationDestType::OSC1TUNE);

					osc1timeModifier *= gPandoraFastPow(modulation);
				}
			}

			// OSC2 TUNE MODULATION
			{
				if (resolvedModulations.IsAffecting(ModulationDestType::OSC2TUNE))
				{
					float modulation = GetModulationAmountSummed(ModulationDestType::OSC2TUNE);

					osc2timeModifier *= gPandoraFastPow(modulation);
				}
			}

			// OSC3 TUNE MODULATION
			{
				if (resolvedModulations.IsAffecting(ModulationDestType::OSC3TUNE))
				{
					float modulation = GetModulationAmountSummed(ModulationDestType::OSC3TUNE);

					osc3timeModifier *= gPandoraFastPow(modulation);
				}
			}

			// OUTPUT SAMPLES
			outputs[0][i] += currentVolumeLevel * vcaoutput * leftPanScalar; // left
			outputs[1][i] += currentVolumeLevel * vcaoutput * rightPanScalar; // right

			// note sliding
			double slideScalar1 = 1.0;/* + slideInitialModifierOsc1 * slideAmount;*/
			double slideScalar2 = 1.0;/* + slideInitialModifierOsc2 * slideAmount;*/
			double slideScalar3 = 1.0;/* + slideInitialModifierOsc3 * slideAmount;*/

			// increment time
			osc1time += Helpers::Max(osc1timestep * osc1timeModifier * slideScalar1, 0);
			osc2time += Helpers::Max(osc2timestep * osc2timeModifier * slideScalar2, 0);
			osc3time += Helpers::Max(osc3timestep * osc3timeModifier * slideScalar3, 0);

			// sync osc2?
			if (osc1time >= 1.0f && pandora->osc2sync)
			{
				osc2time = 0;
			}
			else
			{
				osc2time -= floorf((float)osc2time);
			}

			// wrap to -1..1 range
			osc1time -= floorf((float)osc1time);
			osc3time -= floorf((float)osc3time);

			//slideAmount *= linkedPatch->slideSpeed;

			//slideAmount *= pandora->slideSpeed;
			if (should_be_on && !gate && vcaLevel <= 0.001f)
				should_be_on = false;
		}

		if (!should_be_on)
			Terminate();
	}

	void Pandora::PandoraVoice::NoteOn(int note, int velocity, float detune, float pan)
	{
		Voice::NoteOn(note, velocity, detune, pan);

		const PandoraVoice* slideFrom = NULL; //< Support later?

		gate = true;
		inputVelocity = (float)velocity / 127.0f;

		filter1low = 0.0f;
		filter1band = 0.0f;
		filter2low = 0.0f;
		filter2band = 0.0f;
		osc1level = 0.0f;
		osc2level = 0.0f;
		osc3level = 0.0f;
		vcaLevel = 1.0f;
		currentLfo1Amount = 0;
		currentLfo2Amount = 0;
		currentLfo3Amount = 0;

		double factor = (410.96047696981869831451220512851 / (double)Helpers::CurrentSampleRate) / M_2_PI;
		factor /= 100.0f; // magic scaling function

		float unisonDetune = (2.0f * detune - 1.0f) * pandora->unisonSpread;

		osc1timestep = factor * powf(1.0594630943592952645618252949463f, (float)(note + pandora->osc1baseToneTranspose + pandora->osc1finetune) + unisonDetune);
		osc2timestep = factor * powf(1.0594630943592952645618252949463f, (float)(note + pandora->osc2baseToneTranspose + pandora->osc2finetune) + unisonDetune);
		osc3timestep = factor * powf(1.0594630943592952645618252949463f, (float)(note + pandora->osc3baseToneTranspose + pandora->osc3finetune) + unisonDetune);

		for (int i = 0; i < 4; ++i)
			modulatedModulationDepth[i] = 0.0f;

		// start with a random time
		osc1time = (double)gPandoraRandFloatNormalized();
		osc2time = (double)gPandoraRandFloatNormalized();
		osc3time = (double)gPandoraRandFloatNormalized();

		noiseLoopOffset = (int)floorf(gPandoraRandFloatNormalized() * (float)PANDORA_VOICE_NOISE_LOOP_SIZE);

		ResolveAllModulations();

		// Init some LFO's
		hasOwnLFO[0] = pandora->lfo1keysync != LfoSyncType::OFF || resolvedModulations.IsAffecting(ModulationDestType::LFO1RATE);
		hasOwnLFO[1] = pandora->lfo2keysync != LfoSyncType::OFF || resolvedModulations.IsAffecting(ModulationDestType::LFO2RATE);
		hasOwnLFO[2] = pandora->lfo3keysync != LfoSyncType::OFF || resolvedModulations.IsAffecting(ModulationDestType::LFO3RATE);

		lfo[0] = NULL;
		lfo[1] = NULL;
		lfo[2] = NULL;

		if (hasOwnLFO[0])
		{
			lfo[0] = new LFO();

			if (pandora->lfo1keysync == LfoSyncType::OFF)
				lfo[0]->SyncTo(&pandora->lfo[0]);
			//else if (pandora->lfo1keysync == LfoSyncType::GATE && slideFrom != NULL)
			//	lfo[0]->SyncTo((slideFrom->lfo[0] != NULL) ? slideFrom->lfo[0] : ownerChannel->GetLFO(0));
		}

		if (hasOwnLFO[1])
		{
			lfo[1] = new LFO();

			if (pandora->lfo2keysync == LfoSyncType::OFF)
				lfo[1]->SyncTo(&pandora->lfo[1]);
			//else if (pandora->lfo2keysync == Patch::PANDORA_PATCH_LFOSYNC_GATE && slideFrom != NULL)
			//	lfo[1]->SyncTo((slideFrom->lfo[1] != NULL) ? slideFrom->lfo[1] : ownerChannel->GetLFO(1));
		}

		if (hasOwnLFO[2])
		{
			lfo[2] = new LFO();

			if (pandora->lfo3keysync == LfoSyncType::OFF)
				lfo[2]->SyncTo(&pandora->lfo[2]);
			//else if (pandora->lfo3keysync == Patch::PANDORA_PATCH_LFOSYNC_GATE && slideFrom != NULL)
			//	lfo[2]->SyncTo((slideFrom->lfo[2] != NULL) ? slideFrom->lfo[2] : ownerChannel->GetLFO(2));
		}


		// Pluck our string.
		//stringHelper.SetTimeStep(osc1timestep);
		//stringHelper.Pluck(inputVelocity);

		// Init filterdistortion filter
		//filterDistortion.filter.cutoff = 1.0f;
		//filterDistortion.filter.resonance = 1.0f;
		//filterDistortion.filter.UpdateCoeffs();
	}

	void Pandora::PandoraVoice::NoteOff()
	{
		gate = false;
	}

	void Pandora::PandoraVoice::Terminate()
	{
		if (!IsOn)
			return;

		IsOn = false;
		delete lfo[0];
		delete lfo[1];
		delete lfo[2];
	}

	void Pandora::PandoraVoice::ResolveModulations(
		FixedSizeList<Pandora::ResolvedModulationType>& dest,
		FixedSizeList<Pandora::UnresolvedModulationType>& src)
	{
		dest.AddMultipleNoninitialized(src.Size());

		for (int i = 0; i < src.Size(); ++i)
		{
			UnresolvedModulationType& srcMod = src[i];
			ResolvedModulationType& resMod = dest[i];

			ResolveModulation(resMod, srcMod);
		}
	}

	void Pandora::PandoraVoice::ResolveModulation(Pandora::ResolvedModulationType& dest, Pandora::UnresolvedModulationType& src)
	{
		// resolve depth to either constant or modulated float
		if (src.depthSource == ModulationDepthSourceType::CONSTANT)
		{
			dest.constantDepth = src.constantDepth * sModulationDepthRangeFactor[(int)src.constantDepthRange];
			dest.resolvedDepth = &dest.constantDepth;
		}
		else
		{
			ASSERT((int)srcMod.depthSource < sizeof(modulatedModulationDepth) / sizeof(float));
			dest.constantDepth = 0.0f;
			dest.resolvedDepth = &modulatedModulationDepth[(int)src.depthSource];
		}

		// resolve source to correct provider
		switch (src.source)
		{
		case ModulationSourceType::ENV1:
			dest.resolvedSource = envelope1.GetLevelPtr();
			break;
		case ModulationSourceType::ENV2:
			dest.resolvedSource = envelope2.GetLevelPtr();
			break;
		case ModulationSourceType::ENV3:
			dest.resolvedSource = envelope3.GetLevelPtr();
			break;
		case ModulationSourceType::ENV4:
			dest.resolvedSource = envelope4.GetLevelPtr();
			break;
		case ModulationSourceType::LFO1:
			dest.resolvedSource = &currentLfo1Amount;
			break;
		case ModulationSourceType::LFO2:
			dest.resolvedSource = &currentLfo2Amount;
			break;
		case ModulationSourceType::LFO3:
			dest.resolvedSource = &currentLfo3Amount;
			break;
		case ModulationSourceType::OSC1:
			dest.resolvedSource = &osc1level;
			break;
		case ModulationSourceType::OSC2:
			dest.resolvedSource = &osc2level;
			break;
		case ModulationSourceType::OSC3:
			dest.resolvedSource = &osc3level;
			break;
		case ModulationSourceType::MIDICTRL_A:
			dest.resolvedSource = &pandora->midiControlledSettingA;
			break;
		case ModulationSourceType::MIDICTRL_B:
			dest.resolvedSource = &pandora->midiControlledSettingB;
			break;
		case ModulationSourceType::MIDICTRL_C:
			dest.resolvedSource = &pandora->midiControlledSettingC;
			break;
		case ModulationSourceType::MIDICTRL_D:
			dest.resolvedSource = &pandora->midiControlledSettingD;
			break;
		default:			
			ASSERT_MSG(false, "Unknown modulation source found. Smells like a case of bad coding!");
			break;
		}

	}

	void Pandora::PandoraVoice::ResolveAllModulations()
	{
		for (int i = 0; i < (int)ModulationDestType::COUNT; ++i)
			ResolveModulations(resolvedModulations.modulationsPerDest[i], pandora->modulations.modulationsPerDest[i]);

		resolvedModulations.usedSourcesMask = pandora->modulations.usedSourcesMask;
	}


	__inline float Pandora::PandoraVoice::SampleOscillator(double time, OscWaveformType waveform, float pulseWidth)
	{
		static int noiseSeed = 1;

		double origTime = time;

		// Remap time
		float t2 = 0.5f * pulseWidth + 0.5f;

		if (time < t2)
		{
			time = (time / t2) * 0.5f;
		}
		else
		{
			time = 0.5f + ((time - t2) / (1 - t2)) * 0.5f;
		}


		switch (waveform)
		{
		case OscWaveformType::SINE:
			return sinf((float)time * 6.283185307179586476925286766559f);

		case OscWaveformType::SQUARE:
			return (time >= 0.5) ? -1.0f : 1.0f;

		case OscWaveformType::BISQUARE:
		{
			double w = 0.25 * (pulseWidth + 1);

			if (origTime < w)
				return 1;
			else if (origTime >= 0.5 && origTime < 0.5 + w)
				return -1;
			else
				return 0;

		}

		case OscWaveformType::SAW:
			return (float)((time * 2.0) - 1);

		case OscWaveformType::TRIANGLE:
		{
			if (time < 0.5)
				return 1 - (float)(time * 4.0);
			else
				return -1 + (float)((time - 0.5) * 4.0);
		}
		break;

		case OscWaveformType::NOISE:
			return gPandoraRandFloat();
			break;

		case OscWaveformType::NOISE_LOOP:
			noiseLoopOffset = (noiseLoopOffset + 1) % PANDORA_VOICE_NOISE_LOOP_SIZE;
			return gPandoraNoiseLoopBuffer[noiseLoopOffset];

		default:
		case OscWaveformType::OFF:
			return 0;
		}
	}

} // namespace WaveSabreCore

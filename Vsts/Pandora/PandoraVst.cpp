#include "PandoraVst.h"
#include "PandoraEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

AudioEffect *createEffectInstance(audioMasterCallback audioMaster)
{
	Helpers::Init();
	return new PandoraVst(audioMaster);
}

PandoraVst::PandoraVst(audioMasterCallback audioMaster)
	: VstPlug(audioMaster, (int)Pandora::ParamIndices::NumParams, 0, 2, 'Pndr', new Pandora(), true)
{
	setEditor(new PandoraEditor(this));
}

void PandoraVst::getParameterName(VstInt32 index, char *text)
{
	switch ((Pandora::ParamIndices)index)
	{
	case Pandora::ParamIndices::Osc1waveform:				vst_strncpy(text, "Osc1wave", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc2waveform:				vst_strncpy(text, "Osc2wave", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc3waveform:				vst_strncpy(text, "Osc3wave", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc1baseToneTranspose:		vst_strncpy(text, "Osc1base", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc2baseToneTranspose:		vst_strncpy(text, "Osc2base", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc3baseToneTranspose:		vst_strncpy(text, "Osc3base", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc1finetune:				vst_strncpy(text, "Osc1fine", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc2finetune:				vst_strncpy(text, "Osc2fine", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc3finetune:				vst_strncpy(text, "Osc3fine", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc1pulseWidth:				vst_strncpy(text, "Osc1pw", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc2pulseWidth:				vst_strncpy(text, "Osc2pw", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc3pulseWidth:				vst_strncpy(text, "Osc3pw", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::MixAmountOsc1:				vst_strncpy(text, "Osc1lvl", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::MixAmountOsc2:				vst_strncpy(text, "Osc1lv2", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::MixAmountOsc3:				vst_strncpy(text, "Osc1lv3", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::StringDamping:				vst_strncpy(text, "StrDamp", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::StringFeedbackAmount:		vst_strncpy(text, "StrFb", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::StringFeedbackDelay:		vst_strncpy(text, "StrFbDl", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::StringThickness:			vst_strncpy(text, "StrThck", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::StringLevel:				vst_strncpy(text, "StrLvl", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::OscStringMix:				vst_strncpy(text, "OscStrMx", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc2sync:					vst_strncpy(text, "Osc2sync", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo1rate:					vst_strncpy(text, "Lfo1rate", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo2rate:					vst_strncpy(text, "Lfo2rate", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo3rate:					vst_strncpy(text, "Lfo3rate", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo1waveform:				vst_strncpy(text, "Lfo1wave", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo2waveform:				vst_strncpy(text, "Lfo2wave", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo3waveform:				vst_strncpy(text, "Lfo3wave", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo1keysync:				vst_strncpy(text, "Lfo1sync", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo2keysync:				vst_strncpy(text, "Lfo2sync", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo3keysync:				vst_strncpy(text, "Lfo3sync", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo1positive:				vst_strncpy(text, "Lfo1pos", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo2positive:				vst_strncpy(text, "Lfo2pos", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo3positive:				vst_strncpy(text, "Lfo3pos", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope1attackRate:		vst_strncpy(text, "Env1att", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope1decayRate:			vst_strncpy(text, "Env1dec", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope1sustainLevel:		vst_strncpy(text, "Env1sus", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope1releaseRate:		vst_strncpy(text, "Env1rel", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope2attackRate:		vst_strncpy(text, "Env2att", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope2decayRate:			vst_strncpy(text, "Env2dec", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope2sustainLevel:		vst_strncpy(text, "Env2sus", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope2releaseRate:		vst_strncpy(text, "Env2rel", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope3attackRate:		vst_strncpy(text, "Env3att", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope3decayRate:			vst_strncpy(text, "Env3dec", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope3sustainLevel:		vst_strncpy(text, "Env3sus", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope3releaseRate:		vst_strncpy(text, "Env3rel", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope4attackRate:		vst_strncpy(text, "Env4att", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope4decayRate:			vst_strncpy(text, "Env4dec", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope4sustainLevel:		vst_strncpy(text, "Env4sus", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope4releaseRate:		vst_strncpy(text, "Env4rel", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::VcfRouting:					vst_strncpy(text, "VcfRout", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Vcf1type:					vst_strncpy(text, "Vcf1type", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Vcf1Cutoff:					vst_strncpy(text, "Vcf1Cut", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Vcf1Resonance:				vst_strncpy(text, "Vcf1Reso", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Vcf2type:					vst_strncpy(text, "Vcf2type", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Vcf2Cutoff:					vst_strncpy(text, "Vcf2Cut", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Vcf2Resonance:				vst_strncpy(text, "Vcf2Reso", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::VcfCtrlBalance:				vst_strncpy(text, "VcfBal", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Vcf2CutoffRelative:			vst_strncpy(text, "Vcf2CutR", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::VcfDistType:				vst_strncpy(text, "VDist", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::FilterDistDrive:			vst_strncpy(text, "VDistDrv", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::FilterDistShape:			vst_strncpy(text, "VDistShp", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::DoSlide:					vst_strncpy(text, "DoSlide", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::SlideSpeed:					vst_strncpy(text, "SlideSpd", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::NumUnisonVoices:			vst_strncpy(text, "NumUniso", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::UnisonSpread:				vst_strncpy(text, "UnisonSp", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::ArpeggioType:				vst_strncpy(text, "ArpType", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::ArpeggioNumOctaves:			vst_strncpy(text, "ArpOcta", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::ArpeggioInterval:			vst_strncpy(text, "ArpIntr", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::ArpeggioNoteDuration:		vst_strncpy(text, "ArpNDur", kVstMaxParamStrLen); break;

		// Todo: Add names for modulation parameters
	}
}


const char* gStrOscWaveformType[(int)Pandora::OscWaveformType::COUNT] = {
	"OFF",
	"SINE",
	"SQUARE",
	"SAW",
	"TRIANGLE",
	"NOISE",
	"BISQUARE",
	"NOISE_LOOP"
};

const char* gStrLfoWaveformType[(int)Pandora::LfoWaveformType::COUNT] = {
	"SINE",
	"SQUARE",
	"BISQUARE",
	"SAW",
	"TRIANGLE",
	"NOISEHOLD",
	"NOISEGLIDE"
};

const char* gStrFilterType[(int)Pandora::FilterType::COUNT] = {
	"LPF",
	"BPF",
	"HPF",
	"ALLPASS",
	"NOTCH"
};

const char* gStrFilterRoutingType[(int)Pandora::FilterRoutingType::COUNT] = {
	"NONE",
	"SINGLE",
	"SERIAL",
	"PARALLEL"
};

const char* gStrLfoSyncType[(int)Pandora::LfoSyncType::COUNT] = {
	"OFF",
	"KEY",
	"GATE"
};

const char* gStrArpeggioType[(int)Pandora::ArpeggioType::COUNT] = {
	"OFF",
	"UP",
	"DOWN",
	"UPDOWN"
};

const char* gStrModulationSourceType[(int)Pandora::ModulationSourceType::COUNT] = {
	"ENV1",
	"ENV2",
	"ENV3",
	"ENV4",
	"LFO1",
	"LFO2",
	"LFO3",
	"OSC1",
	"OSC2",
	"OSC3",
	"MIDICTRL_A",
	"MIDICTRL_B",
	"MIDICTRL_C",
	"MIDICTRL_D"
};

const char* gStrModulationDepthSourceType[(int)Pandora::ModulationDepthSourceType::COUNT] = {
	"CONSTANT",
	"A",
	"B",
	"C",
	"D"
};

const char* gStrModulationDepthRange[(int)Pandora::ModulationDepthRange::COUNT] = {
	"1",
	"16",
	"64"
};


void PandoraVst::getParameterDisplayAndLabel(VstInt32 index, char* display, char* label)
{
	switch ((Echo::ParamIndices)index)
	{
	case Pandora::ParamIndices::Osc1waveform:				setParameterDisplayAndLabel(display, label, gStrOscWaveformType[(int)((Pandora*)getDevice())->osc1waveform]); break;
	case Pandora::ParamIndices::Osc2waveform:				setParameterDisplayAndLabel(display, label, gStrOscWaveformType[(int)((Pandora*)getDevice())->osc2waveform]); break;
	case Pandora::ParamIndices::Osc3waveform:				setParameterDisplayAndLabel(display, label, gStrOscWaveformType[(int)((Pandora*)getDevice())->osc3waveform]); break;
	case Pandora::ParamIndices::Osc1baseToneTranspose:		setParameterDisplayAndLabel(display, label, (int)((Pandora*)getDevice())->osc1baseToneTranspose); break;
	case Pandora::ParamIndices::Osc2baseToneTranspose:		setParameterDisplayAndLabel(display, label, (int)((Pandora*)getDevice())->osc2baseToneTranspose); break;
	case Pandora::ParamIndices::Osc3baseToneTranspose:		setParameterDisplayAndLabel(display, label, (int)((Pandora*)getDevice())->osc3baseToneTranspose); break;
	case Pandora::ParamIndices::Osc1finetune:				setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->osc1finetune, 2); break;  
	case Pandora::ParamIndices::Osc2finetune:				setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->osc2finetune, 2); break; 
	case Pandora::ParamIndices::Osc3finetune:				setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->osc3finetune, 2); break; 
	case Pandora::ParamIndices::Osc1pulseWidth:				setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->osc1pulseWidth, 2); break;
	case Pandora::ParamIndices::Osc2pulseWidth:				setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->osc2pulseWidth, 2); break;
	case Pandora::ParamIndices::Osc3pulseWidth:				setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->osc3pulseWidth, 2); break;
	case Pandora::ParamIndices::MixAmountOsc1:				setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->mixAmountOsc1, 2); break;
	case Pandora::ParamIndices::MixAmountOsc2:				setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->mixAmountOsc2, 2); break;
	case Pandora::ParamIndices::MixAmountOsc3:				setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->mixAmountOsc3, 2); break;
	case Pandora::ParamIndices::StringDamping:				setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->stringDamping, 2); break;
	case Pandora::ParamIndices::StringFeedbackAmount:		setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->stringFeedbackAmount, 2); break;
	case Pandora::ParamIndices::StringFeedbackDelay:		setParameterDisplayAndLabel(display, label, (int)((Pandora*)getDevice())->stringFeedbackDelay, " smpls"); break;
	case Pandora::ParamIndices::StringThickness:			setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->stringThickness, 2); break;
	case Pandora::ParamIndices::StringLevel:				setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->stringLevel, 2); break;
	case Pandora::ParamIndices::OscStringMix:				setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->oscStringMix, 2); break;
	case Pandora::ParamIndices::Osc2sync:					setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->osc2sync); break;
	case Pandora::ParamIndices::Lfo1rate:					setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->lfo1rate, 5); break; 
	case Pandora::ParamIndices::Lfo2rate:					setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->lfo2rate, 5); break; 
	case Pandora::ParamIndices::Lfo3rate:					setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->lfo3rate, 5); break; 
	case Pandora::ParamIndices::Lfo1waveform:				setParameterDisplayAndLabel(display, label, gStrLfoWaveformType[(int)((Pandora*)getDevice())->lfo1waveform]); break;
	case Pandora::ParamIndices::Lfo2waveform:				setParameterDisplayAndLabel(display, label, gStrLfoWaveformType[(int)((Pandora*)getDevice())->lfo2waveform]); break;
	case Pandora::ParamIndices::Lfo3waveform:				setParameterDisplayAndLabel(display, label, gStrLfoWaveformType[(int)((Pandora*)getDevice())->lfo3waveform]); break;
	case Pandora::ParamIndices::Lfo1keysync:				setParameterDisplayAndLabel(display, label, gStrLfoSyncType[(int)((Pandora*)getDevice())->lfo1keysync]); break;
	case Pandora::ParamIndices::Lfo2keysync:				setParameterDisplayAndLabel(display, label, gStrLfoSyncType[(int)((Pandora*)getDevice())->lfo2keysync]); break;
	case Pandora::ParamIndices::Lfo3keysync:				setParameterDisplayAndLabel(display, label, gStrLfoSyncType[(int)((Pandora*)getDevice())->lfo3keysync]); break;
	case Pandora::ParamIndices::Lfo1positive:				setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->lfo1positive); break;
	case Pandora::ParamIndices::Lfo2positive:				setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->lfo2positive); break;
	case Pandora::ParamIndices::Lfo3positive:				setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->lfo3positive); break;
	case Pandora::ParamIndices::Envelope1attackRate:		setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->envelope1.attackRate, 5); break; 
	case Pandora::ParamIndices::Envelope1decayRate:			setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->envelope1.decayRate, 5); break; 
	case Pandora::ParamIndices::Envelope1sustainLevel:		setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->envelope1.sustainLevel, 2); break;
	case Pandora::ParamIndices::Envelope1releaseRate:		setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->envelope1.releaseRate, 5); break; 
	case Pandora::ParamIndices::Envelope2attackRate:		setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->envelope2.attackRate, 5); break;
	case Pandora::ParamIndices::Envelope2decayRate:			setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->envelope2.decayRate, 5); break;
	case Pandora::ParamIndices::Envelope2sustainLevel:		setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->envelope2.sustainLevel, 2); break;
	case Pandora::ParamIndices::Envelope2releaseRate:		setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->envelope2.releaseRate, 5); break;
	case Pandora::ParamIndices::Envelope3attackRate:		setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->envelope3.attackRate, 5); break;
	case Pandora::ParamIndices::Envelope3decayRate:			setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->envelope3.decayRate, 5); break;
	case Pandora::ParamIndices::Envelope3sustainLevel:		setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->envelope3.sustainLevel, 2); break;
	case Pandora::ParamIndices::Envelope3releaseRate:		setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->envelope3.releaseRate, 5); break;
	case Pandora::ParamIndices::Envelope4attackRate:		setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->envelope4.attackRate, 5); break;
	case Pandora::ParamIndices::Envelope4decayRate:			setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->envelope4.decayRate, 5); break;
	case Pandora::ParamIndices::Envelope4sustainLevel:		setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->envelope4.sustainLevel, 2); break;
	case Pandora::ParamIndices::Envelope4releaseRate:		setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->envelope4.releaseRate, 5); break;
	case Pandora::ParamIndices::VcfRouting:					setParameterDisplayAndLabel(display, label, gStrFilterRoutingType[(int)((Pandora*)getDevice())->vcfRouting]); break;
	case Pandora::ParamIndices::Vcf1type:					setParameterDisplayAndLabel(display, label, gStrFilterType[(int)((Pandora*)getDevice())->vcf1type]); break;
	case Pandora::ParamIndices::Vcf1Cutoff:					setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->vcf1Cutoff, 3); break; 
	case Pandora::ParamIndices::Vcf1Resonance:				setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->vcf1Resonance, 2); break;
	case Pandora::ParamIndices::Vcf2type:					setParameterDisplayAndLabel(display, label, gStrFilterType[(int)((Pandora*)getDevice())->vcf2type]); break;
	case Pandora::ParamIndices::Vcf2Cutoff:					setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->vcf2Cutoff, 3); break; 
	case Pandora::ParamIndices::Vcf2Resonance:				setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->vcf2Resonance, 2); break;
	case Pandora::ParamIndices::VcfCtrlBalance:				VstPlug::getParameterDisplayAndLabel(index, display, label); break;
	case Pandora::ParamIndices::Vcf2CutoffRelative:			setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->vcf2CutoffRelative); break;
	case Pandora::ParamIndices::VcfDistType:				VstPlug::getParameterDisplayAndLabel(index, display, label); break;
	case Pandora::ParamIndices::FilterDistDrive:			setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->filterDistDrive, 2); break;
	case Pandora::ParamIndices::FilterDistShape:			setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->filterDistShape, 2); break;
	case Pandora::ParamIndices::DoSlide:					setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->doSlide); break;
	case Pandora::ParamIndices::SlideSpeed:					setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->slideSpeed, 4); break;  
	case Pandora::ParamIndices::NumUnisonVoices:			setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->VoicesUnisono); break;
	case Pandora::ParamIndices::UnisonSpread:				setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->unisonSpread, 2); break;
	case Pandora::ParamIndices::ArpeggioType:				setParameterDisplayAndLabel(display, label, gStrArpeggioType[(int)((Pandora*)getDevice())->arpeggioType]); break;
	case Pandora::ParamIndices::ArpeggioNumOctaves:			setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->arpeggioNumOctaves); break;
	case Pandora::ParamIndices::ArpeggioInterval:			VstPlug::getParameterDisplayAndLabel(index, display, label); break;
	case Pandora::ParamIndices::ArpeggioNoteDuration:		VstPlug::getParameterDisplayAndLabel(index, display, label); break;
	case Pandora::ParamIndices::Pan:						setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->VoicesPan, 2); break;
	
	// Modulator
	default:
	{
		int modParamIndex;
		int modIndex;
		int destIndex;
		Pandora::sParamIndexToModulatorIndices(index, &modParamIndex, &modIndex, &destIndex);

		const Pandora::UnresolvedModulationType& mod = ((Pandora*)getDevice())->modulations.modulationsPerDest[destIndex].modulations[modIndex];

		switch ((Pandora::ModulatorParamIndices)modParamIndex)
		{
		case Pandora::ModulatorParamIndices::IsUsed:				setParameterDisplayAndLabel(display, label, mod.isUsed); break;
		case Pandora::ModulatorParamIndices::Source:				setParameterDisplayAndLabel(display, label, gStrModulationSourceType[(int)mod.source]); break;
		case Pandora::ModulatorParamIndices::DepthSource:			setParameterDisplayAndLabel(display, label, gStrModulationDepthSourceType[(int)mod.depthSource]); break;
		case Pandora::ModulatorParamIndices::ConstantDepth:			setParameterDisplayAndLabel(display, label, mod.constantDepth, 2); break;
		case Pandora::ModulatorParamIndices::ConstantDepthRange:	setParameterDisplayAndLabel(display, label, gStrModulationDepthRange[(int)mod.constantDepthRange]); break;
		}
	}
	break;
	}
}


bool PandoraVst::getEffectName(char *name)
{
	vst_strncpy(name, "WaveSabre - Pandora", kVstMaxEffectNameLen);
	return true;
}

bool PandoraVst::getProductString(char *text)
{
	vst_strncpy(text, "WaveSabre - Pandora", kVstMaxProductStrLen);
	return true;
}

#include "PandoraVst.h"
#include "PandoraEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;




const char* gStrModulationParam[(int)Pandora::ModulatorParamIndices::COUNT] = {
	"IsUsed",
	"Source",
	"Depth",
	"Range",
	"DepthSource",
};

const char* gStrModulationDestType[(int)Pandora::ModulationDestType::COUNT] = {
	"Osc1tune",
	"Osc2tune",
	"Vcf1cutoff",
	"Vcf1resonance",
	"Vcf2cutOff",
	"Vcf2resonance",
	"Vca",
	"Osc3tune",
	"Osc1pulsewidth",
	"Osc2pulsewidth",
	"Osc3pulsewidth",
	"Osc1level",
	"Osc2level",
	"Osc3level",
	"StringLevel",
	"Lfo1rate",
	"Lfo2rate",
	"Lfo3rate",
	"ModDepthA",
	"ModDepthB",
	"ModDepthC",
	"ModDepthD",
};

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
	"MIDICC_A",
	"MIDICC_B",
	"MIDICC_C",
	"MIDICC_D"
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
	case Pandora::ParamIndices::Osc1waveform:				vst_strncpy(text, "Osc1waveform", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc2waveform:				vst_strncpy(text, "Osc2waveform", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc3waveform:				vst_strncpy(text, "Osc3waveform", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc1baseToneTranspose:		vst_strncpy(text, "Osc1basetune", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc2baseToneTranspose:		vst_strncpy(text, "Osc2basetune", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc3baseToneTranspose:		vst_strncpy(text, "Osc3basetune", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc1finetune:				vst_strncpy(text, "Osc1finetune", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc2finetune:				vst_strncpy(text, "Osc2finetune", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc3finetune:				vst_strncpy(text, "Osc3finetune", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc1pulseWidth:				vst_strncpy(text, "Osc1pulsewidth", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc2pulseWidth:				vst_strncpy(text, "Osc2pulsewidth", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc3pulseWidth:				vst_strncpy(text, "Osc3pulsewidth", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::MixAmountOsc1:				vst_strncpy(text, "Osc1level1", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::MixAmountOsc2:				vst_strncpy(text, "Osc1level2", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::MixAmountOsc3:				vst_strncpy(text, "Osc1level3", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::StringDamping:				vst_strncpy(text, "StringDamping", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::StringFeedbackAmount:		vst_strncpy(text, "StringFeedback", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::StringFeedbackDelay:		vst_strncpy(text, "StringFeedbackDelay", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::StringThickness:			vst_strncpy(text, "StringThickness", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::StringLevel:				vst_strncpy(text, "StringLevel", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::OscStringMix:				vst_strncpy(text, "OscStingMix", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc2sync:					vst_strncpy(text, "Osc2sync", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo1rate:					vst_strncpy(text, "Lfo1rate", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo2rate:					vst_strncpy(text, "Lfo2rate", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo3rate:					vst_strncpy(text, "Lfo3rate", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo1waveform:				vst_strncpy(text, "Lfo1waveform", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo2waveform:				vst_strncpy(text, "Lfo2waveform", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo3waveform:				vst_strncpy(text, "Lfo3waveform", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo1keysync:				vst_strncpy(text, "Lfo1keysync", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo2keysync:				vst_strncpy(text, "Lfo2keysync", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo3keysync:				vst_strncpy(text, "Lfo3keysync", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo1positive:				vst_strncpy(text, "Lfo1positive", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo2positive:				vst_strncpy(text, "Lfo2positive", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo3positive:				vst_strncpy(text, "Lfo3positive", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope1attackDuration:	vst_strncpy(text, "Env1attack", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope1decayDuration:		vst_strncpy(text, "Env1decay", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope1sustainLevel:		vst_strncpy(text, "Env1sustain", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope1releaseDuration:	vst_strncpy(text, "Env1release", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope2attackDuration:	vst_strncpy(text, "Env2attack", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope2decayDuration:		vst_strncpy(text, "Env2decay", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope2sustainLevel:		vst_strncpy(text, "Env2sustain", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope2releaseDuration:	vst_strncpy(text, "Env2release", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope3attackDuration:	vst_strncpy(text, "Env3attack", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope3decayDuration:		vst_strncpy(text, "Env3decay", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope3sustainLevel:		vst_strncpy(text, "Env3sustain", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope3releaseDuration:	vst_strncpy(text, "Env3release", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope4attackDuration:	vst_strncpy(text, "Env4attack", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope4decayDuration:		vst_strncpy(text, "Env4decay", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope4sustainLevel:		vst_strncpy(text, "Env4sustain", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope4releaseDuration:	vst_strncpy(text, "Env4release", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::VcfRouting:					vst_strncpy(text, "VcfRouting", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Vcf1type:					vst_strncpy(text, "Vcf1type", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Vcf1Cutoff:					vst_strncpy(text, "Vcf1cutoff", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Vcf1Resonance:				vst_strncpy(text, "Vcf1resonance", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Vcf2type:					vst_strncpy(text, "Vcf2type", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Vcf2Cutoff:					vst_strncpy(text, "Vcf2cutoff", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Vcf2Resonance:				vst_strncpy(text, "Vcf2resonance", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::VcfCtrlBalance:				vst_strncpy(text, "Vcfbalance", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Vcf2CutoffRelative:			vst_strncpy(text, "Vcf2cutrelative", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::VcfDistType:				vst_strncpy(text, "Vcfdisttype", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::FilterDistDrive:			vst_strncpy(text, "Vcfdistdrive", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::FilterDistShape:			vst_strncpy(text, "Vcfdistshape", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::DoSlide:					vst_strncpy(text, "DoSlide", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::SlideSpeed:					vst_strncpy(text, "Slidespeed", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::NumUnisonVoices:			vst_strncpy(text, "NumUnison", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::UnisonSpread:				vst_strncpy(text, "Unisonspread", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::ArpeggioType:				vst_strncpy(text, "ArpType", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::ArpeggioNumOctaves:			vst_strncpy(text, "ArpOctaves", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::ArpeggioInterval:			vst_strncpy(text, "ArpInterval", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::ArpeggioNoteDuration:		vst_strncpy(text, "ArpNoteDuration", kVstExtMaxParamStrLen); break;
	case Pandora::ParamIndices::Pan:						vst_strncpy(text, "Pan", kVstExtMaxParamStrLen); break;

	// Modulator
	default:
	{
		int modParamIndex;
		int modIndex;
		int destIndex;
		Pandora::sParamIndexToModulatorIndices(index, &modParamIndex, &modIndex, &destIndex);

		sprintf_s(text, kVstExtMaxParamStrLen + 1, "Mod%s_%s%d", gStrModulationDestType[destIndex], gStrModulationParam[modParamIndex], modIndex);
	}
	break;
	}
}

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
	case Pandora::ParamIndices::Envelope1attackDuration:	setParameterDisplayAndLabel(display, label, 1000.0f / ((float) Helpers::CurrentSampleRate * ((Pandora*)getDevice())->envelope1.attackRate), 2, " ms"); break; 
	case Pandora::ParamIndices::Envelope1decayDuration:		setParameterDisplayAndLabel(display, label, 1000.0f / ((float) Helpers::CurrentSampleRate * ((Pandora*)getDevice())->envelope1.decayRate), 2, " ms"); break; 
	case Pandora::ParamIndices::Envelope1sustainLevel:		setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->envelope1.sustainLevel, 2); break;
	case Pandora::ParamIndices::Envelope1releaseDuration:	setParameterDisplayAndLabel(display, label, 1000.0f / ((float) Helpers::CurrentSampleRate * ((Pandora*)getDevice())->envelope1.releaseRate), 2, " ms"); break;
	case Pandora::ParamIndices::Envelope2attackDuration:	setParameterDisplayAndLabel(display, label, 1000.0f / ((float) Helpers::CurrentSampleRate * ((Pandora*)getDevice())->envelope2.attackRate), 2, " ms"); break;
	case Pandora::ParamIndices::Envelope2decayDuration:		setParameterDisplayAndLabel(display, label, 1000.0f / ((float) Helpers::CurrentSampleRate * ((Pandora*)getDevice())->envelope2.decayRate), 2, " ms"); break;
	case Pandora::ParamIndices::Envelope2sustainLevel:		setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->envelope2.sustainLevel, 2); break;
	case Pandora::ParamIndices::Envelope2releaseDuration:	setParameterDisplayAndLabel(display, label, 1000.0f / ((float) Helpers::CurrentSampleRate * ((Pandora*)getDevice())->envelope2.releaseRate), 2, " ms"); break;
	case Pandora::ParamIndices::Envelope3attackDuration:	setParameterDisplayAndLabel(display, label, 1000.0f / ((float) Helpers::CurrentSampleRate * ((Pandora*)getDevice())->envelope3.attackRate), 2, " ms"); break;
	case Pandora::ParamIndices::Envelope3decayDuration:		setParameterDisplayAndLabel(display, label, 1000.0f / ((float) Helpers::CurrentSampleRate * ((Pandora*)getDevice())->envelope3.decayRate), 2, " ms"); break;
	case Pandora::ParamIndices::Envelope3sustainLevel:		setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->envelope3.sustainLevel, 2); break;
	case Pandora::ParamIndices::Envelope3releaseDuration:	setParameterDisplayAndLabel(display, label, 1000.0f / ((float) Helpers::CurrentSampleRate * ((Pandora*)getDevice())->envelope3.releaseRate), 2, " ms"); break;
	case Pandora::ParamIndices::Envelope4attackDuration:	setParameterDisplayAndLabel(display, label, 1000.0f / ((float) Helpers::CurrentSampleRate * ((Pandora*)getDevice())->envelope4.attackRate), 2, " ms"); break;
	case Pandora::ParamIndices::Envelope4decayDuration:		setParameterDisplayAndLabel(display, label, 1000.0f / ((float) Helpers::CurrentSampleRate * ((Pandora*)getDevice())->envelope4.decayRate), 2, " ms"); break;
	case Pandora::ParamIndices::Envelope4sustainLevel:		setParameterDisplayAndLabel(display, label, ((Pandora*)getDevice())->envelope4.sustainLevel, 2); break;
	case Pandora::ParamIndices::Envelope4releaseDuration:	setParameterDisplayAndLabel(display, label, 1000.0f / ((float) Helpers::CurrentSampleRate * ((Pandora*)getDevice())->envelope4.releaseRate), 2, " ms"); break;
	case Pandora::ParamIndices::VcfRouting:					setParameterDisplayAndLabel(display, label, gStrFilterRoutingType[(int)((Pandora*)getDevice())->vcfRouting]); break;
	case Pandora::ParamIndices::Vcf1type:					setParameterDisplayAndLabel(display, label, gStrFilterType[(int)((Pandora*)getDevice())->vcf1type]); break;
	case Pandora::ParamIndices::Vcf1Cutoff:					VstPlug::getParameterDisplayAndLabel(index, display, label); break;
	case Pandora::ParamIndices::Vcf1Resonance:				VstPlug::getParameterDisplayAndLabel(index, display, label); break;
	case Pandora::ParamIndices::Vcf2type:					setParameterDisplayAndLabel(display, label, gStrFilterType[(int)((Pandora*)getDevice())->vcf2type]); break;
	case Pandora::ParamIndices::Vcf2Cutoff:					VstPlug::getParameterDisplayAndLabel(index, display, label); break;
	case Pandora::ParamIndices::Vcf2Resonance:				VstPlug::getParameterDisplayAndLabel(index, display, label); break;
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

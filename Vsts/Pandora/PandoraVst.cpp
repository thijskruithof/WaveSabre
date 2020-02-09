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
	case Pandora::ParamIndices::Osc1waveform:				vst_strncpy(text, "Osc1waveForm", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc2waveform:				vst_strncpy(text, "Osc2waveForm", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc3waveform:				vst_strncpy(text, "Osc3waveForm", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc1baseToneTranspose:		vst_strncpy(text, "Osc1baseToneTranspose", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc2baseToneTranspose:		vst_strncpy(text, "Osc2baseToneTranspose", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc3baseToneTranspose:		vst_strncpy(text, "Osc3baseToneTranspose", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc1finetune:				vst_strncpy(text, "Osc1finetune", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc2finetune:				vst_strncpy(text, "Osc2finetune", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc3finetune:				vst_strncpy(text, "Osc3finetune", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc1pulseWidth:				vst_strncpy(text, "Osc1pulseWidth", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc2pulseWidth:				vst_strncpy(text, "Osc2pulseWidth", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc3pulseWidth:				vst_strncpy(text, "Osc3pulseWidth", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::MixAmountOsc1:				vst_strncpy(text, "MixAmountOsc1", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::MixAmountOsc2:				vst_strncpy(text, "MixAmountOsc2", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::MixAmountOsc3:				vst_strncpy(text, "MixAmountOsc3", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::StringDamping:				vst_strncpy(text, "StringDamping", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::StringFeedbackAmount:		vst_strncpy(text, "StringFeedbackAmount", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::StringFeedbackDelay:		vst_strncpy(text, "StringFeedbackDelay", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::StringThickness:			vst_strncpy(text, "StringThickness", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::StringLevel:				vst_strncpy(text, "StringLevel", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::OscStringMix:				vst_strncpy(text, "OscStringMix", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Osc2sync:					vst_strncpy(text, "Osc2sync", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo1rate:					vst_strncpy(text, "Lfo1rate", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo2rate:					vst_strncpy(text, "Lfo2rate", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo3rate:					vst_strncpy(text, "Lfo3rate", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo1waveform:				vst_strncpy(text, "Lfo1waveform", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo2waveform:				vst_strncpy(text, "Lfo2waveform", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo3waveform:				vst_strncpy(text, "Lfo3waveform", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo1keysync:				vst_strncpy(text, "Lfo1keysync", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo2keysync:				vst_strncpy(text, "Lfo2keysync", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo3keysync:				vst_strncpy(text, "Lfo3keysync", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo1positive:				vst_strncpy(text, "Lfo1positive", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo2positive:				vst_strncpy(text, "Lfo2positive", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Lfo3positive:				vst_strncpy(text, "Lfo3positive", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope1attackRate:		vst_strncpy(text, "Envelope1attackRate", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope1decayRate:			vst_strncpy(text, "Envelope1decayRate", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope1sustainLevel:		vst_strncpy(text, "Envelope1sustainLevel", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope1releaseRate:		vst_strncpy(text, "Envelope1releaseRate", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope2attackRate:		vst_strncpy(text, "Envelope2attackRate", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope2decayRate:			vst_strncpy(text, "Envelope2decayRate", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope2sustainLevel:		vst_strncpy(text, "Envelope2sustainLevel", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope2releaseRate:		vst_strncpy(text, "Envelope2releaseRate", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope3attackRate:		vst_strncpy(text, "Envelope3attackRate", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope3decayRate:			vst_strncpy(text, "Envelope3decayRate", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope3sustainLevel:		vst_strncpy(text, "Envelope3sustainLevel", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope3releaseRate:		vst_strncpy(text, "Envelope3releaseRate", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope4attackRate:		vst_strncpy(text, "Envelope4attackRate", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope4decayRate:			vst_strncpy(text, "Envelope4decayRate", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope4sustainLevel:		vst_strncpy(text, "Envelope4sustainLevel", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Envelope4releaseRate:		vst_strncpy(text, "Envelope4releaseRate", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::VcfRouting:					vst_strncpy(text, "VcfRouting", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Vcf1type:					vst_strncpy(text, "Vcf1type", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Vcf1Cutoff:					vst_strncpy(text, "Vcf1Cutoff", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Vcf1Resonance:				vst_strncpy(text, "Vcf1Resonance", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Vcf2type:					vst_strncpy(text, "Vcf2type", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Vcf2Cutoff:					vst_strncpy(text, "Vcf2Cutoff", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Vcf2Resonance:				vst_strncpy(text, "Vcf2Resonance", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::VcfCtrlBalance:				vst_strncpy(text, "VcfCtrlBalance", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::Vcf2CutoffRelative:			vst_strncpy(text, "Vcf2CutoffRelative", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::VcfDistType:				vst_strncpy(text, "VcfDistType", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::FilterDistDrive:			vst_strncpy(text, "FilterDistDrive", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::FilterDistShape:			vst_strncpy(text, "FilterDistShape", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::DoSlide:					vst_strncpy(text, "DoSlide", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::SlideSpeed:					vst_strncpy(text, "SlideSpeed", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::NumUnisonVoices:			vst_strncpy(text, "NumUnisonVoices", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::UnisonSpread:				vst_strncpy(text, "UnisonSpread", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::ArpeggioType:				vst_strncpy(text, "ArpeggioType", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::ArpeggioNumOctaves:			vst_strncpy(text, "ArpeggioNumOctaves", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::ArpeggioInterval:			vst_strncpy(text, "ArpeggioInterval", kVstMaxParamStrLen); break;
	case Pandora::ParamIndices::ArpeggioNoteDuration:		vst_strncpy(text, "ArpeggioNoteDuration", kVstMaxParamStrLen); break;
	}
}



void PandoraVst::getParameterDisplayAndLabel(VstInt32 index, char* display, char* label)
{
	switch ((Echo::ParamIndices)index)
	{
	case Pandora::ParamIndices::Osc1waveform:				PLUG_GET_PARAM_DISPLAY(Pandora, osc1waveform, "%d", ""); break;
	case Pandora::ParamIndices::Osc2waveform:				PLUG_GET_PARAM_DISPLAY(Pandora, osc2waveform, "%d", ""); break;
	case Pandora::ParamIndices::Osc3waveform:				PLUG_GET_PARAM_DISPLAY(Pandora, osc3waveform, "%d", ""); break;
	case Pandora::ParamIndices::Osc1baseToneTranspose:		PLUG_GET_PARAM_DISPLAY(Pandora, osc1baseToneTranspose, "%d", ""); break;
	case Pandora::ParamIndices::Osc2baseToneTranspose:		PLUG_GET_PARAM_DISPLAY(Pandora, osc2baseToneTranspose, "%d", ""); break;
	case Pandora::ParamIndices::Osc3baseToneTranspose:		PLUG_GET_PARAM_DISPLAY(Pandora, osc3baseToneTranspose, "%d", ""); break;
	case Pandora::ParamIndices::Osc1finetune:				PLUG_GET_PARAM_DISPLAY(Pandora, osc1finetune, "%.2f", ""); break;
	case Pandora::ParamIndices::Osc2finetune:				PLUG_GET_PARAM_DISPLAY(Pandora, osc2finetune, "%.2f", ""); break;
	case Pandora::ParamIndices::Osc3finetune:				PLUG_GET_PARAM_DISPLAY(Pandora, osc3finetune, "%.2f", ""); break;
	case Pandora::ParamIndices::Osc1pulseWidth:				PLUG_GET_PARAM_DISPLAY(Pandora, osc1pulseWidth, "%.2f", ""); break;
	case Pandora::ParamIndices::Osc2pulseWidth:				PLUG_GET_PARAM_DISPLAY(Pandora, osc2pulseWidth, "%.2f", ""); break;
	case Pandora::ParamIndices::Osc3pulseWidth:				PLUG_GET_PARAM_DISPLAY(Pandora, osc3pulseWidth, "%.2f", ""); break;
	case Pandora::ParamIndices::MixAmountOsc1:				PLUG_GET_PARAM_DISPLAY(Pandora, mixAmountOsc1, "%.2f", ""); break;
	case Pandora::ParamIndices::MixAmountOsc2:				PLUG_GET_PARAM_DISPLAY(Pandora, mixAmountOsc2, "%.2f", ""); break;
	case Pandora::ParamIndices::MixAmountOsc3:				PLUG_GET_PARAM_DISPLAY(Pandora, mixAmountOsc3, "%.2f", ""); break;
	case Pandora::ParamIndices::StringDamping:				PLUG_GET_PARAM_DISPLAY(Pandora, stringDamping, "%.2f", ""); break;
	case Pandora::ParamIndices::StringFeedbackAmount:		PLUG_GET_PARAM_DISPLAY(Pandora, stringFeedbackAmount, "%.2f", ""); break;
	case Pandora::ParamIndices::StringFeedbackDelay:		PLUG_GET_PARAM_DISPLAY(Pandora, stringFeedbackDelay, "%d", "smpls"); break;
	case Pandora::ParamIndices::StringThickness:			PLUG_GET_PARAM_DISPLAY(Pandora, stringThickness, "%.2f", ""); break;
	case Pandora::ParamIndices::StringLevel:				PLUG_GET_PARAM_DISPLAY(Pandora, stringLevel, "%.2f", ""); break;
	case Pandora::ParamIndices::OscStringMix:				PLUG_GET_PARAM_DISPLAY(Pandora, oscStringMix, "%.2f", ""); break;
	case Pandora::ParamIndices::Osc2sync:					PLUG_GET_PARAM_DISPLAY(Pandora, osc2sync, "%d", ""); break;
	case Pandora::ParamIndices::Lfo1rate:					PLUG_GET_PARAM_DISPLAY(Pandora, lfo1rate, "%.5f", ""); break;
	case Pandora::ParamIndices::Lfo2rate:					PLUG_GET_PARAM_DISPLAY(Pandora, lfo2rate, "%.5f", ""); break;
	case Pandora::ParamIndices::Lfo3rate:					PLUG_GET_PARAM_DISPLAY(Pandora, lfo3rate, "%.5f", ""); break;
	case Pandora::ParamIndices::Lfo1waveform:				PLUG_GET_PARAM_DISPLAY(Pandora, lfo1waveform, "%d", ""); break;
	case Pandora::ParamIndices::Lfo2waveform:				PLUG_GET_PARAM_DISPLAY(Pandora, lfo2waveform, "%d", ""); break;
	case Pandora::ParamIndices::Lfo3waveform:				PLUG_GET_PARAM_DISPLAY(Pandora, lfo3waveform, "%d", ""); break;
	case Pandora::ParamIndices::Lfo1keysync:				PLUG_GET_PARAM_DISPLAY(Pandora, lfo1keysync, "%d", ""); break;
	case Pandora::ParamIndices::Lfo2keysync:				PLUG_GET_PARAM_DISPLAY(Pandora, lfo2keysync, "%d", ""); break;
	case Pandora::ParamIndices::Lfo3keysync:				PLUG_GET_PARAM_DISPLAY(Pandora, lfo3keysync, "%d", ""); break;
	case Pandora::ParamIndices::Lfo1positive:				PLUG_GET_PARAM_DISPLAY(Pandora, lfo1positive, "%d", ""); break;
	case Pandora::ParamIndices::Lfo2positive:				PLUG_GET_PARAM_DISPLAY(Pandora, lfo2positive, "%d", ""); break;
	case Pandora::ParamIndices::Lfo3positive:				PLUG_GET_PARAM_DISPLAY(Pandora, lfo3positive, "%d", ""); break;
	case Pandora::ParamIndices::Envelope1attackRate:		PLUG_GET_PARAM_DISPLAY(Pandora, envelope1.attackRate, "%.2f", ""); break;
	case Pandora::ParamIndices::Envelope1decayRate:			PLUG_GET_PARAM_DISPLAY(Pandora, envelope1.decayRate, "%.2f", ""); break;
	case Pandora::ParamIndices::Envelope1sustainLevel:		PLUG_GET_PARAM_DISPLAY(Pandora, envelope1.sustainLevel, "%.2f", ""); break;
	case Pandora::ParamIndices::Envelope1releaseRate:		PLUG_GET_PARAM_DISPLAY(Pandora, envelope1.releaseRate, "%.2f", ""); break;
	case Pandora::ParamIndices::Envelope2attackRate:		PLUG_GET_PARAM_DISPLAY(Pandora, envelope2.attackRate, "%.2f", ""); break;
	case Pandora::ParamIndices::Envelope2decayRate:			PLUG_GET_PARAM_DISPLAY(Pandora, envelope2.decayRate, "%.2f", ""); break;
	case Pandora::ParamIndices::Envelope2sustainLevel:		PLUG_GET_PARAM_DISPLAY(Pandora, envelope2.sustainLevel, "%.2f", ""); break;
	case Pandora::ParamIndices::Envelope2releaseRate:		PLUG_GET_PARAM_DISPLAY(Pandora, envelope2.releaseRate, "%.2f", ""); break;
	case Pandora::ParamIndices::Envelope3attackRate:		PLUG_GET_PARAM_DISPLAY(Pandora, envelope3.attackRate, "%.2f", ""); break;
	case Pandora::ParamIndices::Envelope3decayRate:			PLUG_GET_PARAM_DISPLAY(Pandora, envelope3.decayRate, "%.2f", ""); break;
	case Pandora::ParamIndices::Envelope3sustainLevel:		PLUG_GET_PARAM_DISPLAY(Pandora, envelope3.sustainLevel, "%.2f", ""); break;
	case Pandora::ParamIndices::Envelope3releaseRate:		PLUG_GET_PARAM_DISPLAY(Pandora, envelope3.releaseRate, "%.2f", ""); break;
	case Pandora::ParamIndices::Envelope4attackRate:		PLUG_GET_PARAM_DISPLAY(Pandora, envelope4.attackRate, "%.2f", ""); break;
	case Pandora::ParamIndices::Envelope4decayRate:			PLUG_GET_PARAM_DISPLAY(Pandora, envelope4.decayRate, "%.2f", ""); break;
	case Pandora::ParamIndices::Envelope4sustainLevel:		PLUG_GET_PARAM_DISPLAY(Pandora, envelope4.sustainLevel, "%.2f", ""); break;
	case Pandora::ParamIndices::Envelope4releaseRate:		PLUG_GET_PARAM_DISPLAY(Pandora, envelope4.releaseRate, "%.2f", ""); break;
	case Pandora::ParamIndices::VcfRouting:					PLUG_GET_PARAM_DISPLAY(Pandora, vcfRouting, "%d", ""); break;
	case Pandora::ParamIndices::Vcf1type:					PLUG_GET_PARAM_DISPLAY(Pandora, vcf1type, "%d", ""); break;
	case Pandora::ParamIndices::Vcf1Cutoff:					PLUG_GET_PARAM_DISPLAY(Pandora, vcf1Cutoff, "%.5f", ""); break;
	case Pandora::ParamIndices::Vcf1Resonance:				PLUG_GET_PARAM_DISPLAY(Pandora, vcf1Resonance, "%.2f", ""); break;
	case Pandora::ParamIndices::Vcf2type:					PLUG_GET_PARAM_DISPLAY(Pandora, vcf2type, "%d", ""); break;
	case Pandora::ParamIndices::Vcf2Cutoff:					PLUG_GET_PARAM_DISPLAY(Pandora, vcf2Cutoff, "%.5f", ""); break;
	case Pandora::ParamIndices::Vcf2Resonance:				PLUG_GET_PARAM_DISPLAY(Pandora, vcf2Resonance, "%.2f", ""); break;
	case Pandora::ParamIndices::VcfCtrlBalance:				VstPlug::getParameterDisplayAndLabel(index, display, label); break;
	case Pandora::ParamIndices::Vcf2CutoffRelative:			PLUG_GET_PARAM_DISPLAY(Pandora, vcf2CutoffRelative, "%d", ""); break;
	case Pandora::ParamIndices::VcfDistType:				VstPlug::getParameterDisplayAndLabel(index, display, label); break;
	case Pandora::ParamIndices::FilterDistDrive:			PLUG_GET_PARAM_DISPLAY(Pandora, filterDistDrive, "%.2f", ""); break;
	case Pandora::ParamIndices::FilterDistShape:			PLUG_GET_PARAM_DISPLAY(Pandora, filterDistShape, "%.2f", ""); break;
	case Pandora::ParamIndices::DoSlide:					PLUG_GET_PARAM_DISPLAY(Pandora, doSlide, "%d", ""); break;
	case Pandora::ParamIndices::SlideSpeed:					PLUG_GET_PARAM_DISPLAY(Pandora, slideSpeed, "%.2f", ""); break;
	case Pandora::ParamIndices::NumUnisonVoices:			PLUG_GET_PARAM_DISPLAY(Pandora, numUnisonVoices, "%d", ""); break;
	case Pandora::ParamIndices::UnisonSpread:				PLUG_GET_PARAM_DISPLAY(Pandora, unisonSpread, "%.2f", ""); break;
	case Pandora::ParamIndices::ArpeggioType:				PLUG_GET_PARAM_DISPLAY(Pandora, arpeggioType, "%d", ""); break;
	case Pandora::ParamIndices::ArpeggioNumOctaves:			PLUG_GET_PARAM_DISPLAY(Pandora, arpeggioNumOctaves, "%d", ""); break;
	case Pandora::ParamIndices::ArpeggioInterval:			PLUG_GET_PARAM_DISPLAY(Pandora, arpeggioInterval, "%d", ""); break;
	case Pandora::ParamIndices::ArpeggioNoteDuration:		PLUG_GET_PARAM_DISPLAY(Pandora, arpeggioNoteDuration, "%d", ""); break;
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

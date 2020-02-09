#include "PandoraEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

PandoraEditor::PandoraEditor(AudioEffect *audioEffect)
	: VstEditor(audioEffect, 920, 520, "PANDORA")
{
}

PandoraEditor::~PandoraEditor()
{
}

void PandoraEditor::Open()
{
	addKnob((VstInt32)Pandora::ParamIndices::Osc1waveform, "Osc1wave");
	addKnob((VstInt32)Pandora::ParamIndices::Osc2waveform, "Osc2wave");
	addKnob((VstInt32)Pandora::ParamIndices::Osc3waveform, "Osc3wave");
	addKnob((VstInt32)Pandora::ParamIndices::Osc1baseToneTranspose, "Osc1base");
	addKnob((VstInt32)Pandora::ParamIndices::Osc2baseToneTranspose, "Osc2base");
	addKnob((VstInt32)Pandora::ParamIndices::Osc3baseToneTranspose, "Osc3base");
	addKnob((VstInt32)Pandora::ParamIndices::Osc1finetune, "Osc1fine");
	addKnob((VstInt32)Pandora::ParamIndices::Osc2finetune, "Osc2fine");
	addKnob((VstInt32)Pandora::ParamIndices::Osc3finetune, "Osc3fine");
	addKnob((VstInt32)Pandora::ParamIndices::Osc1pulseWidth, "Osc1pw");
	addKnob((VstInt32)Pandora::ParamIndices::Osc2pulseWidth, "Osc2pw");
	addKnob((VstInt32)Pandora::ParamIndices::Osc3pulseWidth, "Osc3pw");
	addKnob((VstInt32)Pandora::ParamIndices::MixAmountOsc1, "MixOsc1");
	addKnob((VstInt32)Pandora::ParamIndices::MixAmountOsc2, "MixOsc2");
	addKnob((VstInt32)Pandora::ParamIndices::MixAmountOsc3, "MixOsc3");
	addKnob((VstInt32)Pandora::ParamIndices::Osc2sync, "Osc2sync");

	startNextRow();

	addKnob((VstInt32)Pandora::ParamIndices::StringDamping, "StringDamp");
	addKnob((VstInt32)Pandora::ParamIndices::StringFeedbackAmount, "StringFB");
	addKnob((VstInt32)Pandora::ParamIndices::StringFeedbackDelay, "StringFBDelay");
	addKnob((VstInt32)Pandora::ParamIndices::StringThickness, "StringThick");
	addKnob((VstInt32)Pandora::ParamIndices::StringLevel, "StringLvl");
	addKnob((VstInt32)Pandora::ParamIndices::OscStringMix, "OscStringMix");

	startNextRow();

	addKnob((VstInt32)Pandora::ParamIndices::Lfo1rate, "Lfo1rate");
	addKnob((VstInt32)Pandora::ParamIndices::Lfo2rate, "Lfo2rate");
	addKnob((VstInt32)Pandora::ParamIndices::Lfo3rate, "Lfo3rate");
	addKnob((VstInt32)Pandora::ParamIndices::Lfo1waveform, "Lfo1wave");
	addKnob((VstInt32)Pandora::ParamIndices::Lfo2waveform, "Lfo2wave");
	addKnob((VstInt32)Pandora::ParamIndices::Lfo3waveform, "Lfo3wave");
	addKnob((VstInt32)Pandora::ParamIndices::Lfo1keysync, "Lfo1keysync");
	addKnob((VstInt32)Pandora::ParamIndices::Lfo2keysync, "Lfo2keysync");
	addKnob((VstInt32)Pandora::ParamIndices::Lfo3keysync, "Lfo3keysync");
	addKnob((VstInt32)Pandora::ParamIndices::Lfo1positive, "Lfo1positive");
	addKnob((VstInt32)Pandora::ParamIndices::Lfo2positive, "Lfo2positive");
	addKnob((VstInt32)Pandora::ParamIndices::Lfo3positive, "Lfo3positive");

	startNextRow();

	addKnob((VstInt32)Pandora::ParamIndices::Envelope1attackRate, "Env1attack");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope1decayRate, "Env1decay");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope1sustainLevel, "Env1sus");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope1releaseRate, "Env1rel");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope2attackRate, "Env2attack");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope2decayRate, "Env2decay");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope2sustainLevel, "Env2sus");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope2releaseRate, "Env2rel");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope3attackRate, "Env3attack");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope3decayRate, "Env3decay");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope3sustainLevel, "Env3sus");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope3releaseRate, "Env3rel");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope4attackRate, "Env4attack");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope4decayRate, "Env4decay");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope4sustainLevel, "Env4sus");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope4releaseRate, "Env4rel");

	startNextRow();

	addKnob((VstInt32)Pandora::ParamIndices::VcfRouting, "VcfRouting");
	addKnob((VstInt32)Pandora::ParamIndices::Vcf1type, "Vcf1type");
	addKnob((VstInt32)Pandora::ParamIndices::Vcf1Cutoff, "Vcf1Cutoff");
	addKnob((VstInt32)Pandora::ParamIndices::Vcf1Resonance, "Vcf1Reso");
	addKnob((VstInt32)Pandora::ParamIndices::Vcf2type, "Vcf2type");
	addKnob((VstInt32)Pandora::ParamIndices::Vcf2Cutoff, "Vcf2Cutoff");
	addKnob((VstInt32)Pandora::ParamIndices::Vcf2Resonance, "Vcf2Reso");
	addKnob((VstInt32)Pandora::ParamIndices::VcfCtrlBalance, "VcfBalance");
	addKnob((VstInt32)Pandora::ParamIndices::Vcf2CutoffRelative, "Vcf2CutoffRel");

	startNextRow();

	addKnob((VstInt32)Pandora::ParamIndices::VcfDistType, "VcfDistType");
	addKnob((VstInt32)Pandora::ParamIndices::FilterDistDrive, "VcfDistDrive");
	addKnob((VstInt32)Pandora::ParamIndices::FilterDistShape, "VcfDistShape");

	addSpacer();

	addKnob((VstInt32)Pandora::ParamIndices::DoSlide, "DoSlide");
	addKnob((VstInt32)Pandora::ParamIndices::SlideSpeed, "SlideSpeed");

	addSpacer();

	addKnob((VstInt32)Pandora::ParamIndices::NumUnisonVoices, "UnisonVoices");
	addKnob((VstInt32)Pandora::ParamIndices::UnisonSpread, "UnisonSpread");

	startNextRow();

	addKnob((VstInt32)Pandora::ParamIndices::ArpeggioType, "ArpType");
	addKnob((VstInt32)Pandora::ParamIndices::ArpeggioNumOctaves, "ArpNumOctaves");
	addKnob((VstInt32)Pandora::ParamIndices::ArpeggioInterval, "ArpInterval");
	addKnob((VstInt32)Pandora::ParamIndices::ArpeggioNoteDuration, "ArpNoteDur");

	VstEditor::Open();
}

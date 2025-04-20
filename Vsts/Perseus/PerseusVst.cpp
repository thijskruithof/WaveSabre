#include "PerseusVst.h"
#include "PerseusEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

AudioEffect *createEffectInstance(audioMasterCallback audioMaster)
{
	Helpers::Init();
	return new PerseusVst(audioMaster);
}

PerseusVst::PerseusVst(audioMasterCallback audioMaster)
	: VstPlug(audioMaster, (int)Perseus::ParamIndices::NumParams, 2, 2, 'PERS', new Perseus())
{
	setEditor(new PerseusEditor(this));
}

void PerseusVst::getParameterName(VstInt32 index, char *text)
{
	switch ((Perseus::ParamIndices)index)
	{
		case Perseus::ParamIndices::Amount	: vst_strncpy(text, "Amount", kVstMaxParamStrLen); break;
		case Perseus::ParamIndices::InputGain	: vst_strncpy(text, "InputGain", kVstMaxParamStrLen); break;
		case Perseus::ParamIndices::Time		: vst_strncpy(text, "Time", kVstMaxParamStrLen); break;
		case Perseus::ParamIndices::Diffusion	: vst_strncpy(text, "Diffusion", kVstMaxParamStrLen); break;
		case Perseus::ParamIndices::LP		: vst_strncpy(text, "LP", kVstMaxParamStrLen); break;
	}
}

bool PerseusVst::getEffectName(char *name)
{
	vst_strncpy(name, "WaveSabre - Perseus", kVstMaxEffectNameLen);
	return true;
}

bool PerseusVst::getProductString(char *text)
{
	vst_strncpy(text, "WaveSabre - Perseus", kVstMaxProductStrLen);
	return true;
}

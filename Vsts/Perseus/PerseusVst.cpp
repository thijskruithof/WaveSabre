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
	: VstPlug(audioMaster, (int)Perseus::ParamIndices::NumParams, 2, 2, 'PERS', new Perseus(), true)
{
	setEditor(new PerseusEditor(this));
}

void PerseusVst::getParameterName(VstInt32 index, char *text)
{
	switch ((Perseus::ParamIndices)index)
	{
		case Perseus::ParamIndices::Frequency:	vst_strncpy(text, "Frequency", kVstMaxParamStrLen); break;
		case Perseus::ParamIndices::Structure:	vst_strncpy(text, "Structure", kVstMaxParamStrLen); break;
		case Perseus::ParamIndices::Brightness:	vst_strncpy(text, "Brightness", kVstMaxParamStrLen); break;
		case Perseus::ParamIndices::Damping:	vst_strncpy(text, "Damping", kVstMaxParamStrLen); break;
		case Perseus::ParamIndices::Position:	vst_strncpy(text, "Position", kVstMaxParamStrLen); break;
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

#include "PyxisVst.h"
#include "PyxisEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

AudioEffect *createEffectInstance(audioMasterCallback audioMaster)
{
	Helpers::Init();
	return new PyxisVst(audioMaster);
}

PyxisVst::PyxisVst(audioMasterCallback audioMaster)
	: VstPlug(audioMaster, (int)Pyxis::ParamIndices::NumParams, 2, 2, 'PYXS', new Pyxis())
{
	setEditor(new PyxisEditor(this));
}

void PyxisVst::getParameterName(VstInt32 index, char *text)
{
	switch ((Pyxis::ParamIndices)index)
	{
	case Pyxis::ParamIndices::LeftDelayCoarse: vst_strncpy(text, "LDly Crs", kVstMaxParamStrLen); break;
	case Pyxis::ParamIndices::LeftDelayFine: vst_strncpy(text, "LDly Fin", kVstMaxParamStrLen); break;
	case Pyxis::ParamIndices::RightDelayCoarse: vst_strncpy(text, "RDly Crs", kVstMaxParamStrLen); break;
	case Pyxis::ParamIndices::RightDelayFine: vst_strncpy(text, "RDly Fin", kVstMaxParamStrLen); break;
	case Pyxis::ParamIndices::LowCutFreq: vst_strncpy(text, "LC Freq", kVstMaxParamStrLen); break;
	case Pyxis::ParamIndices::HighCutFreq: vst_strncpy(text, "HC Freq", kVstMaxParamStrLen); break;
	case Pyxis::ParamIndices::Feedback: vst_strncpy(text, "Feedback", kVstMaxParamStrLen); break;
	case Pyxis::ParamIndices::Cross: vst_strncpy(text, "Cross", kVstMaxParamStrLen); break;
	case Pyxis::ParamIndices::DryWet: vst_strncpy(text, "Dry/Wet", kVstMaxParamStrLen); break;
	}
}

bool PyxisVst::getEffectName(char *name)
{
	vst_strncpy(name, "WaveSabre - Pyxis", kVstMaxEffectNameLen);
	return true;
}

bool PyxisVst::getProductString(char *text)
{
	vst_strncpy(text, "WaveSabre - Pyxis", kVstMaxProductStrLen);
	return true;
}

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
		case Pyxis::ParamIndices::Amount	: vst_strncpy(text, "Amount", kVstMaxParamStrLen); break;
		case Pyxis::ParamIndices::InputGain	: vst_strncpy(text, "InputGain", kVstMaxParamStrLen); break;
		case Pyxis::ParamIndices::Time		: vst_strncpy(text, "Time", kVstMaxParamStrLen); break;
		case Pyxis::ParamIndices::Diffusion	: vst_strncpy(text, "Diffusion", kVstMaxParamStrLen); break;
		case Pyxis::ParamIndices::LP		: vst_strncpy(text, "LP", kVstMaxParamStrLen); break;
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

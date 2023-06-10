#include "PyxisEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

PyxisEditor::PyxisEditor(AudioEffect *audioEffect)
	: VstEditor(audioEffect, 400, 160, "PYXIS")
{
}

PyxisEditor::~PyxisEditor()
{
}

void PyxisEditor::Open()
{
	addKnob((VstInt32)Pyxis::ParamIndices::LeftDelayCoarse, "LEFT COARSE");
	addSpacer();
	addKnob((VstInt32)Pyxis::ParamIndices::LeftDelayFine, "LEFT FINE");
	addSpacer();
	addKnob((VstInt32)Pyxis::ParamIndices::Feedback, "FEEDBACK");
	addSpacer();
	addKnob((VstInt32)Pyxis::ParamIndices::Cross, "CROSS");
	addSpacer();
	addKnob((VstInt32)Pyxis::ParamIndices::DryWet, "DRY/WET");

	startNextRow();
	addKnob((VstInt32)Pyxis::ParamIndices::RightDelayCoarse, "RIGHT COARSE");
	addSpacer();
	addKnob((VstInt32)Pyxis::ParamIndices::RightDelayFine, "RIGHT FINE");
	addSpacer();
	addKnob((VstInt32)Pyxis::ParamIndices::LowCutFreq, "LC FREQ");
	addSpacer();
	addKnob((VstInt32)Pyxis::ParamIndices::HighCutFreq, "HC FREQ");

	VstEditor::Open();
}

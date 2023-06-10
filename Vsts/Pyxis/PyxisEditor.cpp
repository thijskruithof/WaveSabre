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
	addKnob((VstInt32)Pyxis::ParamIndices::Amount, "Dry/Wet");
	addSpacer();
	addKnob((VstInt32)Pyxis::ParamIndices::InputGain, "Input Gain");
	addSpacer();
	addKnob((VstInt32)Pyxis::ParamIndices::Time, "Time");
	addSpacer();
	addKnob((VstInt32)Pyxis::ParamIndices::Diffusion, "Diffusion");
	addSpacer();
	addKnob((VstInt32)Pyxis::ParamIndices::LP, "LP");

	VstEditor::Open();
}

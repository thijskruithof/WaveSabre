#include "PerseusEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

PerseusEditor::PerseusEditor(AudioEffect *audioEffect)
	: VstEditor(audioEffect, 400, 160, "PERSEUS")
{
}

PerseusEditor::~PerseusEditor()
{
}

void PerseusEditor::Open()
{
	addKnob((VstInt32)Perseus::ParamIndices::Amount, "Dry/Wet");
	addSpacer();
	addKnob((VstInt32)Perseus::ParamIndices::InputGain, "Input Gain");
	addSpacer();
	addKnob((VstInt32)Perseus::ParamIndices::Time, "Time");
	addSpacer();
	addKnob((VstInt32)Perseus::ParamIndices::Diffusion, "Diffusion");
	addSpacer();
	addKnob((VstInt32)Perseus::ParamIndices::LP, "LP");

	VstEditor::Open();
}

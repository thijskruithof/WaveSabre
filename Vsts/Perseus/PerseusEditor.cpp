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
	addKnob((VstInt32)Perseus::ParamIndices::PolyphonyMode, "PolyhonyMode");
	addSpacer();
	addKnob((VstInt32)Perseus::ParamIndices::ResonatorModel, "ResonatorModel");
	addSpacer();
	startNextRow();
	addKnob((VstInt32)Perseus::ParamIndices::Frequency, "Frequency");
	addSpacer();
	addKnob((VstInt32)Perseus::ParamIndices::Structure, "Structure");
	addSpacer();
	addKnob((VstInt32)Perseus::ParamIndices::Brightness, "Brightness");
	addSpacer();
	addKnob((VstInt32)Perseus::ParamIndices::Damping, "Damping");
	addSpacer();
	addKnob((VstInt32)Perseus::ParamIndices::Position, "Position");

	VstEditor::Open();
}

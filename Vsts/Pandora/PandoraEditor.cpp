#include "PandoraEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

PandoraEditor::PandoraEditor(AudioEffect *audioEffect)
	: VstEditor(audioEffect, 820, 220, "PANDORA")
{
}

PandoraEditor::~PandoraEditor()
{
}

void PandoraEditor::Open()
{
	addKnob((VstInt32)Pandora::ParamIndices::MasterLevel, "MASTER");

	VstEditor::Open();
}

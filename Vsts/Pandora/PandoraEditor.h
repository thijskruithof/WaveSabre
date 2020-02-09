#ifndef __PANDORAEDITOR_H__
#define __PANDORAEDITOR_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class PandoraEditor : public VstEditor
{
public:
	PandoraEditor(AudioEffect *audioEffect);
	virtual ~PandoraEditor();

	virtual void Open();
};

#endif

#ifndef __PERSEUSEDITOR_H__
#define __PERSEUSEDITOR_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class PerseusEditor : public VstEditor
{
public:
	PerseusEditor(AudioEffect *audioEffect);
	virtual ~PerseusEditor();

	virtual void Open();
};

#endif

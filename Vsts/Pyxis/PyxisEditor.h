#ifndef __PYXISEDITOR_H__
#define __PYXISEDITOR_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class PyxisEditor : public VstEditor
{
public:
	PyxisEditor(AudioEffect *audioEffect);
	virtual ~PyxisEditor();

	virtual void Open();
};

#endif

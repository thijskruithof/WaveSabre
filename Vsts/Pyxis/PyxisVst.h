#ifndef __PYXISVST_H__
#define __PYXISVST_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class PyxisVst : public VstPlug
{
public:
	PyxisVst(audioMasterCallback audioMaster);

	virtual void getParameterName(VstInt32 index, char *text);

	virtual bool getEffectName(char *name);
	virtual bool getProductString(char *text);
};

#endif
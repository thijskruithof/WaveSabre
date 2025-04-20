#ifndef __PERSEUSVST_H__
#define __PERSEUSVST_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class PerseusVst : public VstPlug
{
public:
	PerseusVst(audioMasterCallback audioMaster);

	virtual void getParameterName(VstInt32 index, char *text);

	virtual bool getEffectName(char *name);
	virtual bool getProductString(char *text);
};

#endif
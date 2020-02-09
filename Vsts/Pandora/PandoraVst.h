#ifndef __PANDORAVST_H__
#define __PANDORAVST_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class PandoraVst : public VstPlug
{
public:
	PandoraVst(audioMasterCallback audioMaster);

	virtual void getParameterName(VstInt32 index, char *text);

	virtual bool getEffectName(char *name);
	virtual bool getProductString(char *text);
};

#endif

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
	virtual void Close();


	virtual void valueChanged(CControl* control);

private:
	void addModulatorControls();
	void onModulatorOnOffChanged(int modulatorPanelIndex, bool newValue);
	void updateModulatorPanelOnOffState(int modulatorPanelIndex);

	class ModulatorOnOffButtonListener : public CControlListener
	{
	public:
		ModulatorOnOffButtonListener(PandoraEditor* editor) : editor(editor) {}

		virtual void valueChanged(VSTGUI::CControl* pControl);
	private:
		PandoraEditor* editor;
	};

	struct ModulatorPanelInfo
	{
		CViewContainer* panel;
		COnOffButton* onOffButton;
		CTextLabel* onOffLabel;
		int modulationSourceParameterIndex;
	};

	ModulatorOnOffButtonListener* modulatorOnOffButtonListener;
	std::vector<ModulatorPanelInfo> modulatorPanelInfos;
};

#endif

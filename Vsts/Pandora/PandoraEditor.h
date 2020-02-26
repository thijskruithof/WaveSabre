#ifndef __PANDORAEDITOR_H__
#define __PANDORAEDITOR_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class CFrameResizerListener
{
public:
	virtual void onFrameResizerSizeAdjusted(int newWidth, int newHeight) = 0;
};


class CFrameResizer : public CViewContainer
{
public:
	CFrameResizer(int minFrameWidth, int minFrameHeight, CFrameResizerListener* listener, const CRect& size, CFrame* pParent);
	CFrameResizer(const CFrameResizer& other);

	virtual CMouseEventResult onMouseDown(CPoint& where, const long& buttons);
	virtual CMouseEventResult onMouseUp(CPoint& where, const long& buttons);
	virtual CMouseEventResult onMouseMoved(CPoint& where, const long& buttons);

private:
	int minFrameWidth = 64;
	int minFrameHeight = 64;

	bool isResizing = false;
	CPoint startResizeMousePos = CPoint(0, 0);
	int startResizeFrameWidth = 0;
	int startResizeFrameHeight = 0;

	CBitmap* icon = NULL;
	CFrameResizerListener* listener = NULL;
};


class PandoraEditor : public VstEditor, public CFrameResizerListener
{
public:
	PandoraEditor(AudioEffect *audioEffect);
	virtual ~PandoraEditor();

	virtual void Open();
	virtual void Close();

	virtual void idle();
	virtual void valueChanged(CControl* control);
	virtual void onFrameResizerSizeAdjusted(int newWidth, int newHeight);

private:
	void addModulatorControls();
	void addResizeControl();
	void onModulatorOnOffChanged(int modulatorPanelIndex, bool newValue);
	void updateModulatorPanelOnOffState(int modulatorPanelIndex);
	void onResized(int newFrameWidth, int newFrameHeight, int oldFrameWidth, int oldFrameHeight);

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
		int modulationIsUsedParameterIndex;
		int modulationSourceParameterIndex;
	};

	ModulatorOnOffButtonListener* modulatorOnOffButtonListener;
	std::vector<ModulatorPanelInfo> modulatorPanelInfos;
	CScrollView* modulatorsScrollView = NULL;

	CFrameResizer* frameResizer = NULL;

	bool hasResized = false;
	int desiredFrameWidth = 0;
	int desiredFrameHeight = 0;
};

#endif

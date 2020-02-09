#include <WaveSabreVstLib/CEnhancedKnob.h>
#include <WaveSabreVstLib/VstEditor.h>

namespace WaveSabreVstLib
{
	static const int cAutoHideDelayDurationMs = 500; // in Ms.


	CEnhancedKnob::CEnhancedKnob(const CRect& size, CEnhancedKnobListener* listener, long tag, CBitmap* background, const CPoint& offset) :
		CAnimKnob(size, listener, tag, background, offset)
	{
		setListener(new CValueChangeListener(this, listener));

		initValueLabel(size);
	}


	CEnhancedKnob::CEnhancedKnob(const CRect& size, CEnhancedKnobListener* listener, long tag, long subPixmaps, CCoord heightOfOneImage, CBitmap* background, const CPoint& offset) :
		CAnimKnob(size, listener, tag, subPixmaps, heightOfOneImage, background, offset)
	{
		setListener(new CValueChangeListener(this, listener));

		initValueLabel(size);
	}


	CEnhancedKnob::CEnhancedKnob(const CEnhancedKnob& knob) :
		CAnimKnob(knob)
	{
		((CValueChangeListener*)knob.getListener())->setKnob(this);

		initValueLabel(knob.getViewSize());
	}


	CEnhancedKnob::~CEnhancedKnob()
	{
		delete getListener();
		setListener(nullptr);
		valueLabelHideTimer->forget();
	}


	void CEnhancedKnob::initValueLabel(const CRect& knobSize)
	{
		const int cValueLabelVertOffset = 22;
		const int cValueLabelWidth = 70;
		const int cValueLabelHeight = 12;
		CPoint valueLabelCenter((knobSize.left + knobSize.right) / 2, knobSize.bottom + cValueLabelVertOffset);
		CRect valueLabelRect(valueLabelCenter.x - cValueLabelWidth / 2, valueLabelCenter.y - cValueLabelHeight / 2, valueLabelCenter.x + cValueLabelWidth / 2, valueLabelCenter.y + cValueLabelHeight / 2);

		valueLabel = new CTextLabel(valueLabelRect);
		valueLabel->setVisible(false);
		shouldValueLabelBeVisible = false;

		valueLabelHideTimer = new CVSTGUITimer(this, cAutoHideDelayDurationMs);

		updateValueLabel();
	}


	bool CEnhancedKnob::attached(CView* parent)
	{
		bool res = CAnimKnob::attached(parent);

		CViewContainer* container = (CViewContainer*)parent;
		container->addView(valueLabel);

		// Refresh our value label's text
		updateValueLabel();

		return res;
	}

	CMouseEventResult CEnhancedKnob::onMouseDown(CPoint& where, const long& buttons)
	{
		CMouseEventResult res = CAnimKnob::onMouseDown(where, buttons);
		setValueLabelVisible(true);
		updateValueLabel();
		return res;
	}

	CMouseEventResult CEnhancedKnob::onMouseUp(CPoint& where, const long& buttons)
	{
		setValueLabelVisible(false);
		return CAnimKnob::onMouseUp(where, buttons);
	}

	bool CEnhancedKnob::onWheel(const CPoint& where, const float& distance, const long& buttons)
	{
		bool handled = CAnimKnob::onWheel(where, distance, buttons);

		if (handled && !shouldValueLabelBeVisible)
		{
			setValueLabelVisible(true);
			setValueLabelVisible(false);
		}
		
		return handled;
	}

	long CEnhancedKnob::onKeyDown(VstKeyCode& keyCode)
	{
		long result = CAnimKnob::onKeyDown(keyCode);

		if (result != -1 && !shouldValueLabelBeVisible)
		{
			setValueLabelVisible(true);
			setValueLabelVisible(false);
		}

		return result;
	}

	void CEnhancedKnob::updateValueLabel()
	{
		char txt[256];
		strcpy(txt, "?");

		CValueChangeListener* knobListener = (CValueChangeListener*)getListener();
		if (knobListener != nullptr && knobListener->getOtherListener() != nullptr)
			knobListener->getOtherListener()->onGetKnobValueText(this, txt);

		valueLabel->setText(txt);
	}



	void CEnhancedKnob::setValueLabelVisible(bool visible)
	{
		if (visible == shouldValueLabelBeVisible)
			return;

		shouldValueLabelBeVisible = visible;
		valueLabel->setVisible(true); // Always set to visible, as our timer will hide it afterwards
		if (valueLabel->getParentView() != nullptr)
			valueLabel->getParentView()->invalid();

		// Stop any timer that's (still) active
		valueLabelHideTimer->stop();

		if (!visible)
		{
			valueLabelHideTimer->setFireTime(cAutoHideDelayDurationMs);
			valueLabelHideTimer->start();
		}
	}


	CMessageResult CEnhancedKnob::notify(CBaseObject* sender, const char* msg)
	{
		if (msg == CVSTGUITimer::kMsgTimer)
		{
			if (!shouldValueLabelBeVisible)
			{
				valueLabel->setVisible(false);
				valueLabelHideTimer->stop();
				if (valueLabel->getParentView() != nullptr)
					valueLabel->getParentView()->invalid();
			}				
			return kMessageNotified;
		}
		return kMessageUnknown;
	}


	void CEnhancedKnob::CValueChangeListener::valueChanged(VSTGUI::CControl* pControl)
	{
		knob->updateValueLabel();

		if (otherListener != nullptr)
			otherListener->valueChanged(pControl);
	}
}


#pragma once

#include "Common.h"

namespace WaveSabreVstLib
{
	class CEnhancedKnob;

	class CEnhancedKnobListener : public CControlListener
	{
	public:
		virtual void onGetKnobValueText(CEnhancedKnob* knob, char* text) = 0;
	};


	class CEnhancedKnob : public CAnimKnob
	{
	public:
		CEnhancedKnob(const CRect& size, CEnhancedKnobListener* listener, long tag, CBitmap* background, const CPoint& offset = CPoint(0, 0));
		CEnhancedKnob(const CRect& size, CEnhancedKnobListener* listener, long tag, long subPixmaps, CCoord heightOfOneImage, CBitmap* background, const CPoint& offset = CPoint(0, 0));
		CEnhancedKnob(const CEnhancedKnob& knob);
		virtual ~CEnhancedKnob();

		virtual bool attached(CView* parent);														///< view is attached to a parent view

		virtual CMouseEventResult onMouseDown(CPoint& where, const long& buttons);
		virtual CMouseEventResult onMouseUp(CPoint& where, const long& buttons);
		virtual bool onWheel(const CPoint& where, const float& distance, const long& buttons);
		virtual long onKeyDown(VstKeyCode& keyCode);

		virtual CMessageResult notify(CBaseObject* sender, const char* msg);

		CLASS_METHODS(CEnhancedKnob, CAnimKnob)

	protected:

		class CValueChangeListener : public CControlListener
		{
		public:
			CValueChangeListener(CEnhancedKnob* knob, CEnhancedKnobListener* otherListener) : knob(knob), otherListener(otherListener) {}

			void					setKnob(CEnhancedKnob* newKnob) { knob = newKnob; }
			virtual void			valueChanged(VSTGUI::CControl* pControl);
			CEnhancedKnobListener*	getOtherListener() const		{ return otherListener; }
		private:
			CEnhancedKnob*			knob;
			CEnhancedKnobListener*	otherListener;
		};

		void initValueLabel(const CRect& knobSize);
		void updateValueLabel();
		void setValueLabelVisible(bool visible);

		CTextLabel* valueLabel = nullptr;
		bool shouldValueLabelBeVisible = true;
		CVSTGUITimer* valueLabelHideTimer = nullptr;
	};
}
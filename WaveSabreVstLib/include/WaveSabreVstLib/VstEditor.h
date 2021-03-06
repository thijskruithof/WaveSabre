#ifndef __WAVESABREVSTLIB_VSTEDITOR_H__
#define __WAVESABREVSTLIB_VSTEDITOR_H__

#include "Common.h"
#include "ImageManager.h"
#include "NoTextCOptionMenu.h"

#include <WaveSabreVstLib/CEnhancedKnob.h>

namespace WaveSabreVstLib
{
	class VstEditor : public AEffGUIEditor, public CEnhancedKnobListener
	{
	public:
		VstEditor(AudioEffect *audioEffect, int width, int height, std::string title);
		virtual ~VstEditor();

		virtual void Open();
		virtual void Close();

		virtual void setParameter(VstInt32 index, float value);
		virtual void valueChanged(CControl *control);

		virtual void onGetKnobValueText(CEnhancedKnob* knob, char* text);

		virtual bool open(void *ptr);
		virtual void close();

	protected:
		void startNextRow();
		void registerControl(int param, CControl* control);

		CTextLabel *addTextLabel(int x, int y, int w, int h, std::string text, CFontRef fontId = kNormalFontVeryBig, CHoriTxtAlign textAlign = kLeftText);

		CAnimKnob *addKnob(VstInt32 param, std::string caption);
		CKickButton *addButton(VstInt32 param, std::string caption);
		NoTextCOptionMenu *addOptionMenu(std::string caption);

		void addSpacer();
        void addSpace(int width);

	private:
		void showAbout();

		CKickButton* aboutButton;

		std::string title;
		int currentX, currentY, currentRow;
		int maxX;

		std::map<VstInt32, CControl *> controls;
	};
}

#endif

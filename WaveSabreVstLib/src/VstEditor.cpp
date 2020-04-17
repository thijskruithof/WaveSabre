#include <WaveSabreVstLib/VstEditor.h>
#include <WaveSabreVstLib/VstPlug.h>

using namespace std;

namespace WaveSabreVstLib
{
	const int BaseSize = 20;
	const int LeftMargin = BaseSize;

	const int BuildVersionHeight = 20;
	const int BuildVersionTopMargin = 30;

	const int TitleTopMargin = BaseSize / 2;
	const int TitleAreaHeight = BaseSize * 2 + BuildVersionHeight * 2;

	const int RowHeight = BaseSize * 3;

	const int KnobWidth = 55;
	const int KnobKnobOffset = 12;
	const int KnobCaptionWidth = 100;
	const int KnobCaptionOffset = 30;

	const int ButtonWidth = 55;
	const int ButtonButtonOffset = 18;
	const int ButtonCaptionWidth = KnobCaptionWidth;
	const int ButtonCaptionOffset = 14;

	const int OptionMenuWidth = 100;
	const int OptionMenuButtonOffset = 0;
	const int OptionMenuCaptionWidth = 120;
	const int OptionMenuCaptionOffset = 18;

	const int SpacerWidth = BaseSize;

	VstEditor::VstEditor(AudioEffect *audioEffect, int width, int height, string title)
		: AEffGUIEditor(audioEffect)
		, title(title)
	{
		setKnobMode(kLinearMode);

		frame = nullptr;

		rect.left = rect.top = 0;
		rect.right = width;
		rect.bottom = height + BuildVersionHeight * 2;
	}

	VstEditor::~VstEditor()
	{
	}

	void VstEditor::Open()
	{
	}

	void VstEditor::Close()
	{
	}

	void VstEditor::setParameter(VstInt32 index, float value)
	{
		if (!frame) return;
		if (controls.find(index) != controls.end()) controls[index]->setValue(effect->getParameter(index));
	}

	void VstEditor::valueChanged(CControl *control)
	{
		if (control == aboutButton)
		{
			if (((CKickButton*)control)->getValue() == 1.0f)
				showAbout();
			return;
		}

		effect->setParameterAutomated(control->getTag(), control->getValue());
		control->setDirty();
	}

	const char* sAboutText =
		"https://github.com/thijskruithof/WaveSabre\n"
		"\n"
		"\n"
		"Copyright for portions of project WaveSabre (forked by Inque) are held by WabeSabre Team 2012-2019 as part of project WaveSabre. All other copyright for project WaveSabre (forked by Inque) are held by Inque, 2020.\n"
		"\n"
		"Permission is hereby granted, free of charge, to any person obtaining a copy of this softwareand associated documentation files(the 'Software'), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and /or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :\n"
		"\n"
		"The above copyright noticeand this permission notice shall be included in all copies or substantial portions of the Software.\n"
		"\n"
		"THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n"
		"\n"
		"VST PlugIn Technology by Steinberg Media Technologies. VST is a trademark and software of Steinberg Media Technologies GmbH.\n";

	void VstEditor::showAbout()
	{
		MessageBoxA((HWND)systemWindow, sAboutText, "About", MB_ICONINFORMATION | MB_OK);
	}

	bool VstEditor::open(void *ptr)
	{
		AEffGUIEditor::open(ptr);

		controls.clear();

		CRect size(rect.left, rect.top, rect.right, rect.bottom);
		frame = new CFrame(size, ptr, this);

		frame->setBackground(ImageManager::Get(ImageManager::ImageIds::Background));
		addTextLabel(LeftMargin, TitleTopMargin, 200, BaseSize, title);

		// WaveSabre name and build
		const int build_label_width = 110;
		char build_label_text[256];
		build_label_text[0] = 0;
		((VstPlug*)effect)->getEffectBuildNumberString(build_label_text);
		CTextLabel* brand_label = addTextLabel(LeftMargin, BuildVersionTopMargin, build_label_width, BaseSize, "Wave Sabre#Inque", kNormalFont, kLeftText);
		brand_label->setBackColor(VSTGUI::kGreyCColor);
		brand_label->setFrameColor(VSTGUI::kGreyCColor);
		brand_label->setFontColor(VSTGUI::kWhiteCColor);
		brand_label->setTransparency(false);
		addTextLabel(LeftMargin + 2, BuildVersionTopMargin + 20, 300, BaseSize, build_label_text, kNormalFont, kLeftText);

		// About button
		CBitmap* switchImage = ImageManager::Get(ImageManager::ImageIds::TinyButton);

		aboutButton = new CKickButton(CRect(
			LeftMargin + build_label_width + 8,
			BuildVersionTopMargin + (BaseSize - switchImage->getHeight() / 2) / 2,
			LeftMargin + build_label_width + 8 + switchImage->getWidth(),
			BuildVersionTopMargin + (BaseSize - switchImage->getHeight() / 2) / 2 + switchImage->getHeight()/2),
			this,
			-1, // tag
			switchImage->getHeight() / 2,
			switchImage);
		aboutButton->setTransparency(true);
		frame->addView(aboutButton);

		addTextLabel(
			LeftMargin + build_label_width + 10 + switchImage->getWidth(), 
			BuildVersionTopMargin,
			200, 
			BaseSize, 
			"About", 
			kNormalFont, kLeftText);

		currentX = LeftMargin;
		currentY = TitleAreaHeight;
		currentRow = 1;
		maxX = currentX;

		Open();

		return true;
	}

	void VstEditor::close()
	{
		Close();
		if (frame) delete frame;
		frame = 0;
	}

	void VstEditor::startNextRow()
	{
		currentX = LeftMargin;
		currentY += RowHeight;
		currentRow++;
	}

	void VstEditor::registerControl(int param, CControl* control)
	{
		controls.emplace(param, control);
	}

	CTextLabel *VstEditor::addTextLabel(int x, int y, int w, int h, string text, CFontRef fontId, CHoriTxtAlign textAlign)
	{
		if (!frame) return nullptr;

		CRect size(x, y, x + w, y + h);
		CTextLabel *c = new CTextLabel(size, text.c_str());
		c->setFontColor(VSTGUI::kBlackCColor);
		c->setTransparency(true);
		c->setTextTransparency(true);
		c->setHoriAlign(textAlign);
		c->setFont(fontId);
		c->setStyle(kBoldFace);
		frame->addView(c);

		return c;
	}	

	void VstEditor::onGetKnobValueText(CEnhancedKnob* knob, char* text)
	{
		VstInt32 param = knob->getTag();

		VstPlug* plug = (VstPlug*)effect;
		char disp[kVstExtMaxParamStrLen + 1], label[kVstExtMaxParamStrLen + 1];
		plug->getParameterDisplayAndLabel(param, disp, label);
		sprintf_s(text, 256, "%s%s", disp, label);
	}

	CAnimKnob *VstEditor::addKnob(VstInt32 param, string caption)
	{
		if (!frame) return nullptr;

		int x = currentX;
		int y = currentY;

		CBitmap *image = ImageManager::Get(ImageManager::ImageIds::Knob1);

		CRect size(x + KnobKnobOffset, y, x + KnobKnobOffset + image->getWidth(), y + image->getWidth());
		CEnhancedKnob*c = new CEnhancedKnob(size, this, param, image->getHeight() / image->getWidth(), image->getWidth(), image, CPoint(0));
	
		float v = effect->getParameter(param);
		c->setDefaultValue(v);
		c->setValue(v);
		c->setTransparency(true);
		c->setTag(param);
		frame->addView(c);

		controls.emplace(param, c);

		addTextLabel(x + KnobWidth / 2 - KnobCaptionWidth / 2, y + KnobCaptionOffset, KnobCaptionWidth, BaseSize, caption, kNormalFontVerySmall, kCenterText);

		currentX += KnobWidth;
		maxX = max(currentX, maxX);

		return c;
	}

	CKickButton *VstEditor::addButton(VstInt32 param, string caption)
	{
		if (!frame) return nullptr;

		int x = currentX;
		int y = currentY;

		CBitmap *image = ImageManager::Get(ImageManager::ImageIds::TinyButton);

		CRect size(x + ButtonButtonOffset, y, x + ButtonButtonOffset + image->getWidth(), y + image->getHeight() / 2);
		CKickButton *c = new CKickButton(size, this, param, image->getHeight() / 2, image, CPoint(0));
		c->setTransparency(true);
		frame->addView(c);

		controls.emplace(param, c);

		addTextLabel(x + ButtonWidth / 2 - ButtonCaptionWidth / 2, y + ButtonCaptionOffset, ButtonCaptionWidth, BaseSize, caption, kNormalFontVerySmall, kCenterText);

		currentX += ButtonWidth;
		maxX = max(currentX, maxX);

		return c;
	}

	NoTextCOptionMenu *VstEditor::addOptionMenu(string caption)
	{
		if (!frame) return nullptr;

		int x = currentX;
		int y = currentY;

		CBitmap *unpressedImage = ImageManager::Get(ImageManager::ImageIds::OptionMenuUnpressed);
		CBitmap *pressedImage = ImageManager::Get(ImageManager::ImageIds::OptionMenuPressed);

		CRect size(x + OptionMenuButtonOffset, y, x + OptionMenuButtonOffset + unpressedImage->getWidth(), y + unpressedImage->getHeight());

		// So COptionMenu is fucking useless and doesn't help us _at all_ to propagate messages from its
		//  submenus. This causes all kinds of "fun" things like the caption being completely bogus, the
		//  pressed image behaving strangely when items from child menus are selected, etc. So, we do some
		//  pretty weird stuff when using this, but it all makes sense, somehow..
		auto c = new NoTextCOptionMenu(size, this, -1, unpressedImage/*, pressedImage*/);

		c->setFontColor(VSTGUI::kBlackCColor);
		c->setTransparency(true);
		c->setTextTransparency(true);
		c->setStyle(kBoldFace);
		frame->addView(c);

		addTextLabel(x + OptionMenuWidth / 2 - OptionMenuCaptionWidth / 2, y + OptionMenuCaptionOffset, OptionMenuCaptionWidth, BaseSize, caption, kNormalFontVerySmall, kCenterText);

		currentX += OptionMenuWidth;
		maxX = max(currentX, maxX);

		return c;
	}

	void VstEditor::addSpacer()
	{
        addSpace(SpacerWidth);
	}

    void VstEditor::addSpace(int width)
    {
        currentX += width;
        maxX = max(currentX, maxX);
    }
}

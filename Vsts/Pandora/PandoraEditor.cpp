#include "PandoraEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

PandoraEditor::PandoraEditor(AudioEffect *audioEffect)
	: VstEditor(audioEffect, 920, 720, "PANDORA")
{
}

PandoraEditor::~PandoraEditor()
{
}

void PandoraEditor::Open()
{
	addKnob((VstInt32)Pandora::ParamIndices::Osc1waveform, "Osc1wave");
	addKnob((VstInt32)Pandora::ParamIndices::Osc2waveform, "Osc2wave");
	addKnob((VstInt32)Pandora::ParamIndices::Osc3waveform, "Osc3wave");
	addKnob((VstInt32)Pandora::ParamIndices::Osc1baseToneTranspose, "Osc1base");
	addKnob((VstInt32)Pandora::ParamIndices::Osc2baseToneTranspose, "Osc2base");
	addKnob((VstInt32)Pandora::ParamIndices::Osc3baseToneTranspose, "Osc3base");
	addKnob((VstInt32)Pandora::ParamIndices::Osc1finetune, "Osc1fine");
	addKnob((VstInt32)Pandora::ParamIndices::Osc2finetune, "Osc2fine");
	addKnob((VstInt32)Pandora::ParamIndices::Osc3finetune, "Osc3fine");
	addKnob((VstInt32)Pandora::ParamIndices::Osc1pulseWidth, "Osc1pw");
	addKnob((VstInt32)Pandora::ParamIndices::Osc2pulseWidth, "Osc2pw");
	addKnob((VstInt32)Pandora::ParamIndices::Osc3pulseWidth, "Osc3pw");
	addKnob((VstInt32)Pandora::ParamIndices::MixAmountOsc1, "MixOsc1");
	addKnob((VstInt32)Pandora::ParamIndices::MixAmountOsc2, "MixOsc2");
	addKnob((VstInt32)Pandora::ParamIndices::MixAmountOsc3, "MixOsc3");
	addKnob((VstInt32)Pandora::ParamIndices::Osc2sync, "Osc2sync");

	startNextRow();

	addKnob((VstInt32)Pandora::ParamIndices::StringDamping, "StringDamp");
	addKnob((VstInt32)Pandora::ParamIndices::StringFeedbackAmount, "StringFB");
	addKnob((VstInt32)Pandora::ParamIndices::StringFeedbackDelay, "StringFBDelay");
	addKnob((VstInt32)Pandora::ParamIndices::StringThickness, "StringThick");
	addKnob((VstInt32)Pandora::ParamIndices::StringLevel, "StringLvl");
	addKnob((VstInt32)Pandora::ParamIndices::OscStringMix, "OscStringMix");

	startNextRow();

	addKnob((VstInt32)Pandora::ParamIndices::Lfo1rate, "Lfo1rate");
	addKnob((VstInt32)Pandora::ParamIndices::Lfo2rate, "Lfo2rate");
	addKnob((VstInt32)Pandora::ParamIndices::Lfo3rate, "Lfo3rate");
	addKnob((VstInt32)Pandora::ParamIndices::Lfo1waveform, "Lfo1wave");
	addKnob((VstInt32)Pandora::ParamIndices::Lfo2waveform, "Lfo2wave");
	addKnob((VstInt32)Pandora::ParamIndices::Lfo3waveform, "Lfo3wave");
	addKnob((VstInt32)Pandora::ParamIndices::Lfo1keysync, "Lfo1keysync");
	addKnob((VstInt32)Pandora::ParamIndices::Lfo2keysync, "Lfo2keysync");
	addKnob((VstInt32)Pandora::ParamIndices::Lfo3keysync, "Lfo3keysync");
	addKnob((VstInt32)Pandora::ParamIndices::Lfo1positive, "Lfo1positive");
	addKnob((VstInt32)Pandora::ParamIndices::Lfo2positive, "Lfo2positive");
	addKnob((VstInt32)Pandora::ParamIndices::Lfo3positive, "Lfo3positive");

	startNextRow();

	addKnob((VstInt32)Pandora::ParamIndices::Envelope1attackRate, "Env1attack");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope1decayRate, "Env1decay");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope1sustainLevel, "Env1sus");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope1releaseRate, "Env1rel");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope2attackRate, "Env2attack");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope2decayRate, "Env2decay");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope2sustainLevel, "Env2sus");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope2releaseRate, "Env2rel");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope3attackRate, "Env3attack");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope3decayRate, "Env3decay");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope3sustainLevel, "Env3sus");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope3releaseRate, "Env3rel");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope4attackRate, "Env4attack");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope4decayRate, "Env4decay");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope4sustainLevel, "Env4sus");
	addKnob((VstInt32)Pandora::ParamIndices::Envelope4releaseRate, "Env4rel");

	startNextRow();

	addKnob((VstInt32)Pandora::ParamIndices::VcfRouting, "VcfRouting");
	addKnob((VstInt32)Pandora::ParamIndices::Vcf1type, "Vcf1type");
	addKnob((VstInt32)Pandora::ParamIndices::Vcf1Cutoff, "Vcf1Cutoff");
	addKnob((VstInt32)Pandora::ParamIndices::Vcf1Resonance, "Vcf1Reso");
	addKnob((VstInt32)Pandora::ParamIndices::Vcf2type, "Vcf2type");
	addKnob((VstInt32)Pandora::ParamIndices::Vcf2Cutoff, "Vcf2Cutoff");
	addKnob((VstInt32)Pandora::ParamIndices::Vcf2Resonance, "Vcf2Reso");
	addKnob((VstInt32)Pandora::ParamIndices::VcfCtrlBalance, "VcfBalance");
	addKnob((VstInt32)Pandora::ParamIndices::Vcf2CutoffRelative, "Vcf2CutoffRel");

	startNextRow();

	addKnob((VstInt32)Pandora::ParamIndices::VcfDistType, "VcfDistType");
	addKnob((VstInt32)Pandora::ParamIndices::FilterDistDrive, "VcfDistDrive");
	addKnob((VstInt32)Pandora::ParamIndices::FilterDistShape, "VcfDistShape");

	addSpacer();

	addKnob((VstInt32)Pandora::ParamIndices::DoSlide, "DoSlide");
	addKnob((VstInt32)Pandora::ParamIndices::SlideSpeed, "SlideSpeed");

	addSpacer();

	addKnob((VstInt32)Pandora::ParamIndices::NumUnisonVoices, "UnisonVoices");
	addKnob((VstInt32)Pandora::ParamIndices::UnisonSpread, "UnisonSpread");

	startNextRow();

	addKnob((VstInt32)Pandora::ParamIndices::ArpeggioType, "ArpType");
	addKnob((VstInt32)Pandora::ParamIndices::ArpeggioNumOctaves, "ArpNumOctaves");
	addKnob((VstInt32)Pandora::ParamIndices::ArpeggioInterval, "ArpInterval");
	addKnob((VstInt32)Pandora::ParamIndices::ArpeggioNoteDuration, "ArpNoteDur");

	addSpacer();

	addKnob((VstInt32)Pandora::ParamIndices::Pan, "Pan");

	addModulatorControls();
}


const char* sPandoraModulatorParamName[(int)Pandora::ModulatorParamIndices::COUNT] = {
	"Source",
	"DepthSource",
	"Constant",
	"Range"
};

const char* sPandoraModulatorDestNames[(int)Pandora::ModulationDestType::COUNT] = {
	"OSC1TUNE",
	"OSC2TUNE",
	"VCF1CUTOFF",
	"VCF1RESONANCE",
	"VCF2CUTOFF",
	"VCF2RESONANCE",
	"VCA",
	"OSC3TUNE",
	"OSC1PULSEWIDTH",
	"OSC2PULSEWIDTH",
	"OSC3PULSEWIDTH",
	"MODDEPTHA",
	"MODDEPTHB",
	"MODDEPTHC",
	"MODDEPTHD",
	"OSC1LEVEL",
	"OSC2LEVEL",
	"OSC3LEVEL",
	"STRINGLEVEL",
	"LFO1RATE",
	"LFO2RATE",
	"LFO3RATE"
};


void PandoraEditor::addModulatorControls()
{
	const int cColWidth = 220;
	const int cColHeight = 800;
	const int cColSeperator = 16;
	const int cStartY = 510;
	const int cLeftMargin = 20;
	const int cLabelHeight = 16;
	const int cBottomMargin = 20;

	const int cInnerMargin = 5;
	const int cModRowSeperator = 5;
	const int cModRowHeight = 90;
	const int cModSwitchLblSeperator = 5;
	const int cModLblHeight = 16;
	const int cModMargin = 5;
	const int cModKnobY = 32;
	const int cModKnobCaptionWidth = 100;
	const int cModKnobCaptionOffset = 30;
	const int cModKnobCaptionHeight = 20;
	const int cModKnobOffsetX = 60;

	// Add modulator destinations
	CScrollView* view = new CScrollView(
		CRect(cLeftMargin, cStartY, frame->getWidth() - cLeftMargin, frame->getHeight()-cBottomMargin),
		CRect(0, 0, 2*cInnerMargin + PANDORA_NUM_MODULATOR_DEST * (cColWidth + cColSeperator) - cColSeperator, cColHeight),
		frame, CScrollView::kHorizontalScrollbar | CScrollView::kVerticalScrollbar);
	view->setBackgroundColor({ 225, 225, 225, 255 });
	view->getHorizontalScrollbar()->setScrollerColor(kGreyCColor);
	view->getVerticalScrollbar()->setScrollerColor(kGreyCColor);
	frame->addView(view);

	for (int destIndex = 0; destIndex < PANDORA_NUM_MODULATOR_DEST; ++destIndex)
	{
		// Header
		CRect size(
			cInnerMargin + destIndex * (cColWidth + cColSeperator),
			cInnerMargin,
			cInnerMargin + destIndex * (cColWidth + cColSeperator) + cColWidth,
			cInnerMargin + cLabelHeight);
		CTextLabel* c = new CTextLabel(size, sPandoraModulatorDestNames[destIndex]);
		c->setFontColor(VSTGUI::kBlackCColor);
		c->setBackColor(VSTGUI::kWhiteCColor);
		c->setTransparency(false);
		c->setTextTransparency(true);
		c->setHoriAlign(CHoriTxtAlign::kCenterText);
		c->setFont(kNormalFont);
		c->setStyle(kBoldFace);
		view->addView(c);


		CBitmap* switchImage = ImageManager::Get(ImageManager::ImageIds::TinyButton);
		CBitmap* knobImage = ImageManager::Get(ImageManager::ImageIds::Knob1);

		// All modulators
		int y = cInnerMargin + cLabelHeight + cModRowSeperator;

		for (int modIndex = 0; modIndex < PANDORA_MAX_MODULATORS_PER_DEST; ++modIndex)
		{
			int parameterBaseIndex =
				(int)Pandora::ParamIndices::ModulatorFirstParam +
				modIndex * (int)Pandora::ModulatorParamIndices::COUNT +
				destIndex * PANDORA_MAX_MODULATORS_PER_DEST * (int)Pandora::ModulatorParamIndices::COUNT;

			int rowHeight = cModRowHeight;

			CRect panelRect(
				cInnerMargin + destIndex * (cColWidth + cColSeperator),
				y,
				cInnerMargin + destIndex * (cColWidth + cColSeperator) + cColWidth,
				y + rowHeight
			);

			// Panel
			CViewContainer* panel = new CViewContainer(panelRect, frame);
			panel->setBackgroundColor(kWhiteCColor);
			view->addView(panel);

			// Switch
			COnOffButton* sw = new COnOffButton(CRect(
				cModMargin, 
				cModMargin + (cModLblHeight - switchImage->getHeight() / 2)/2,
				cModMargin + switchImage->getWidth(),
				cModMargin + (cModLblHeight - switchImage->getHeight() / 2)/2 + switchImage->getHeight()/2),
				NULL, 0, switchImage);
			sw->setTransparency(true);
			panel->addView(sw);

			// Label
			char lblText[512];
			sprintf_s(lblText, 512, "Mod %d", modIndex);
			CTextLabel* lbl = new CTextLabel(CRect(cModMargin + switchImage->getWidth() + cModSwitchLblSeperator, cModMargin, panelRect.right, cModMargin+cModLblHeight), lblText);
			lbl->setFontColor(VSTGUI::kBlackCColor);
			lbl->setTransparency(true);
			lbl->setTextTransparency(true);
			lbl->setHoriAlign(CHoriTxtAlign::kLeftText);
			lbl->setFont(kNormalFont);
			lbl->setStyle(kBoldFace);
			panel->addView(lbl);

			// Knobs
			for (int paramIndex = 0; paramIndex < (int)Pandora::ModulatorParamIndices::COUNT; ++paramIndex)
			{
				int parameterIndex = parameterBaseIndex + paramIndex;

				int x = paramIndex * cModKnobOffsetX;

				// Knob
				CEnhancedKnob* knobSource = new CEnhancedKnob(
					CRect(cModMargin + x, cModKnobY, cModMargin + x + knobImage->getWidth(), cModKnobY + knobImage->getWidth()),
					this,
					parameterIndex,
					knobImage->getHeight() / knobImage->getWidth(), knobImage->getWidth(), knobImage, CPoint(0));
				float v = effect->getParameter(parameterIndex);
				knobSource->setDefaultValue(v);
				knobSource->setValue(v);
				knobSource->setTransparency(true);
				knobSource->setTag(parameterIndex);
				panel->addView(knobSource);
				registerControl(parameterIndex, knobSource);

				// Value label
				CTextLabel* c = new CTextLabel(
					CRect(cModMargin + x + knobImage->getWidth() / 2 - cModKnobCaptionWidth / 2,
						cModKnobY + cModKnobCaptionOffset,
						cModMargin + x + knobImage->getWidth() / 2 + cModKnobCaptionWidth / 2,
						cModKnobY + cModKnobCaptionOffset + cModKnobCaptionHeight),
					sPandoraModulatorParamName[paramIndex]);
				c->setFontColor(VSTGUI::kBlackCColor);
				c->setTransparency(true);
				c->setTextTransparency(true);
				c->setHoriAlign(kCenterText);
				c->setFont(kNormalFontVerySmall);
				c->setStyle(kBoldFace);
				panel->addView(c);
			}

			y += rowHeight + cModRowSeperator;
		}

	}

	VstEditor::Open();
}

#include "OptionAVState.h"
#include "../SDL/Sprite.h"
#include "../Text/Text.h"
#include "../Audio/Audio.h"
#include "../Input/InputCMD.h"
#include <fstream>
using namespace Input;

const Time kdPercentRate = 0.1f;

COptionAVState::COptionAVState(void)
{
	this->m_eType = EGameStateType::gs_OPTIONS_AV;
	this->m_pStarField = NULL;
	this->m_dPercentTimer = -1.0f;
	this->m_sfxTick = -1;
	this->m_cGammaButton.SetPercent(0.5f);
}
COptionAVState::~COptionAVState(void)
{
}

void COptionAVState::Enter(Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText, Audio::CAudio* pAudio)
{
	IGameState::Enter(pSprite, pInput, pText, pAudio);

	this->m_sfxTick = this->m_pAudio->LoadAudio("Assets/Sfx/beep_good.wav", Audio::EAudioType::SFX_2D);
	slong sfxBack = this->m_pAudio->LoadAudio("Assets/Sfx/back.wav", Audio::EAudioType::SFX_2D);
	slong sfxPush = this->m_pAudio->LoadAudio("Assets/Sfx/push.wav", Audio::EAudioType::SFX_2D);
	slong sfxPull = this->m_pAudio->LoadAudio("Assets/Sfx/pull.wav", Audio::EAudioType::SFX_2D);
	slong sfxHover = this->m_pAudio->LoadAudio("Assets/Sfx/hover.wav", Audio::EAudioType::SFX_2D);

	//Button vec
	std::vector<GUI::CButton*> vButs;

#pragma region PRIMARY
	for (ulong i = 0; i < NUM_BUTTONS; i++)
	{
		m_vButtons[i].DefaultColor(CLR::CLEAR);
		m_vButtons[i].SetTextColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
		m_vButtons[i].SetTextColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
		m_vButtons[i].SetTextColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);

		vButs.push_back(&m_vButtons[i]);
	}

	//Back Button
	Type2<ulong> tSize;
	tSize = this->m_pText->GetSize("Back", 1.0f);
	m_vButtons[BACK].SetText("Back");
	m_vButtons[BACK].SetRect(_RECT<slong>(Sprite::SCREEN_WIDTH - tSize.x - 32, 0, tSize.x, tSize.y));
	m_vButtons[BACK].SetSfx(sfxHover, GUI::CButton::EButtonSfx::SFX_HOVER);
	m_vButtons[BACK].SetSfx(sfxPush, GUI::CButton::EButtonSfx::SFX_PUSH);
	m_vButtons[BACK].SetSfx(sfxBack, GUI::CButton::EButtonSfx::SFX_PULL);

	//Group
	this->m_vButtonGroup[PRIMARY].SetButtonList(vButs);
	this->m_vButtonGroup[PRIMARY].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[PRIMARY].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[PRIMARY].SetInputCMD(this->m_pInput);
	this->m_vButtonGroup[PRIMARY].SetButtonGroupDir(LEFT, VIDEO);
	this->m_vButtonGroup[PRIMARY].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
	vButs.clear();
#pragma endregion
#pragma region AUDIO
	for (ulong i = 0; i < NUM_AUDIO_BUTTONS; i++)
	{
		this->m_vAudioButtons[i].DefaultColor(CLR::CLEAR);
		this->m_vAudioButtons[i].SetRect(_RECT<slong>(480, 720 - i * 180, 960, 128));

		//Background
		this->m_vAudioButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::DEFAULT);
		this->m_vAudioButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::HOVER);
		this->m_vAudioButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::MDOWN);
		//Outline
		this->m_vAudioButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
		this->m_vAudioButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
		this->m_vAudioButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
		//BarL
		this->m_vAudioButtons[i].SetBarColor(CLR::GREEN * 0.50f, GUI::CButton::EButtonState::DEFAULT);
		this->m_vAudioButtons[i].SetBarColor(CLR::GREEN * 0.40f, GUI::CButton::EButtonState::HOVER);
		this->m_vAudioButtons[i].SetBarColor(CLR::GREEN * 0.30f, GUI::CButton::EButtonState::MDOWN);
		//Slider
		this->m_vAudioButtons[i].SetSliderColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
		this->m_vAudioButtons[i].SetSliderColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
		this->m_vAudioButtons[i].SetSliderColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
		//Outline Size
		this->m_vAudioButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::DEFAULT);
		this->m_vAudioButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::HOVER);
		this->m_vAudioButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::MDOWN);

		this->m_vAudioButtons[i].SetSfx(sfxHover, GUI::CButton::EButtonSfx::SFX_HOVER);
		vButs.push_back(&m_vAudioButtons[i]);
	}

	//Group
	this->m_vButtonGroup[AUDIO].SetButtonList(vButs);
	this->m_vButtonGroup[AUDIO].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[AUDIO].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[AUDIO].SetInputCMD(this->m_pInput);
	this->m_vButtonGroup[AUDIO].SetButtonGroupDir(DOWN, VIDEO);
	this->m_vButtonGroup[AUDIO].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
	vButs.clear();
#pragma endregion
#pragma region VIDEO
	//Gamma
	{
		this->m_cGammaButton.DefaultColor(CLR::CLEAR);
		this->m_cGammaButton.SetRect(_RECT<slong>(480, 180, 960, 128));

		//Background
		this->m_cGammaButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::DEFAULT);
		this->m_cGammaButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::HOVER);
		this->m_cGammaButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::MDOWN);
		//Outline
		this->m_cGammaButton.SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
		this->m_cGammaButton.SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
		this->m_cGammaButton.SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
		//BarL
		this->m_cGammaButton.SetBarColor(CLR::GREEN * 0.50f, GUI::CButton::EButtonState::DEFAULT);
		this->m_cGammaButton.SetBarColor(CLR::GREEN * 0.40f, GUI::CButton::EButtonState::HOVER);
		this->m_cGammaButton.SetBarColor(CLR::GREEN * 0.30f, GUI::CButton::EButtonState::MDOWN);
		//Slider
		this->m_cGammaButton.SetSliderColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
		this->m_cGammaButton.SetSliderColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
		this->m_cGammaButton.SetSliderColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
		//Outline Size
		this->m_cGammaButton.SetOutlineSize(6, GUI::CButton::EButtonState::DEFAULT);
		this->m_cGammaButton.SetOutlineSize(6, GUI::CButton::EButtonState::HOVER);
		this->m_cGammaButton.SetOutlineSize(6, GUI::CButton::EButtonState::MDOWN);

		this->m_cGammaButton.SetButtonGroupDir(-2, RIGHT);
		this->m_cGammaButton.SetButtonGroupDir(-2, LEFT);
		this->m_cGammaButton.SetSfx(sfxHover, GUI::CButton::EButtonSfx::SFX_HOVER);

		vButs.push_back(&this->m_cGammaButton);
	}
	//Toggle Full Screen
	{
		this->m_cFullscreenButton.DefaultColor(CLR::CLEAR);
		this->m_cFullscreenButton.SetRect(_RECT<slong>(1344, 34, 96, 96));
		this->m_cFullscreenButton.SetToggle(this->m_pSprite->GetFullScreen());
		if (!this->m_pSprite->GetFullScreen())
			this->m_cFullscreenButton.SetText("");
		else
			this->m_cFullscreenButton.SetText("X");
		//Background
		this->m_cFullscreenButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::ON);
		this->m_cFullscreenButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::ON_HOVER);
		this->m_cFullscreenButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_cFullscreenButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF);
		this->m_cFullscreenButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_cFullscreenButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_MDOWN);
		//Outline
		this->m_cFullscreenButton.SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::ON);
		this->m_cFullscreenButton.SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::ON_HOVER);
		this->m_cFullscreenButton.SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_cFullscreenButton.SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::OFF);
		this->m_cFullscreenButton.SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_cFullscreenButton.SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::OFF_MDOWN);
		//Text
		this->m_cFullscreenButton.SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON);
		this->m_cFullscreenButton.SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_HOVER);
		this->m_cFullscreenButton.SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_cFullscreenButton.SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::OFF);
		this->m_cFullscreenButton.SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_cFullscreenButton.SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::OFF_MDOWN);
		//Outline Size
		this->m_cFullscreenButton.SetOutlineSize(6, GUI::CButton::EButtonState::ON);
		this->m_cFullscreenButton.SetOutlineSize(6, GUI::CButton::EButtonState::ON_HOVER);
		this->m_cFullscreenButton.SetOutlineSize(6, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_cFullscreenButton.SetOutlineSize(6, GUI::CButton::EButtonState::OFF);
		this->m_cFullscreenButton.SetOutlineSize(6, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_cFullscreenButton.SetOutlineSize(6, GUI::CButton::EButtonState::OFF_MDOWN);

		this->m_cFullscreenButton.SetButtonGroupDir(-2, LEFT);
		Type2<ulong> size = this->m_pText->GetSize("0", 1.0f);
		this->m_cFullscreenButton.SetTextOffset(Type2<slong>(0, -(slong)(size.h * 0.22f)));

		this->m_cFullscreenButton.SetSfx(sfxHover, GUI::CButton::EButtonSfx::SFX_HOVER);
		this->m_cFullscreenButton.SetSfx(sfxPush, GUI::CButton::EButtonSfx::SFX_PUSH);
		//this->m_cFullscreenButton.SetSfx(sfxPull, GUI::CButton::EButtonSfx::SFX_PULL);


		vButs.push_back(&this->m_cFullscreenButton);
	}


	//Group
	this->m_vButtonGroup[VIDEO].SetButtonList(vButs);
	this->m_vButtonGroup[VIDEO].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[VIDEO].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[VIDEO].SetInputCMD(this->m_pInput);
	this->m_vButtonGroup[VIDEO].SetButtonGroupDir(UP, AUDIO);
	this->m_vButtonGroup[VIDEO].SetButtonGroupDir(RIGHT, PRIMARY);
	this->m_vButtonGroup[VIDEO].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
	vButs.clear();
#pragma endregion

	//Build the Button  manager
	std::vector<GUI::CButtonGroup> vGroups;
	for (ulong i = 0; i < NUM_BUTTON_GROUPS; i++)
		vGroups.push_back(this->m_vButtonGroup[i]);
	this->m_cButMan.SetGroups(vGroups);

	this->LoadAudio();
}
void COptionAVState::Exit(void)
{
	this->SaveAudio();
}

void COptionAVState::Input(void)
{
	Input::eInputType eType = this->m_pInput->GetFinalInputType();
	this->m_pInput->Cursor_Lock(eType != MOUSE);

	//Selection
	{
		this->m_cButMan.Selection(this->m_pInput);
		if (this->m_pInput->Ctrl_Push(CTRL_KEYS::XB_RBUMP))
			this->m_cButMan.MoveGroups(RIGHT);
		if (this->m_pInput->Ctrl_Push(CTRL_KEYS::XB_LBUMP))
			this->m_cButMan.MoveGroups(LEFT);
	}
	//Confirmation
	{
		bool confirm = false;
		if (eType == Input::eInputType::MOUSE)
			confirm = this->m_pInput->Key_Held(VK_LBUTTON);
		else
			confirm = this->m_pInput->Key_Held(VK_RETURN) || this->m_pInput->Key_Held(VK_SPACE) || this->m_pInput->Ctrl_Held(CTRL_KEYS::XB_A) || this->m_pInput->Ctrl_Held(CTRL_KEYS::START);

		for (ulong i = 0; i < NUM_BUTTON_GROUPS; i++)
			this->m_vButtonGroup[i].Confirmation(confirm);
	}
	//Results
	{
		InputPrimary();
		InputAudio();
		InputVideo();
	}
}
void COptionAVState::Update(Time dDelta, Time gameTime)
{
	this->m_pStarField->Update(dDelta);
	this->m_cButMan.Update(dDelta, this->m_pAudio);

	if (this->m_dPercentTimer > 0.0f)
		this->m_dPercentTimer -= dDelta;

	//In case we ALT + ENTER full screen
	if (this->m_pSprite->GetFullScreen() != this->m_cFullscreenButton.GetToggle())
	{
		this->m_cFullscreenButton.SetToggle(this->m_pSprite->GetFullScreen());
		if (!this->m_pSprite->GetFullScreen())
			this->m_cFullscreenButton.SetText("");
		else
			this->m_cFullscreenButton.SetText("X");
	}
}
void COptionAVState::Render(void)
{
	this->m_pStarField->Render();
	this->RenderButtons();
	this->RenderText();
}

//Input
void COptionAVState::InputPrimary(void)
{
	if (this->m_vButtons[BACK].GetClick().Pull())
		this->m_tResult.Prev(IGameState::EGameStateType::gs_OPTIONS);
}
void COptionAVState::InputAudio(void)
{
	if (this->m_pInput->GetFinalInputType() != MOUSE && (this->m_vAudioButtons[SFX].GetState() == GUI::CButton::EButtonState::HOVER || this->m_vAudioButtons[SFX].GetState() == GUI::CButton::EButtonState::MDOWN))
	{
		VecComp lThumb = this->m_pInput->Ctrl_Stick(CTRL_KEYS::XB_LTHUMB);
		float dir = 0;
		//Push (inc or dec by 1 percent)
		if (this->m_pInput->Key_Push(VK_RIGHT) || this->m_pInput->Ctrl_Push(CTRL_KEYS::d_RIGHT))
			dir += 0.01f;
		if (this->m_pInput->Key_Push(VK_LEFT) || this->m_pInput->Ctrl_Push(CTRL_KEYS::d_LEFT))
			dir -= 0.01f;
		
		//While held, and not pushed (dir == 0.0f)
		if (dir == 0.0f)
		{
			//Right
			if (this->m_pInput->Key_Held(VK_RIGHT) && this->m_pInput->Key_Time(VK_RIGHT) > 0.1f)
				dir += pow(float(this->m_pInput->Key_Time(VK_RIGHT)) * 0.1f, 2);
			else if (this->m_pInput->Ctrl_Held(CTRL_KEYS::d_RIGHT) && this->m_pInput->Ctrl_Time(CTRL_KEYS::d_RIGHT) > 0.1f)
				dir += pow(float(this->m_pInput->Ctrl_Time(CTRL_KEYS::d_RIGHT)) * 0.1f, 2);
			else if (lThumb.mag != 0.0f && lThumb.dir.x > 0.0f && abs(lThumb.dir.x) > abs(lThumb.dir.y))
				dir += lThumb.mag * 0.01f;
			//Left
			if (this->m_pInput->Key_Held(VK_LEFT) && this->m_pInput->Key_Time(VK_LEFT) > 0.1f)
				dir -= pow(float(this->m_pInput->Key_Time(VK_LEFT)) * 0.1f, 2);
			else if (this->m_pInput->Ctrl_Held(CTRL_KEYS::d_LEFT) && this->m_pInput->Ctrl_Time(CTRL_KEYS::d_LEFT) > 0.1f)
				dir -= pow(float(this->m_pInput->Ctrl_Time(CTRL_KEYS::d_LEFT)) * 0.1f, 2);
			else if (lThumb.mag != 0.0f && lThumb.dir.x < 0.0f && abs(lThumb.dir.x) > abs(lThumb.dir.y))
				dir -= lThumb.mag * 0.01f;
		}

		//Set the percent
		float percent = clamp<float>(this->m_pAudio->GetSfxVol() + dir, 0.0f, 1.0f);
		this->m_vAudioButtons[SFX].SetPercent(percent);

		if ( dir == 0.0f)
			this->m_dPercentTimer = -1.0f;
	}
	else
	{
		//For Sfx
		if (!this->m_pInput->Key_Held(VK_LBUTTON))
			this->m_dPercentTimer = -1.0f;
	}

	if (this->m_dPercentTimer < 0.0f && slong(this->m_pAudio->GetSfxVol() * 100) != slong(this->m_vAudioButtons[SFX].GetPercent() * 100))
	{
		this->m_dPercentTimer = kdPercentRate;
		this->m_pAudio->Play(this->m_sfxTick);
	}
	this->m_pAudio->SetSfxVol(this->m_vAudioButtons[SFX].GetPercent());
}
void COptionAVState::InputVideo(void)
{
	if (this->m_cFullscreenButton.GetClick().Pull())
	{
		if (this->m_pSprite->GetFullScreen())
		{
			this->m_cFullscreenButton.SetText("");
			this->m_pSprite->SetFullScreen(false);
		}
		else
		{
			this->m_cFullscreenButton.SetText("X");
			this->m_pSprite->SetFullScreen(true);
		}
	}

	this->m_pSprite->SetGamma(this->m_cGammaButton.GetPercent());
}
//Render
void COptionAVState::RenderButtons(void)
{
	for (ulong i = 0; i < NUM_BUTTON_GROUPS; i++)
		this->m_vButtonGroup[i].Render(this->m_pSprite, this->m_pText);
}
void COptionAVState::RenderText(void)
{
	_RECT<slong> rect;
	Type2<ulong> size;
	//AUDIO
	rect = this->m_vAudioButtons[SFX].GetRect();
	rect.y += 180;
	size = this->m_pText->GetSize("AUDIO", 1.0f);
	this->m_pText->Write("AUDIO", rect, CLR::LAVENDER, 1.0f);
	this->m_pSprite->DrawLine(rect.x, rect.y, rect.x + size.w, rect.y, CLR::WHITE, 2);
	//SFX
	rect = this->m_vAudioButtons[SFX].GetRect();
	rect.x -= 256;
	this->m_pText->Write("SFX", rect, CLR::GOLDENROD, 1.0f);
	rect.x += rect.w + 256;
	rect.w = 256;
	this->m_pText->Write(std::to_string(slong(this->m_vAudioButtons[SFX].GetPercent() * 100)).c_str(), rect, CLR::GOLDENROD, 1.0f, true, Text::EFontAlignment::F_RIGHT);
	//MUS
	rect = this->m_vAudioButtons[MUS].GetRect();
	rect.x -= 256;
	this->m_pText->Write("MUS", rect, CLR::GOLDENROD, 1.0f);
	rect.x += rect.w + 256;
	rect.w = 256;
	this->m_pText->Write(std::to_string(slong(this->m_vAudioButtons[MUS].GetPercent() * 100)).c_str(), rect, CLR::GOLDENROD, 1.0f, true, Text::EFontAlignment::F_RIGHT);
	//VIDEO
	rect = this->m_cGammaButton.GetRect();
	size = this->m_pText->GetSize("VIDEO", 1.0f);
	rect.y += 180;
	this->m_pText->Write("VIDEO", rect, CLR::LAVENDER, 1.0f);
	this->m_pSprite->DrawLine(rect.x, rect.y, rect.x + size.w, rect.y, CLR::WHITE, 2);
	//GAMMA
	rect = this->m_cGammaButton.GetRect();
	size = this->m_pText->GetSize("GAMMA", 1.0f);
	rect.x -= size.w + 32;
	this->m_pText->Write("GAMMA", rect, CLR::GOLDENROD, 1.0f);
	rect = this->m_cGammaButton.GetRect();
	rect.x += rect.w;
	rect.w = 256;
	this->m_pText->Write(std::to_string(slong(this->m_cGammaButton.GetPercent() * 100)).c_str(), rect, CLR::GOLDENROD, 1.0f, true, Text::EFontAlignment::F_RIGHT);
	//FullScreen
	rect = this->m_cFullscreenButton.GetRect();
	rect.x = this->m_cGammaButton.GetRect().x;
	rect.w = this->m_cGammaButton.GetRect().w;
	this->m_pText->Write("FULL SCREEN", rect, CLR::GOLDENROD, 1.0f);
}
//Saving & Loading
void COptionAVState::SaveAudio(void)
{
	std::ofstream fout;
	fout.open("AudioSettings.bin", std::ios_base::out | std::ios_base::binary);
	if (fout.is_open())
	{
		float sfx = this->m_pAudio->GetSfxVol();
		fout.write((const char*)(&sfx), sizeof(float));
		fout.close();
	}
}
void COptionAVState::LoadAudio(void)
{
	std::ifstream fin;
	fin.open("AudioSettings.bin", std::ios_base::in | std::ios_base::binary);
	if (fin.is_open())
	{
		float sfx = this->m_pAudio->GetSfxVol();
		fin.read((char*)(&sfx), sizeof(float));
		this->m_pAudio->SetSfxVol(sfx);
		this->m_vAudioButtons[SFX].SetPercent(sfx);
		fin.close();
	}
}

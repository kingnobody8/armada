/*
Author:			Daniel Habig
Date:			11-15-13
File:			IntroState.cpp
Purpose:		gamestate that allows selection of sub-states
*/
#include "OptionState.h"
#include "../SDL/Sprite.h"
#include "../Text/Text.h"
#include "../Input/InputCMD.h"
#include "../Audio/Audio.h"
using namespace Input;

COptionState::COptionState(void)
{
	this->m_eType = EGameStateType::gs_OPTIONS;
}
COptionState::~COptionState(void)
{
}

void COptionState::Enter(Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText, Audio::CAudio* pAudio)
{
	IGameState::Enter(pSprite, pInput, pText, pAudio);
	
	//Sfx
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
		m_vButtons[i].SetSfx(sfxHover, GUI::CButton::EButtonSfx::SFX_HOVER);
		m_vButtons[i].SetSfx(sfxPush, GUI::CButton::EButtonSfx::SFX_PUSH);
		m_vButtons[i].SetSfx(sfxPull, GUI::CButton::EButtonSfx::SFX_PULL);
		vButs.push_back(&m_vButtons[i]);
	}

	//AV Buton
	Type2<ulong> tSize;
	tSize = this->m_pText->GetSize("A/V", 1.0f);
	m_vButtons[AV].SetRect(_RECT<slong>(Sprite::SCREEN_WIDTH - tSize.x - 32, 768 - AV * 192, tSize.x, tSize.y));
	m_vButtons[AV].SetText("A/V");
	//Input Button
	tSize = this->m_pText->GetSize("Input", 1.0f);
	m_vButtons[INPUT].SetRect(_RECT<slong>(Sprite::SCREEN_WIDTH - tSize.x - 32, 768 - INPUT * 192, tSize.x, tSize.y));
	m_vButtons[INPUT].SetText("Input");
	//Rules Button
	tSize = this->m_pText->GetSize("Rules", 1.0f);
	m_vButtons[RULES].SetRect(_RECT<slong>(Sprite::SCREEN_WIDTH - tSize.x - 32, 768 - RULES * 192, tSize.x, tSize.y));
	m_vButtons[RULES].SetText("Rules");
	//Credits Button
	tSize = this->m_pText->GetSize("Credits", 1.0f);
	m_vButtons[CREDITS].SetRect(_RECT<slong>(Sprite::SCREEN_WIDTH - tSize.x - 32, 768 - CREDITS * 192, tSize.x, tSize.y));
	m_vButtons[CREDITS].SetText("Credits");
	//Back Button
	tSize = this->m_pText->GetSize("Back", 1.0f);
	m_vButtons[BACK].SetRect(_RECT<slong>(Sprite::SCREEN_WIDTH - tSize.x - 32, 768 - BACK * 192, tSize.x, tSize.y));
	m_vButtons[BACK].SetText("Back");
	m_vButtons[BACK].SetSfx(sfxBack, GUI::CButton::EButtonSfx::SFX_PULL);

	//Group
	this->m_vButtonGroup[PRIMARY].SetButtonList(vButs);
	this->m_vButtonGroup[PRIMARY].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[PRIMARY].SetWrap(Type2<bool>(false, true));
	this->m_vButtonGroup[PRIMARY].SetInputCMD(this->m_pInput);
	this->m_vButtonGroup[PRIMARY].SetFindClosestButtonGroup(true);
	this->m_vButtonGroup[PRIMARY].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
	this->m_vButtonGroup[PRIMARY].SetSelectedIndex(BACK);
	vButs.clear();
#pragma endregion

	//Build the Button  manager
	std::vector<GUI::CButtonGroup> vGroups;
	for (ulong i = 0; i < NUM_BUTTON_GROUPS; i++)
		vGroups.push_back(this->m_vButtonGroup[i]);
	this->m_cButMan.SetGroups(vGroups);
}
void COptionState::Exit(void)
{
	IGameState::Exit();
}

void COptionState::Input(void)
{
	Input::eInputType eType = this->m_pInput->GetFinalInputType();
	this->m_pInput->Cursor_Lock(eType != MOUSE);

	//Selection
	this->m_cButMan.Selection(this->m_pInput);

	//Confirmation
	EOptionButtons eBut = NUM_BUTTONS;
	bool confirm = false;
	if (eType == Input::eInputType::MOUSE)
		confirm = this->m_pInput->Key_Held(VK_LBUTTON);
	else
		confirm = this->m_pInput->Key_Held(VK_RETURN) || this->m_pInput->Key_Held(VK_SPACE) ||
		this->m_pInput->Ctrl_Held(CTRL_KEYS::XB_A) || this->m_pInput->Ctrl_Held(CTRL_KEYS::START);
	for (ulong i = 0; i < NUM_BUTTONS; i++)
	{
		if (this->m_vButtons[i].Confirmation(confirm).Pull())
			eBut = EOptionButtons(i);
	}

	//Results
	if (this->m_vButtons[AV].GetClick().Pull())
	{
		this->m_tResult.Push(&this->m_cOptionAVState);
		this->m_cOptionAVState.SetStarField(this->m_pStarField);
	}
	if (this->m_vButtons[INPUT].GetClick().Pull())
	{

	}
	if (this->m_vButtons[RULES].GetClick().Pull())
	{

	}
	if (this->m_vButtons[CREDITS].GetClick().Pull())
	{

	}
	if (this->m_vButtons[BACK].GetClick().Pull())
	{
		if (this->m_vButtons[BACK].GetClick().Pull())
			this->m_tResult.Prev(IGameState::EGameStateType::gs_MAIN_MENU);
	}

}
void COptionState::Update(Time delta, Time gameTime)
{
	this->m_pStarField->Update(delta);
	this->m_cButMan.Update(delta, this->m_pAudio);
}
void COptionState::Render(void)
{
	this->m_pStarField->Render();

	for (ulong i = 0; i < NUM_BUTTONS; i++)
		this->m_vButtons[i].Render(m_pSprite, m_pText);
}
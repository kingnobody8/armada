/*
Author:			Daniel Habig
Date:			11-15-13
File:			IntroState.cpp
Purpose:		gamestate that allows selection of sub-states
*/
#include "MenuState.h"
#include "../SDL/Sprite.h"
#include "../Text/Text.h"
#include "../Input/InputCMD.h"
#include "../Audio/Audio.h"
using namespace Input;


CMenuState::CMenuState(void)
{
	this->m_eType = EGameStateType::gs_MAIN_MENU;
	this->m_dCtrlStickTimer = -1.0f;
}
CMenuState::~CMenuState(void)
{
}

void CMenuState::Enter(Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText, Audio::CAudio* pAudio)
{
	IGameState::Enter(pSprite, pInput, pText, pAudio);

	slong push, pull, hover;
	push = pAudio->LoadAudio("Assets/Sfx/push.wav", Audio::EAudioType::SFX_2D, false);
	pull = pAudio->LoadAudio("Assets/Sfx/pull.wav", Audio::EAudioType::SFX_2D, false);
	hover = pAudio->LoadAudio("Assets/Sfx/hover.wav", Audio::EAudioType::SFX_2D, false);

	//List of buttons for each group
	std::vector<GUI::CButton*> vButs;
	//List of groups for the button manager
	std::vector<GUI::CButtonGroup> vGroups;

	//Setup button Colors
	for (ulong i = 0; i < NUM_BUTTONS; i++)
	{
		m_vButtons[i].DefaultColor(CLR::CLEAR);

		m_vButtons[i].SetTextColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
		m_vButtons[i].SetTextColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
		m_vButtons[i].SetTextColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);

		m_vButtons[i].SetSfx(push, GUI::CButton::EButtonSfx::SFX_PUSH);
		m_vButtons[i].SetSfx(pull, GUI::CButton::EButtonSfx::SFX_PULL);
		m_vButtons[i].SetSfx(hover, GUI::CButton::EButtonSfx::SFX_HOVER);


		vButs.push_back(&m_vButtons[i]);
	}

	//Setup button Text and Rects
	Type2<ulong> tSize;
	//Play
	tSize = this->m_pText->GetSize("Play", 1.0f);
	m_vButtons[PLAY].SetText("Play");
	m_vButtons[PLAY].SetRect(_RECT<slong>(Sprite::SCREEN_WIDTH - tSize.x - 32, 576 - PLAY * 192, tSize.x, tSize.y));
	m_vButtons[PLAY].SetPrevState(GUI::CButton::EButtonState::HOVER);
	//Forge
	tSize = this->m_pText->GetSize("Forge", 1.0f);
	m_vButtons[FORGE].SetText("Forge");
	m_vButtons[FORGE].SetRect(_RECT<slong>(Sprite::SCREEN_WIDTH - tSize.x - 32, 576 - FORGE * 192, tSize.x, tSize.y));
	//Options
	tSize = this->m_pText->GetSize("Options", 1.0f);
	m_vButtons[OPTIONS].SetText("Options");
	m_vButtons[OPTIONS].SetRect(_RECT<slong>(Sprite::SCREEN_WIDTH - tSize.x - 32, 576 - OPTIONS * 192, tSize.x, tSize.y));
	//Exit
	tSize = this->m_pText->GetSize("Exit", 1.0f);
	m_vButtons[EXIT].SetText("Exit");
	m_vButtons[EXIT].SetRect(_RECT<slong>(Sprite::SCREEN_WIDTH - tSize.x - 32, 576 - EXIT * 192, tSize.x, tSize.y));


	//Put Buttons into Group
	GUI::CButtonGroup cGroup;
	cGroup.SetInputCMD(this->m_pInput);
	cGroup.SetButtonList(vButs);
	cGroup.SetMove(Type2<bool>(false, true));
	cGroup.SetWrap(Type2<bool>(false, true));
	vGroups.push_back(cGroup);

	//Put Groups into Button Manager
	this->m_cButMan.SetGroups(vGroups);
}
void CMenuState::Exit(void)
{
	IGameState::Exit();
}

void CMenuState::Input(void)
{
	Input::eInputType eType = this->m_pInput->GetFinalInputType();
	this->m_pInput->Cursor_Lock(eType != MOUSE);

	//Selection
	this->m_cButMan.Selection(this->m_pInput);

	//Confirmation
	EMenuButtons eBut = NUM_BUTTONS;
	bool confirm = false;
	if (eType == Input::eInputType::MOUSE)
		confirm = this->m_pInput->Key_Held(VK_LBUTTON);
	else
		confirm = this->m_pInput->Key_Held(VK_RETURN) || this->m_pInput->Key_Held(VK_SPACE) ||
		this->m_pInput->Ctrl_Held(CTRL_KEYS::XB_A) || this->m_pInput->Ctrl_Held(CTRL_KEYS::START);
	for (ulong i = 0; i < NUM_BUTTONS; i++)
	{
		if (this->m_vButtons[i].Confirmation(confirm).Pull())
			eBut = EMenuButtons(i);
	}

	//GOTO next state
	switch (eBut)
	{
	case PLAY:
		this->m_tResult.Push(&this->m_cPreLobbyState);
		this->m_cPreLobbyState.SetStarField(this->m_pStarField);
		break;
	case FORGE:
		this->m_tResult.Push(&this->m_cForgeState);
		this->m_cForgeState.SetStarField(this->m_pStarField);
		break;
	case OPTIONS:
		this->m_tResult.Push(&this->m_cOptionState);
		this->m_cOptionState.SetStarField(this->m_pStarField);
		break;
	case EXIT:
		this->m_tResult.Kill();
		break;
	default:
		break;
	}
}
void CMenuState::Update(Time delta, Time gameTime)
{
	this->m_pStarField->Update(delta);
	this->m_cButMan.Update(delta, this->m_pAudio);
}
void CMenuState::Render(void)
{
	this->m_pStarField->Render();

	Type2<ulong> size = this->m_pText->GetSize("QUANTUM", 3.0f);
	this->m_pText->Write("QUANTUM", _RECT<slong>(0, Sprite::SCREEN_HEIGHT - size.y, Sprite::SCREEN_WIDTH, size.y), CLR::LAVENDER, 3.0f, true, Text::EFontAlignment::F_CENTER);

	for (ulong i = 0; i < NUM_BUTTONS; i++)
		this->m_vButtons[i].Render(m_pSprite, m_pText);
}
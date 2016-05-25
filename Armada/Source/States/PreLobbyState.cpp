/*
Author:			Daniel Habig
Date:			11-15-13
File:			IntroState.cpp
Purpose:		gamestate that allows selection of sub-states
*/
#include "PreLobbyState.h"
#include "../SDL/Sprite.h"
#include "../Text/Text.h"
#include "../Input/InputCMD.h"
#include "LobbyStateServer.h"
using namespace Input;
using namespace Network;

CPreLobbyState::CPreLobbyState(void)
{
	this->m_eType = EGameStateType::gs_PRELOBBY;
}
CPreLobbyState::~CPreLobbyState(void)
{
}

void CPreLobbyState::Enter(Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText, Audio::CAudio* pAudio)
{
	IGameState::Enter(pSprite, pInput, pText, pAudio);

	//List of buttons for each group
	std::vector<GUI::CButton*> vButs;
	//List of groups for the button manager
	std::vector<GUI::CButtonGroup> vGroups;

	for (ulong i = 0; i < NUM_BUTTONS; i++)
	{
		m_vButtons[i].DefaultColor(CLR::CLEAR);
		m_vButtons[i].SetTextColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
		m_vButtons[i].SetTextColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
		m_vButtons[i].SetTextColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
		m_vButtons[i].SetTextAlignment(Text::EFontAlignment::F_LEFT);
	}
	Type2<ulong> tSize;
	//Back
	tSize = this->m_pText->GetSize("Back", 1.0f);
	m_vButtons[BACK].SetText("Back");
	m_vButtons[BACK].SetRect(_RECT<slong>(Sprite::SCREEN_WIDTH - tSize.x - 32, 0, tSize.x, tSize.y));

	//HOST
	tSize = this->m_pText->GetSize("Host Lobby", 1.0f);
	m_vButtons[HOST].SetText("Host Lobby");
	m_vButtons[HOST].SetRect(_RECT<slong>(128, Sprite::SCREEN_HEIGHT - 192 * 2, tSize.x, tSize.y));

	//JOIN LAN
	tSize = this->m_pText->GetSize("Join LAN", 1.0f);
	m_vButtons[JOIN_LAN].SetText("Join LAN");
	m_vButtons[JOIN_LAN].SetRect(_RECT<slong>(128, Sprite::SCREEN_HEIGHT - tSize.y - 192 * 3, tSize.x, tSize.y));

	//JOIN WAN
	tSize = this->m_pText->GetSize("Join WAN", 1.0f);
	m_vButtons[JOIN_WAN].SetText("Join WAN");
	m_vButtons[JOIN_WAN].SetRect(_RECT<slong>(128, Sprite::SCREEN_HEIGHT - tSize.y - 192 * 4, tSize.x, tSize.y));


	//Put Buttons into Group
	GUI::CButtonGroup cGroup;
	cGroup.SetInputCMD(this->m_pInput);
	vButs.push_back(&this->m_vButtons[BACK]);
	cGroup.SetButtonList(vButs);
	cGroup.SetButtonGroupDir(LEFT, 1);
	cGroup.SetMove(Type2<bool>(false, true));
	cGroup.SetWrap(Type2<bool>(false, false));
	vGroups.push_back(cGroup);
	vButs.clear();

	cGroup = GUI::CButtonGroup();
	cGroup.SetInputCMD(this->m_pInput);
	vButs.push_back(&this->m_vButtons[HOST]);
	vButs.push_back(&this->m_vButtons[JOIN_LAN]);
	vButs.push_back(&this->m_vButtons[JOIN_WAN]);
	cGroup.SetButtonList(vButs);
	cGroup.SetButtonGroupDir(RIGHT, 0);
	cGroup.SetMove(Type2<bool>(false, true));
	cGroup.SetWrap(Type2<bool>(false, true));
	vGroups.push_back(cGroup);
	vButs.clear();

	//Put Groups into Button Manager
	this->m_cButMan.SetGroups(vGroups);
}
void CPreLobbyState::Exit(void)
{
	IGameState::Exit();
	this->m_cSMach.End();
	this->m_cClient.stop();
}

void CPreLobbyState::Input(void)
{
	Input::eInputType eType = this->m_pInput->GetFinalInputType();
	this->m_pInput->Cursor_Lock(eType != MOUSE);

	//Selection
	this->m_cButMan.Selection(this->m_pInput);

	//Confirmation
	EPreLobbyButtons eBut = NUM_BUTTONS;
	bool confirm = false;
	if (eType == Input::eInputType::MOUSE)
		confirm = this->m_pInput->Key_Held(VK_LBUTTON);
	else
		confirm = this->m_pInput->Key_Held(VK_RETURN) || this->m_pInput->Key_Held(VK_SPACE) ||
		this->m_pInput->Ctrl_Held(CTRL_KEYS::XB_A) || this->m_pInput->Ctrl_Held(CTRL_KEYS::START);
	for (slong i = 0; i < NUM_BUTTONS; i++)
	{
		if (this->m_vButtons[i].Confirmation(confirm).Pull())
			eBut = EPreLobbyButtons(i);
	}

	//GOTO next state
	switch (eBut)
	{
	case BACK:
		this->m_tResult.Prev(EGameStateType::gs_MAIN_MENU);
		break;
	default:
		break;
	}

	if (m_vButtons[HOST].GetClick().Pull())
	{
		//Start Server Thread & Client
		//We have to unlock the mouse in case they need to click on the "Allow Network" pop-up
		m_cSMach.Start(new CLobbyStateServer());
		this->m_cClient.init();
		this->m_cClient.connect_lan("127.0.0.1", SERVER_PORT);
	}
	if (m_vButtons[JOIN_LAN].GetClick().Pull())
	{
		this->m_cLobbyState.SetStarField(this->m_pStarField);
		this->m_cLobbyState.SetClient(&this->m_cClient);
		this->m_tResult.Push(&this->m_cLobbyState);
	}
}
void CPreLobbyState::Update(Time delta, Time gameTime)
{
	this->m_pStarField->Update(delta);
	this->UpdateClient(delta);
}
void CPreLobbyState::Render(void)
{
	this->m_pStarField->Render();

	for (ulong i = 0; i < NUM_BUTTONS; i++)
	{
		this->m_vButtons[i].Render(m_pSprite, m_pText);
		if (i == BACK)
			continue;
		_RECT<slong> r = this->m_vButtons[i].GetRect();
		_COLOR<uchar> clr = this->m_vButtons[i].GetTextColor(this->m_vButtons[i].GetState());
		this->m_pSprite->DrawRect(_RECT<slong>(r.x - 64, (slong)(r.y + r.h * 0.5f), 24, 24), clr);
	}

	Type2<ulong> size = this->m_pText->GetSize("Host", 0.5f);
	ulong h = Sprite::SCREEN_HEIGHT - size.y - 128;
	this->m_pText->Write("Host", _RECT<slong>(64, h, size.w, size.y), CLR::LAVENDER, 0.5f);
	this->m_pSprite->DrawLine(32, h, 768, h);

	size = this->m_pText->GetSize("Join", 0.5f);
	h = h - size.y - 256;
	this->m_pText->Write("Join", _RECT<slong>(64, h, size.w, size.y), CLR::LAVENDER, 0.5f);
	this->m_pSprite->DrawLine(32, h - 1, 768, h - 1);

}

//Update
void CPreLobbyState::UpdateClient(Time dDelta)
{
	if (!this->m_cClient.read_msg())
		return;

	std::vector<CNetMsg> msgs = this->m_cClient.GetMsgs();
	this->m_cClient.SetMsgsClear();

	ulong size = msgs.size();
	for (ulong i = 0; i < size; ++i)
	{
		if (msgs[i].GetType() == NM_CLIENT_ID)
		{
			this->m_cSetupState.SetStarField(this->m_pStarField);
			this->m_cSetupState.SetClient(&this->m_cClient);
			this->m_cSetupState.SetServerMachine(&this->m_cSMach);
			this->m_tResult.Push(&this->m_cSetupState);
		}
		else if (msgs[i].GetType() == ID_NO_FREE_INCOMING_CONNECTIONS)
		{

		}
	}
}
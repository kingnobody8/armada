/*
Author:			Daniel Habig
Date:			11-15-13
File:			IntroState.cpp
Purpose:		gamestate that begins the game, shows logos, transitions to StartState
*/
#include "SetupState.h"
#include "LobbyStateServer.h"
#include "../SDL/Sprite.h"
#include "../Paker/Paker.h"
#include "../Input/InputCMD.h"
#include "../Text/Text.h"
#include "../Audio/Audio.h"
#include "../Network/Client.h"
#include "../State_Machine/Server_Machine.h"
#include <algorithm>
#include <fstream>
using namespace Input;
using namespace Board;
using namespace Network;

const Time kdMarkerRate = 0.5l;

CSetupState::CSetupState(void)
{
	this->m_eType = EGameStateType::gs_FORGE;
	this->m_pStarField = NULL;
	this->m_pClient = NULL;
	this->m_pSMach = NULL;
	this->m_bMarkerVisible = false;
	this->m_dMarkerTimer = kdMarkerRate;
	this->m_slDefaultOffset = 0;
	this->m_slDefaultIndex = -1;
	this->m_slCustomOffset = 0;
	this->m_slCustomIndex = -1;
	this->m_slMsgOffset = 0;
	this->m_slSfxMsg = -1;
}
CSetupState::~CSetupState(void)
{
}

void CSetupState::Enter(Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText, Audio::CAudio* pAudio)
{
	IGameState::Enter(pSprite, pInput, pText, pAudio);

	//Build Buttons
	this->EnterPrimary();
	this->EnterGrid();
	this->EnterNames();
	this->EnterClientType();
	this->EnterTextMsg();
	this->EnterTextSlots();
	this->EnterTextButs();
	this->EnterDefaultSlots();
	this->EnterDefaultButs();
	this->EnterCustomSlots();
	this->EnterCustomButs();

	//Build the Button  manager
	std::vector<GUI::CButtonGroup> vGroups;
	for (ulong i = 0; i < NUM_BUTTON_GROUPS; i++)
		vGroups.push_back(this->m_vButtonGroup[i]);
	this->m_cButMan.SetGroups(vGroups);

	//Clear the txt msg list
	this->m_vTxtMsgs.clear();

	//Sfx
	this->m_slSfxMsg = this->m_pAudio->LoadAudio("Assets/SFX/msg.wav", Audio::SFX_2D);

	//Host Only
	if (this->Host())
	{
		//Initialize Buttons
		this->m_slDefaultIndex = 0;
		this->m_vDefaultSlotButtons[0].SetToggle(true);
		//Load Boards
		this->LoadDefault();
		this->LoadCustom();
		this->SortBoards();
		//Send board
		this->SendBoard();
	}

	//Inform the server that we are ready
	this->m_pClient->send_msg(CNetMsg(ENetMsg::NM_LOBBY_ENTER));
	//Determine Active Buttons
	this->DetermineActiveButtons();
}
void CSetupState::Exit(void)
{
	IGameState::Exit();
	if (this->m_pStarField)
		this->m_pStarField = NULL;
	if (this->m_pClient)
	{
		this->m_pClient->stop();
		this->m_pClient = NULL;
	}
	if (this->m_pSMach)
	{
		this->m_pSMach->End();
		this->m_pSMach = NULL;
	}

	//In case we were typing
	this->m_pInput->Text_Set_Mode(false);
}

void CSetupState::Input(void)
{
	Input::eInputType eType = this->m_pInput->GetFinalInputType();
	this->m_pInput->Cursor_Lock(eType != MOUSE);

	//Selection
	this->m_cButMan.Selection(this->m_pInput);

	//Special Case Name Buttons
	bool bNameToggle = false;
	for (ulong i = 0; i < Game::kulMaxPlayers; ++i)
	{
		if (this->m_vNameButtons[i].GetToggle())
		{
			bNameToggle = true;
			break;
		}
	}

	//Confirmation
	bool confirm = false;
	if (eType == Input::eInputType::MOUSE)
		confirm = this->m_pInput->Key_Held(VK_LBUTTON);
	else
		confirm = this->m_pInput->Key_Held(VK_RETURN) || (this->m_pInput->Key_Held(VK_SPACE) && !bNameToggle) ||
		this->m_pInput->Ctrl_Held(CTRL_KEYS::XB_A) || this->m_pInput->Ctrl_Held(CTRL_KEYS::START);
	for (ulong i = 0; i < NUM_BUTTON_GROUPS; i++)
		this->m_vButtonGroup[i].Confirmation(confirm);

	//Special Case Name Buttons
	for (ulong i = 0; i < Game::kulMaxPlayers; ++i)
	{
		if (this->m_vNameButtons[i].GetToggle() && eType == Input::eInputType::MOUSE && this->m_pInput->Key_Push(VK_RETURN))
		{
			this->m_vNameButtons[i].Selection(true);
			this->m_vNameButtons[i].Confirmation(true);
			this->m_vNameButtons[i].Selection(true);
			this->m_vNameButtons[i].Confirmation(false);
		}
	}
	//Special Case TxtMsg Button
	if (this->m_cTextMsgButton.GetToggle() && eType == Input::eInputType::MOUSE && this->m_pInput->Key_Push(VK_RETURN))
	{
		this->m_cTextMsgButton.Selection(true);
		this->m_cTextMsgButton.Confirmation(true);
		this->m_cTextMsgButton.Selection(true);
		this->m_cTextMsgButton.Confirmation(false);
	}

	//Results
	{
		this->InputPrimary();
		this->InputNames();
		this->InputClientTypes();
		this->InputTxtMsg();
		this->InputTxtButs();
		this->InputGrid();
		this->InputDefaultSlots();
		this->InputDefaultButs();
		this->InputCustomSlots();
		this->InputCustomButs();
	}
}
void CSetupState::Update(Time delta, Time gameTime)
{
	this->m_pStarField->Update(delta);
	this->m_cButMan.Update(delta, this->m_pAudio);
	this->UpdateNames(delta);
	this->UpdateClient(delta);
	this->UpdateTxtMsg(delta);
}
void CSetupState::Render(void)
{
	this->m_pStarField->Render();
	this->RenderButtons();
	this->RenderText();
	this->RenderClientType();

	//this->m_pSprite->DrawRect(this->m_tMsgRect, CLR::CHARCOAL);
	//for (slong x = 0; x < GRID::kGridSize.x; ++x)
	//{
	//	for (slong y = 0; y < GRID::kGridSize.y; ++y)
	//	{
	//		this->m_pSprite->DrawRect(GRID::kGrid[x][y], CLR::CHARTREUSE, false, 4);
	//	}
	//}
}

//Enter
void CSetupState::EnterPrimary(void)
{
	//Sfx
	slong sfxBack = this->m_pAudio->LoadAudio("Assets/Sfx/back.wav", Audio::EAudioType::SFX_2D);
	slong sfxPush = this->m_pAudio->LoadAudio("Assets/Sfx/push.wav", Audio::EAudioType::SFX_2D);
	slong sfxHover = this->m_pAudio->LoadAudio("Assets/Sfx/hover.wav", Audio::EAudioType::SFX_2D);
	//Button vec
	std::vector<GUI::CButton*> vButs;

	//Shared Settings
	for (ulong i = 0; i < NUM_BUTTONS; i++)
	{
		//Clear all colors
		m_vButtons[i].DefaultColor(CLR::CLEAR);
		//Text Color
		m_vButtons[i].SetTextColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
		m_vButtons[i].SetTextColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
		m_vButtons[i].SetTextColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
		//Sfx
		m_vButtons[i].SetSfx(sfxHover, GUI::CButton::EButtonSfx::SFX_HOVER);
		m_vButtons[i].SetSfx(sfxPush, GUI::CButton::EButtonSfx::SFX_PUSH);
		m_vButtons[i].SetSfx(sfxPush, GUI::CButton::EButtonSfx::SFX_PULL);
		//Push Back
		vButs.push_back(&m_vButtons[i]);
	}

	/* Individual Settings */
	//Back Button
	Type2<ulong> tSize;
	tSize = this->m_pText->GetSize("Back", 1.0f);
	m_vButtons[BACK].SetText("Back");
	m_vButtons[BACK].SetRect(_RECT<slong>(Sprite::SCREEN_WIDTH - tSize.x - 32, 0, tSize.x, tSize.y));
	m_vButtons[BACK].SetSfx(sfxBack, GUI::CButton::EButtonSfx::SFX_PULL);

	//Group Settings
	this->m_vButtonGroup[PRIMARY].SetButtonList(vButs);
	this->m_vButtonGroup[PRIMARY].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[PRIMARY].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[PRIMARY].SetInputCMD(this->m_pInput);
	//this->m_vButtonGroup[PRIMARY].SetFindClosestButtonGroup(true);
	this->m_vButtonGroup[PRIMARY].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
}
void CSetupState::EnterGrid(void)
{
	//Button vec
	std::vector<GUI::CButton*> vButs;
	//Grab Sfx
	slong sfxGood = this->m_pAudio->LoadAudio("Assets/Sfx/beep_good.wav");
	slong sfxHover = this->m_pAudio->LoadAudio("Assets/Sfx/hover.wav");

	/*Shared Settings*/
	for (ulong x = 0; x < Board::kslBoardSize; x++)
	{
		for (ulong y = 0; y < Board::kslBoardSize; y++)
		{
			//Clear all colors
			this->m_vGrid[x][y].DefaultColor(CLR::CLEAR);
			//Outline color
			this->m_vGrid[x][y].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
			this->m_vGrid[x][y].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::HOVER);
			this->m_vGrid[x][y].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::MDOWN);
			this->m_vGrid[x][y].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::INACTIVE);
			//Outline Size
			this->m_vGrid[x][y].SetOutlineSize(2, GUI::CButton::EButtonState::DEFAULT);
			this->m_vGrid[x][y].SetOutlineSize(2, GUI::CButton::EButtonState::HOVER);
			this->m_vGrid[x][y].SetOutlineSize(2, GUI::CButton::EButtonState::MDOWN);
			this->m_vGrid[x][y].SetOutlineSize(2, GUI::CButton::EButtonState::INACTIVE);
			//Text
			this->m_vGrid[x][y].SetTextAlignment(Text::EFontAlignment::F_CENTER);
			this->m_vGrid[x][y].SetTextScale(0.5f);
			this->m_vGrid[x][y].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::DEFAULT);
			this->m_vGrid[x][y].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::HOVER);
			this->m_vGrid[x][y].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::MDOWN);
			this->m_vGrid[x][y].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::INACTIVE);
			//Sfx
			this->m_vGrid[x][y].SetSfx(sfxGood, GUI::CButton::EButtonSfx::SFX_PULL);
			this->m_vGrid[x][y].SetSfx(sfxHover, GUI::CButton::EButtonSfx::SFX_HOVER);
			//Rect
			_RECT<slong> rect;
			rect.w = rect.h = 96;
			rect.x = GRID::kGrid[6][2].x + x * rect.w;
			rect.y = GRID::kGrid[6][2].y + y * rect.h;
			this->m_vGrid[x][y].SetRect(rect);
			//Push Back
			vButs.push_back(&m_vGrid[x][y]);
		}
	}

	/*Group Settings*/
	this->m_vButtonGroup[GRID].SetButtonList(vButs);
	this->m_vButtonGroup[GRID].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[GRID].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[GRID].SetInputCMD(this->m_pInput);
	this->m_vButtonGroup[GRID].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
	this->m_vButtonGroup[GRID].SetFindClosestButtonGroup(true);
}
void CSetupState::EnterNames(void)
{
	//Sfx
	slong sfxBack = this->m_pAudio->LoadAudio("Assets/Sfx/back.wav", Audio::EAudioType::SFX_2D);
	slong sfxPush = this->m_pAudio->LoadAudio("Assets/Sfx/push.wav", Audio::EAudioType::SFX_2D);
	slong sfxHover = this->m_pAudio->LoadAudio("Assets/Sfx/hover.wav", Audio::EAudioType::SFX_2D);
	//Button vec
	std::vector<GUI::CButton*> vButs;

	//Shared Settings
	for (ulong i = 0; i < Game::kulMaxPlayers; i++)
	{
		//Clear all colors
		m_vNameButtons[i].DefaultColor(CLR::CLEAR);
		//Set Active and Toggle
		m_vNameButtons[i].SetActive(false);
		m_vNameButtons[i].SetToggle(false);
		//Background Color
		m_vNameButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
		m_vNameButtons[i].SetBGColor(CLR::GREY, GUI::CButton::EButtonState::ON);
		m_vNameButtons[i].SetBGColor(CLR::GREY, GUI::CButton::EButtonState::ON_HOVER);
		m_vNameButtons[i].SetBGColor(CLR::GREY, GUI::CButton::EButtonState::ON_MDOWN);
		m_vNameButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF);
		m_vNameButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_HOVER);
		m_vNameButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_MDOWN);
		//Outline Color
		//m_vNameButtons[i].SetOutlineColor(CLR::GOLDENROD * 0.5f, GUI::CButton::EButtonState::INACTIVE);
		m_vNameButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::ON);
		m_vNameButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::ON_HOVER);
		m_vNameButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::ON_MDOWN);
		m_vNameButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::OFF);
		m_vNameButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::OFF_HOVER);
		m_vNameButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::OFF_MDOWN);
		//Outline Size
		m_vNameButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::INACTIVE);
		m_vNameButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::ON);
		m_vNameButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::ON_HOVER);
		m_vNameButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::ON_MDOWN);
		m_vNameButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::OFF);
		m_vNameButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::OFF_HOVER);
		m_vNameButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::OFF_MDOWN);
		//Text Color
		m_vNameButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::INACTIVE);
		m_vNameButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON);
		m_vNameButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_HOVER);
		m_vNameButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_MDOWN);
		m_vNameButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF);
		m_vNameButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_HOVER);
		m_vNameButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_MDOWN);
		//Text Size
		m_vNameButtons[i].SetTextScale(0.56f);
		m_vNameButtons[i].SetTextAlignment(Text::EFontAlignment::F_LEFT);
		m_vNameButtons[i].SetTextOffset(Type2<slong>(12, 0));
		//Rect
		_RECT<slong> rect = GRID::kGrid[0][4];
		rect.y += (Game::kulMaxPlayers - i - 1) * (GRID::kSize.h + GRID::kSpace.h);
		rect.w = 4 * (GRID::kSize.w + GRID::kSpace.w) - GRID::kSpace.w;
		m_vNameButtons[i].SetRect(rect);
		//Push Back
		vButs.push_back(&m_vNameButtons[i]);
	}

	//Group Settings
	this->m_vButtonGroup[NAMES].SetButtonList(vButs);
	this->m_vButtonGroup[NAMES].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[NAMES].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[NAMES].SetInputCMD(this->m_pInput);
	//this->m_vButtonGroup[NAMES].SetFindClosestButtonGroup(true);
	this->m_vButtonGroup[NAMES].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
}
void CSetupState::EnterClientType(void)
{
	//Early Out
	if (!this->Host())
		return;

	//Button vec
	std::vector<GUI::CButton*> vButs;

	//Shared Settings
	for (ulong i = 0; i < Game::kulMaxPlayers; i++)
	{
		//Clear all colors
		this->m_vClientTypeButtons[i].DefaultColor(CLR::CLEAR);
		//Background Color
		this->m_vClientTypeButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::DEFAULT);
		this->m_vClientTypeButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::HOVER);
		this->m_vClientTypeButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::MDOWN);
		//Outline Color
		this->m_vClientTypeButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
		this->m_vClientTypeButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
		this->m_vClientTypeButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
		//Outline Size
		this->m_vClientTypeButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::DEFAULT);
		this->m_vClientTypeButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::HOVER);
		this->m_vClientTypeButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::MDOWN);
		//Rect
		_RECT<slong> rect = GRID::kGrid[4][4];
		rect.y += (Game::kulMaxPlayers - i - 1) * (GRID::kSize.h + GRID::kSpace.h);
		this->m_vClientTypeButtons[i].SetRect(rect);
		//Push Back
		vButs.push_back(&this->m_vClientTypeButtons[i]);
	}

	//Group Settings
	this->m_vButtonGroup[CLIENT_TYPES].SetButtonList(vButs);
	this->m_vButtonGroup[CLIENT_TYPES].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[CLIENT_TYPES].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[CLIENT_TYPES].SetInputCMD(this->m_pInput);
	//this->m_vButtonGroup[CLIENT_TYPES].SetFindClosestButtonGroup(true);
	this->m_vButtonGroup[CLIENT_TYPES].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
}
void CSetupState::EnterTextMsg(void)
{
	//Button vec
	std::vector<GUI::CButton*> vButs;

	this->m_cTextMsgButton.SetToggle(false);
	//Clear all colors
	this->m_cTextMsgButton.DefaultColor(CLR::CLEAR);
	//Outline Color
	this->m_cTextMsgButton.SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::ON);
	this->m_cTextMsgButton.SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::ON_HOVER);
	this->m_cTextMsgButton.SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::ON_MDOWN);
	this->m_cTextMsgButton.SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::OFF);
	this->m_cTextMsgButton.SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::OFF_HOVER);
	this->m_cTextMsgButton.SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::OFF_MDOWN);
	//Background Color
	this->m_cTextMsgButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
	this->m_cTextMsgButton.SetBGColor(CLR::GREY, GUI::CButton::EButtonState::ON);
	this->m_cTextMsgButton.SetBGColor(CLR::GREY, GUI::CButton::EButtonState::ON_HOVER);
	this->m_cTextMsgButton.SetBGColor(CLR::GREY, GUI::CButton::EButtonState::ON_MDOWN);
	this->m_cTextMsgButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF);
	this->m_cTextMsgButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_HOVER);
	this->m_cTextMsgButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_MDOWN);
	//Text Color
	this->m_cTextMsgButton.SetTextColor(CLR::GREY, GUI::CButton::EButtonState::INACTIVE);
	this->m_cTextMsgButton.SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON);
	this->m_cTextMsgButton.SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_HOVER);
	this->m_cTextMsgButton.SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_MDOWN);
	this->m_cTextMsgButton.SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF);
	this->m_cTextMsgButton.SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_HOVER);
	this->m_cTextMsgButton.SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_MDOWN);
	//Text Size
	this->m_cTextMsgButton.SetTextScale(0.56f * 0.588235f);
	this->m_cTextMsgButton.SetTextAlignment(Text::EFontAlignment::F_LEFT);
	this->m_cTextMsgButton.SetTextOffset(Type2<slong>(slong(12 * 0.588235f), 4));
	//Rect
	_RECT<slong> rect;
	rect.x = GRID::kGrid[2][0].x + GRID::kSize.w;
	rect.w = GRID::kGrid[10][0].x + GRID::kSize.w - rect.x;
	rect.h = 52;
	this->m_cTextMsgButton.SetRect(rect);
	//Push Back
	vButs.push_back(&this->m_cTextMsgButton);

	//Group Settings
	this->m_vButtonGroup[TXTMSG].SetButtonList(vButs);
	this->m_vButtonGroup[TXTMSG].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[TXTMSG].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[TXTMSG].SetInputCMD(this->m_pInput);
	//this->m_vButtonGroup[TXTMSG].SetFindClosestButtonGroup(true);
	this->m_vButtonGroup[TXTMSG].SetMoveType(GUI::CButton::EMoveType::CLOSEST);

	//Setup for TxtMsgGui
	//Clear all colors
	this->m_cTxtMsgGui.DefaultColor(CLR::CLEAR);
	//Outline Color
	this->m_cTxtMsgGui.SetOutlineColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
	this->m_cTxtMsgGui.SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
	this->m_cTxtMsgGui.SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::HOVER);
	this->m_cTxtMsgGui.SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::MDOWN);
	//Background Color
	this->m_cTxtMsgGui.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
	this->m_cTxtMsgGui.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::DEFAULT);
	this->m_cTxtMsgGui.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::HOVER);
	this->m_cTxtMsgGui.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::MDOWN);
	//Text Color
	this->m_cTxtMsgGui.SetTextColor(CLR::LAVENDER, GUI::CButton::EButtonState::INACTIVE);
	this->m_cTxtMsgGui.SetTextColor(CLR::LAVENDER, GUI::CButton::EButtonState::DEFAULT);
	this->m_cTxtMsgGui.SetTextColor(CLR::LAVENDER, GUI::CButton::EButtonState::HOVER);
	this->m_cTxtMsgGui.SetTextColor(CLR::LAVENDER, GUI::CButton::EButtonState::MDOWN);
	//Text Size
	this->m_cTxtMsgGui.SetTextScale(0.56f * 0.588235f);
	this->m_cTxtMsgGui.SetTextAlignment(Text::EFontAlignment::F_CENTER);
	this->m_cTxtMsgGui.SetTextOffset(Type2<slong>(slong(12 * 0.588235f), 0));
	this->m_cTxtMsgGui.SetText("TEXT CHAT");
	//Rect
	rect = _RECT<slong>();
	rect.w = GRID::kGrid[2][0].x + GRID::kSize.w;
	rect.h = 52;
	this->m_cTxtMsgGui.SetRect(rect);
}
void CSetupState::EnterTextSlots(void)
{
	/*Shared Settings*/
	for (ulong i = 0; i < NUM_LIST_SLOTS - 1; i++)
	{
		/*TxtMsg*/
		//Clear all colors
		this->m_vMsgSlotTxtGui[i].DefaultColor(CLR::CLEAR);
		//Text Color
		this->m_vMsgSlotTxtGui[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
		this->m_vMsgSlotTxtGui[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::DEFAULT);
		this->m_vMsgSlotTxtGui[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::HOVER);
		this->m_vMsgSlotTxtGui[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::MDOWN);
		//Background Color
		this->m_vMsgSlotTxtGui[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
		this->m_vMsgSlotTxtGui[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::DEFAULT);
		this->m_vMsgSlotTxtGui[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::HOVER);
		this->m_vMsgSlotTxtGui[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::MDOWN);
		//Rect
		_RECT<slong> rect;
		rect.x = GRID::kGrid[2][0].x + GRID::kSize.w;
		rect.y += (NUM_LIST_SLOTS - i - 1) * 52;
		rect.w = GRID::kGrid[10][0].x + GRID::kSize.w - rect.x;
		rect.h = 52;
		this->m_vMsgSlotTxtGui[i].SetRect(rect);
		//Text
		Type2<ulong> size = this->m_pText->GetSize("0", 0.7f);
		Type2<slong> text_offset = Type2<slong>(12, 4);
		text_offset.x = slong(text_offset.x * 0.588235f);
		text_offset.y = slong(text_offset.y * 0.588235f);
		this->m_vMsgSlotTxtGui[i].SetTextOffset(text_offset);
		this->m_vMsgSlotTxtGui[i].SetTextScale(0.56f * 0.588235f);
		this->m_vMsgSlotTxtGui[i].SetTextAlignment(Text::EFontAlignment::F_LEFT);


		/*TxtName*/
		this->m_vMsgSlotNameGui[i].DefaultColor(CLR::CLEAR);
		//Text Color
		this->m_vMsgSlotNameGui[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
		this->m_vMsgSlotNameGui[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::DEFAULT);
		this->m_vMsgSlotNameGui[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::HOVER);
		this->m_vMsgSlotNameGui[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::MDOWN);
		//Background Color
		this->m_vMsgSlotNameGui[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
		this->m_vMsgSlotNameGui[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::DEFAULT);
		this->m_vMsgSlotNameGui[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::HOVER);
		this->m_vMsgSlotNameGui[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::MDOWN);
		//Rect
		rect = _RECT<slong>();
		rect.y += (NUM_LIST_SLOTS - i - 1) * 52;
		rect.w = GRID::kGrid[2][0].x + GRID::kSize.w - rect.x;
		rect.h = 52;
		this->m_vMsgSlotNameGui[i].SetRect(rect);
		//Text
		size = this->m_pText->GetSize("0", 0.7f);
		text_offset = Type2<slong>(12, 4);
		text_offset.x = slong(text_offset.x * 0.588235f);
		text_offset.y = slong(text_offset.y * 0.588235f);
		this->m_vMsgSlotNameGui[i].SetTextOffset(text_offset);
		this->m_vMsgSlotNameGui[i].SetTextScale(0.56f * 0.588235f);
		this->m_vMsgSlotNameGui[i].SetTextAlignment(Text::EFontAlignment::F_LEFT);
	}
}
void CSetupState::EnterTextButs(void)
{
	//Button vec
	std::vector<GUI::CButton*> vButs;

	/*Shared Settings*/
	for (ulong i = 0; i < NUM_LIST_BUTS; i++)
	{
		//Clear all colors
		this->m_vMsgListButtons[i].DefaultColor(CLR::CLEAR);
		//Text Color
		this->m_vMsgListButtons[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
		//Background Color
		this->m_vMsgListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
		this->m_vMsgListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::DEFAULT);
		this->m_vMsgListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::HOVER);
		this->m_vMsgListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::MDOWN);
		//Outline Color
		this->m_vMsgListButtons[i].SetOutlineColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
		this->m_vMsgListButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
		this->m_vMsgListButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
		this->m_vMsgListButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
		//Outline Size
		this->m_vMsgListButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::HOVER);
		this->m_vMsgListButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::MDOWN);
		//Rect
		_RECT<slong> rect;
		rect.x = GRID::kGrid[11][0].x - GRID::kSpace.w;
		rect.w = 104;
		rect.h = 104;
		rect.y += (NUM_LIST_BUTS - i - 1) * rect.h;

		this->m_vMsgListButtons[i].SetRect(rect);
		//Text
		Type2<ulong> size = this->m_pText->GetSize("0", 0.7f);
		this->m_vMsgListButtons[i].SetTextOffset(Type2<slong>(0, -(slong)(size.h * 0.12f)));
		this->m_vMsgListButtons[i].SetTextScale(0.7f);
		this->m_vMsgListButtons[i].SetTextAlignment(Text::EFontAlignment::F_CENTER);
		//Push Back
		vButs.push_back(&this->m_vMsgListButtons[i]);
	}

	/*Group Settings*/
	this->m_vButtonGroup[MSG_BUTS].SetButtonList(vButs);
	this->m_vButtonGroup[MSG_BUTS].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[MSG_BUTS].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[MSG_BUTS].SetInputCMD(this->m_pInput);
	this->m_vButtonGroup[MSG_BUTS].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
}
void CSetupState::EnterDefaultSlots(void)
{
	//Early Out
	if (!this->Host())
		return;

	//Button vec
	std::vector<GUI::CButton*> vButs;

	/*Shared Settings*/
	for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
	{
		//Clear all colors
		this->m_vDefaultSlotButtons[i].DefaultColor(CLR::CLEAR);
		//Text Color
		this->m_vDefaultSlotButtons[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
		this->m_vDefaultSlotButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON);
		this->m_vDefaultSlotButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_HOVER);
		this->m_vDefaultSlotButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_vDefaultSlotButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF);
		this->m_vDefaultSlotButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_vDefaultSlotButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_MDOWN);
		//Background Color
		this->m_vDefaultSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
		this->m_vDefaultSlotButtons[i].SetBGColor(CLR::SLATE, GUI::CButton::EButtonState::ON);
		this->m_vDefaultSlotButtons[i].SetBGColor(CLR::SLATE, GUI::CButton::EButtonState::ON_HOVER);
		this->m_vDefaultSlotButtons[i].SetBGColor(CLR::SLATE, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_vDefaultSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF);
		this->m_vDefaultSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_vDefaultSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_MDOWN);
		//Outline Color
		this->m_vDefaultSlotButtons[i].SetOutlineColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
		this->m_vDefaultSlotButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::ON);
		this->m_vDefaultSlotButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::ON_HOVER);
		this->m_vDefaultSlotButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_vDefaultSlotButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::OFF);
		this->m_vDefaultSlotButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_vDefaultSlotButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::OFF_MDOWN);
		//Outline Size
		this->m_vDefaultSlotButtons[i].SetOutlineSize(2, GUI::CButton::EButtonState::ON);
		this->m_vDefaultSlotButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::ON_HOVER);
		this->m_vDefaultSlotButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_vDefaultSlotButtons[i].SetOutlineSize(2, GUI::CButton::EButtonState::OFF);
		this->m_vDefaultSlotButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_vDefaultSlotButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::OFF_MDOWN);
		//Rect
		_RECT<slong> rect = GRID::kGrid[13][6];
		rect.y += (NUM_LIST_SLOTS - i - 1) * 52;
		rect.w = 3 * (GRID::kSize.w + GRID::kSpace.w) - GRID::kSpace.w;
		rect.h = 48;
		this->m_vDefaultSlotButtons[i].SetRect(rect);
		//Text
		Type2<ulong> size = this->m_pText->GetSize("0", 0.7f);
		Type2<slong> text_offset = Type2<slong>(12, 0);
		text_offset.x = slong(text_offset.x * 0.588235f);
		text_offset.y = slong(text_offset.y * 0.588235f);
		this->m_vDefaultSlotButtons[i].SetTextOffset(text_offset);
		this->m_vDefaultSlotButtons[i].SetTextScale(0.56f * 0.588235f);
		//this->m_vDefaultSlotButtons[i].SetButtonGroupDir(MAPBUTS, LEFT);
		this->m_vDefaultSlotButtons[i].SetTextAlignment(Text::EFontAlignment::F_LEFT);
		//Push Back
		vButs.push_back(&this->m_vDefaultSlotButtons[i]);
	}

	/*Individual Settings*/
	this->m_vDefaultSlotButtons[FIRST].SetButtonGroupDir(-2, UP);

	/*Group Settings*/
	this->m_vButtonGroup[DEFAULT_SLOTS].SetButtonList(vButs);
	this->m_vButtonGroup[DEFAULT_SLOTS].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[DEFAULT_SLOTS].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[DEFAULT_SLOTS].SetInputCMD(this->m_pInput);
	//this->m_vButtonGroup[DEFAULT_SLOTS].SetFindClosestButtonGroup(true);
	this->m_vButtonGroup[DEFAULT_SLOTS].SetMoveType(GUI::CButton::EMoveType::PREVIOUS);
}
void CSetupState::EnterDefaultButs(void)
{
	//Early Out
	if (!this->Host())
		return;

	//Button vec
	std::vector<GUI::CButton*> vButs;

	/*Shared Settings*/
	for (ulong i = 0; i < NUM_LIST_BUTS; i++)
	{
		//Clear all Colors
		this->m_vDefaultListButtons[i].DefaultColor(CLR::CLEAR);
		//Text Color
		this->m_vDefaultListButtons[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
		//Background Color
		this->m_vDefaultListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
		this->m_vDefaultListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::DEFAULT);
		this->m_vDefaultListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::HOVER);
		this->m_vDefaultListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::MDOWN);
		//Outline Color
		this->m_vDefaultListButtons[i].SetOutlineColor(CLR::CLEAR, GUI::CButton::EButtonState::INACTIVE);
		this->m_vDefaultListButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
		this->m_vDefaultListButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
		this->m_vDefaultListButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
		//Outline Size
		this->m_vDefaultListButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::HOVER);
		this->m_vDefaultListButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::MDOWN);
		//Rect
		_RECT<slong> rect = GRID::kGrid[16][6];
		rect.y += (NUM_LIST_BUTS - i - 1) * (GRID::kSize.h + GRID::kSpace.h);
		this->m_vDefaultListButtons[i].SetRect(rect);
		//Text
		Type2<ulong> size = this->m_pText->GetSize("0", 0.7f);
		this->m_vDefaultListButtons[i].SetTextOffset(Type2<slong>(0, -(slong)(size.h * 0.12f)));
		this->m_vDefaultListButtons[i].SetTextScale(0.7f);
		this->m_vDefaultListButtons[i].SetTextAlignment(Text::EFontAlignment::F_CENTER);
		vButs.push_back(&this->m_vDefaultListButtons[i]);
	}

	//Group
	this->m_vButtonGroup[DEFAULT_BUTS].SetButtonList(vButs);
	this->m_vButtonGroup[DEFAULT_BUTS].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[DEFAULT_BUTS].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[DEFAULT_BUTS].SetInputCMD(this->m_pInput);
	this->m_vButtonGroup[DEFAULT_BUTS].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
}
void CSetupState::EnterCustomSlots(void)
{
	//Early Out
	if (!this->Host())
		return;

	//Button vec
	std::vector<GUI::CButton*> vButs;

	/*Shared Settings*/
	for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
	{
		//Clear all colors
		this->m_vCustomSlotButtons[i].DefaultColor(CLR::CLEAR);
		//Text Color
		this->m_vCustomSlotButtons[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
		this->m_vCustomSlotButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON);
		this->m_vCustomSlotButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_HOVER);
		this->m_vCustomSlotButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_vCustomSlotButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF);
		this->m_vCustomSlotButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_vCustomSlotButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_MDOWN);
		//Background Color
		this->m_vCustomSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
		this->m_vCustomSlotButtons[i].SetBGColor(CLR::SLATE, GUI::CButton::EButtonState::ON);
		this->m_vCustomSlotButtons[i].SetBGColor(CLR::SLATE, GUI::CButton::EButtonState::ON_HOVER);
		this->m_vCustomSlotButtons[i].SetBGColor(CLR::SLATE, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_vCustomSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF);
		this->m_vCustomSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_vCustomSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_MDOWN);
		//Outline Color
		this->m_vCustomSlotButtons[i].SetOutlineColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
		this->m_vCustomSlotButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::ON);
		this->m_vCustomSlotButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::ON_HOVER);
		this->m_vCustomSlotButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_vCustomSlotButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::OFF);
		this->m_vCustomSlotButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_vCustomSlotButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::OFF_MDOWN);
		//Outline Size
		this->m_vCustomSlotButtons[i].SetOutlineSize(2, GUI::CButton::EButtonState::ON);
		this->m_vCustomSlotButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::ON_HOVER);
		this->m_vCustomSlotButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_vCustomSlotButtons[i].SetOutlineSize(2, GUI::CButton::EButtonState::OFF);
		this->m_vCustomSlotButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_vCustomSlotButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::OFF_MDOWN);
		//Rect
		_RECT<slong> rect = GRID::kGrid[13][3];
		rect.y += (NUM_LIST_SLOTS - i - 1) * 52;
		rect.w = 3 * (GRID::kSize.w + GRID::kSpace.w) - GRID::kSpace.w;
		rect.h = 48;
		this->m_vCustomSlotButtons[i].SetRect(rect);
		//Text
		Type2<ulong> size = this->m_pText->GetSize("0", 0.7f);
		Type2<slong> text_offset = Type2<slong>(12, 0);
		text_offset.x = slong(text_offset.x * 0.588235f);
		text_offset.y = slong(text_offset.y * 0.588235f);
		this->m_vCustomSlotButtons[i].SetTextOffset(text_offset);
		this->m_vCustomSlotButtons[i].SetTextScale(0.56f * 0.588235f);
		this->m_vCustomSlotButtons[i].SetTextAlignment(Text::EFontAlignment::F_LEFT);
		vButs.push_back(&this->m_vCustomSlotButtons[i]);
	}

	/*Group Settings*/
	this->m_vButtonGroup[CUSTOM_SLOTS].SetButtonList(vButs);
	this->m_vButtonGroup[CUSTOM_SLOTS].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[CUSTOM_SLOTS].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[CUSTOM_SLOTS].SetInputCMD(this->m_pInput);
	//this->m_vButtonGroup[CUSTOM_SLOTS].SetFindClosestButtonGroup(true);
	this->m_vButtonGroup[CUSTOM_SLOTS].SetMoveType(GUI::CButton::EMoveType::PREVIOUS);
}
void CSetupState::EnterCustomButs(void)
{
	//Early Out
	if (!this->Host())
		return;

	//Button vec
	std::vector<GUI::CButton*> vButs;

	/*Shared Settings*/
	for (ulong i = 0; i < NUM_LIST_BUTS; i++)
	{
		//Clear all colors
		this->m_vCustomListButtons[i].DefaultColor(CLR::CLEAR);
		//Text Color
		this->m_vCustomListButtons[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
		//Background Color
		this->m_vCustomListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
		this->m_vCustomListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::DEFAULT);
		this->m_vCustomListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::HOVER);
		this->m_vCustomListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::MDOWN);
		//Outline Color
		this->m_vCustomListButtons[i].SetOutlineColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
		this->m_vCustomListButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
		this->m_vCustomListButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
		this->m_vCustomListButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
		//Outline Size
		this->m_vCustomListButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::HOVER);
		this->m_vCustomListButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::MDOWN);
		//Rect
		_RECT<slong> rect = GRID::kGrid[16][3];
		rect.y += (NUM_LIST_BUTS - i - 1) * (GRID::kSize.h + GRID::kSpace.h);
		this->m_vCustomListButtons[i].SetRect(rect);
		//Text
		Type2<ulong> size = this->m_pText->GetSize("0", 0.7f);
		this->m_vCustomListButtons[i].SetTextOffset(Type2<slong>(0, -(slong)(size.h * 0.12f)));
		this->m_vCustomListButtons[i].SetTextScale(0.7f);
		this->m_vCustomListButtons[i].SetTextAlignment(Text::EFontAlignment::F_CENTER);
		//Push Back
		vButs.push_back(&this->m_vCustomListButtons[i]);
	}

	/*Group Settings*/
	this->m_vButtonGroup[CUSTOM_BUTS].SetButtonList(vButs);
	this->m_vButtonGroup[CUSTOM_BUTS].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[CUSTOM_BUTS].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[CUSTOM_BUTS].SetInputCMD(this->m_pInput);
	this->m_vButtonGroup[CUSTOM_BUTS].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
}
//Input
void CSetupState::InputPrimary(void)
{
	if (this->m_vButtons[BACK].GetClick().Pull())
		this->m_tResult.Prev(IGameState::EGameStateType::gs_PRELOBBY);
}
void CSetupState::InputGrid(void)
{
	for (uchar x = 0; x < Board::kslBoardSize; x++)
	{
		for (uchar y = 0; y < Board::kslBoardSize; y++)
		{
			if (this->m_vGrid[x][y].GetClick().Pull())
			{
				CPanel panel = this->m_cBoard.GetPanel(x, y);
				auto vec = panel.GetSlots();
				if (!vec.empty() && vec[0] != -1)
					this->SendSlotRequest(vec[0]);
			}
		}
	}
}
void CSetupState::InputNames(void)
{
	for (ulong i = 0; i < Game::kulMaxPlayers; ++i)
	{
		if (this->m_vNameButtons[i].GetClick().Pull())
		{
			if (this->m_vNameButtons[i].GetToggle())
			{
				//Set Active
				for (ulong j = 0; j < NUM_BUTTON_GROUPS; j++)
					this->m_vButtonGroup[j].SetActive(false);
				this->m_vButtonGroup[PRIMARY].SetActive(true);
				this->m_vNameButtons[i].SetActive(true);
				std::string text = this->m_vNameButtons[i].GetText();
				if (text.length() > strlen("Player"))
				{
					text.erase(text.begin() + strlen("Player"), text.end());
					if (text == std::string("Player"))
						this->m_vNameButtons[i].SetText("");
				}
				//Set Text
				this->m_pInput->Text_Set_Mode(true);
				this->m_pInput->Text_Set_String(this->m_vNameButtons[i].GetText().c_str());
				this->m_pInput->Text_Set_Marker(this->m_vNameButtons[i].GetText().length());
			}
			else
			{
				//Set Text
				this->m_pInput->Text_Set_Mode(false);
				if (!this->m_vNameButtons[i].GetText().empty())
					this->m_vClientData[i].m_strName = this->m_vNameButtons[i].GetText();
				else
					this->m_vClientData[i].m_strName = std::string("Player ") + std::to_string(i + 1);
				//Set Active
				this->DetermineActiveButtons();
				//Send Data to Server
				this->SendClientData();
			}
		}
	}
}
void CSetupState::InputClientTypes(void)
{
	//Early Out
	if (!this->Host())
		return;

	for (ulong i = 0; i < Game::kulMaxPlayers; ++i)
	{
		if (this->m_vClientTypeButtons[i].GetClick().Pull())
		{
			CNetMsg msg(NM_CLIENT_TYPE);
			msg.WriteLong(i);
			this->m_pClient->send_msg(msg);


		}
	}
}
void CSetupState::InputTxtMsg(void)
{
	if (this->m_cTextMsgButton.GetClick().Pull())
	{
		if (this->m_cTextMsgButton.GetToggle())
		{
			//Set Active
			for (ulong j = 0; j < NUM_BUTTON_GROUPS; j++)
				this->m_vButtonGroup[j].SetActive(false);
			this->m_vButtonGroup[PRIMARY].SetActive(true);
			this->m_cTextMsgButton.SetActive(true);
			std::string text = this->m_cTextMsgButton.GetText();
			//Set Text
			this->m_pInput->Text_Set_Mode(true);
			this->m_pInput->Text_Set_String(this->m_cTextMsgButton.GetText().c_str());
			this->m_pInput->Text_Set_Marker(this->m_cTextMsgButton.GetText().length());
		}
		else
		{
			//Set Text
			this->m_pInput->Text_Set_Mode(false);
			//If we have txt send the msg
			if (!this->m_cTextMsgButton.GetText().empty())
			{
				this->SendTxtMsg(this->m_cTextMsgButton.GetText().c_str());
				this->m_cTextMsgButton.SetText("");
			}
			//Set Active
			this->DetermineActiveButtons();
		}
	}
}
void CSetupState::InputTxtButs(void)
{
	slong dir = 0;
	if (this->m_vMsgListButtons[INC].GetClick().Pull())
		dir++;
	else if (this->m_vMsgListButtons[DEC].GetClick().Pull())
		dir--;

	//Early Out
	if (dir == 0)
		return;

	//Move the offset
	this->m_slMsgOffset -= dir;
	this->m_slMsgOffset = clamp<slong>(this->m_slMsgOffset, 0, this->m_vTxtMsgs.size() - 1);

	//Reset the txt on the buttons
	this->DetermineActiveButtons();
}
void CSetupState::InputDefaultSlots(void)
{
	for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
	{
		if (this->m_vDefaultSlotButtons[i].GetClick().Pull())
		{
			if (this->m_vDefaultSlotButtons[i].GetToggle())
			{
				for (ulong j = 0; j < NUM_LIST_SLOTS; j++)
					this->m_vDefaultSlotButtons[j].SetToggle(false);
				this->m_vDefaultSlotButtons[i].SetToggle(true);
				bool bSendBoard = i + this->m_slDefaultOffset != this->m_slDefaultIndex; //If we clicked on a different slot, then send the board msg
				this->m_slDefaultIndex = i + this->m_slDefaultOffset;
				this->m_bDefault = true;
				if (bSendBoard)
					this->SendBoard();
			}
			else if (this->m_bDefault)
			{
				//Force it back to active
				this->m_vDefaultSlotButtons[i].SetToggle(true);
			}
			this->DetermineActiveButtons();
		}
	}
}
void CSetupState::InputDefaultButs(void)
{
	slong dir = 0;
	if (this->m_vDefaultListButtons[INC].GetClick().Pull())
		dir++;
	else if (this->m_vDefaultListButtons[DEC].GetClick().Pull())
		dir--;

	//Early Out
	if (dir == 0)
		return;

	slong original_index = this->m_slDefaultIndex;

	//If the index is -1, then just move the offset
	if (this->m_slDefaultIndex == -1)
	{
		this->m_slDefaultOffset -= dir;
		this->m_slDefaultOffset = clamp<slong>(this->m_slDefaultOffset, 0, this->m_vDefaultList.size() - 1);
	}
	//If Dec
	else if (dir < 0)
	{
		//If the index is at the very end of the list
		if (this->m_slDefaultIndex == this->m_vDefaultList.size() - 1)
		{
			//If the offset has room to move
			if (this->m_slDefaultOffset < slong(this->m_vDefaultList.size()) - 1)
			{
				//Move the offset
				this->m_slDefaultOffset++;
				this->m_slDefaultOffset = clamp<slong>(this->m_slDefaultOffset, 0, this->m_vDefaultList.size() - 1);
				//Adjust the buttons
				ulong index = -1;
				for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
				{
					if (this->m_vDefaultSlotButtons[i].GetToggle())
					{
						index = i - 1;
						this->m_vDefaultSlotButtons[index].SetToggle(true);
						this->m_vDefaultSlotButtons[i].SetToggle(false);
						break;
					}
				}
				//Adjust the index
				this->m_slDefaultIndex = clamp<slong>(index + this->m_slDefaultOffset, 0, this->m_vDefaultList.size() - 1);
			}
			else
			{
				//Do nothing, 
			}
		}
		//The selected button is on the bottom
		else if (this->m_vDefaultSlotButtons[NUM_LIST_SLOTS - 1].GetToggle())
		{
			//Move the offset
			this->m_slDefaultOffset++;
			this->m_slDefaultOffset = clamp<slong>(this->m_slDefaultOffset, 0, this->m_vDefaultList.size() - 1);
			//Adjust the index
			this->m_slDefaultIndex = clamp<slong>(NUM_LIST_SLOTS - 1 + this->m_slDefaultOffset, 0, this->m_vDefaultList.size() - 1);
		}
		else
		{
			ulong index = -1;
			for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
			{
				if (this->m_vDefaultSlotButtons[i].GetToggle())
				{
					index = i + 1;
					this->m_vDefaultSlotButtons[index].SetToggle(true);
					this->m_vDefaultSlotButtons[i].SetToggle(false);
					break;
				}
			}
			//Adjust the index
			this->m_slDefaultIndex = clamp<slong>(index + this->m_slDefaultOffset, 0, this->m_vDefaultList.size() - 1);
		}
	}
	//If Inc
	else if (dir > 0)
	{
		//If the index is at the very front of the list
		if (this->m_slDefaultIndex == 0)
		{
			//Do Nothing
		}
		//If the selected button is the top one
		else if (this->m_vDefaultSlotButtons[0].GetToggle())
		{
			//Move the offset
			this->m_slDefaultOffset--;
			this->m_slDefaultOffset = clamp<slong>(this->m_slDefaultOffset, 0, this->m_vDefaultList.size() - 1);
			//Adjust the index
			this->m_slDefaultIndex = clamp<slong>(0 + this->m_slDefaultOffset, 0, this->m_vDefaultList.size() - 1);
		}
		else
		{
			//Adjust the buttons
			ulong index = -1;
			for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
			{
				if (this->m_vDefaultSlotButtons[i].GetToggle())
				{
					index = i - 1;
					this->m_vDefaultSlotButtons[index].SetToggle(true);
					this->m_vDefaultSlotButtons[i].SetToggle(false);
					break;
				}
			}
			//Adjust the index
			this->m_slDefaultIndex = clamp<slong>(index + this->m_slDefaultOffset, 0, this->m_vDefaultList.size() - 1);
		}
	}
	this->DetermineActiveButtons();

	if (original_index != this->m_slDefaultIndex)
		this->SendBoard();
}
void CSetupState::InputCustomSlots(void)
{
	for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
	{
		if (this->m_vCustomSlotButtons[i].GetClick().Pull())
		{
			if (this->m_vCustomSlotButtons[i].GetToggle())
			{
				for (ulong j = 0; j < NUM_LIST_SLOTS; j++)
					this->m_vCustomSlotButtons[j].SetToggle(false);
				this->m_vCustomSlotButtons[i].SetToggle(true);
				bool bSendBoard = i + this->m_slCustomOffset != this->m_slCustomIndex; //If we clicked on a different slot, then send the board msg
				this->m_slCustomIndex = i + this->m_slCustomOffset;
				this->m_bDefault = false;
				if (bSendBoard)
					this->SendBoard();
			}
			else if (!this->m_bDefault)
			{
				//Force it back to active
				this->m_vCustomSlotButtons[i].SetToggle(true);
			}
			this->DetermineActiveButtons();
		}
	}
}
void CSetupState::InputCustomButs(void)
{
	slong dir = 0;
	if (this->m_vCustomListButtons[INC].GetClick().Pull())
		dir++;
	else if (this->m_vCustomListButtons[DEC].GetClick().Pull())
		dir--;

	//Early Out
	if (dir == 0)
		return;

	slong original_index = this->m_slCustomIndex;

	//If the index is -1, then just move the offset
	if (this->m_slCustomIndex == -1)
	{
		this->m_slCustomOffset -= dir;
		this->m_slCustomOffset = clamp<slong>(this->m_slCustomOffset, 0, this->m_vCustomList.size() - 1);
	}
	//If Dec
	else if (dir < 0)
	{
		//If the index is at the very end of the list
		if (this->m_slCustomIndex == this->m_vCustomList.size() - 1)
		{
			//If the offset has room to move
			if (this->m_slCustomOffset < slong(this->m_vCustomList.size()) - 1)
			{
				//Move the offset
				this->m_slCustomOffset++;
				this->m_slCustomOffset = clamp<slong>(this->m_slCustomOffset, 0, this->m_vCustomList.size() - 1);
				//Adjust the buttons
				ulong index = -1;
				for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
				{
					if (this->m_vCustomSlotButtons[i].GetToggle())
					{
						index = i - 1;
						this->m_vCustomSlotButtons[index].SetToggle(true);
						this->m_vCustomSlotButtons[i].SetToggle(false);
						break;
					}
				}
				//Adjust the index
				this->m_slCustomIndex = clamp<slong>(index + this->m_slCustomOffset, 0, this->m_vCustomList.size() - 1);
			}
			else
			{
				//Do nothing, 
			}
		}
		//The selected button is on the bottom
		else if (this->m_vCustomSlotButtons[NUM_LIST_SLOTS - 1].GetToggle())
		{
			//Move the offset
			this->m_slCustomOffset++;
			this->m_slCustomOffset = clamp<slong>(this->m_slCustomOffset, 0, this->m_vCustomList.size() - 1);
			//Adjust the index
			this->m_slCustomIndex = clamp<slong>(NUM_LIST_SLOTS - 1 + this->m_slCustomOffset, 0, this->m_vCustomList.size() - 1);
		}
		else
		{
			ulong index = -1;
			for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
			{
				if (this->m_vCustomSlotButtons[i].GetToggle())
				{
					index = i + 1;
					this->m_vCustomSlotButtons[index].SetToggle(true);
					this->m_vCustomSlotButtons[i].SetToggle(false);
					break;
				}
			}
			//Adjust the index
			this->m_slCustomIndex = clamp<slong>(index + this->m_slCustomOffset, 0, this->m_vCustomList.size() - 1);
		}
	}
	//If Inc
	else if (dir > 0)
	{
		//If the index is at the very front of the list
		if (this->m_slCustomIndex == 0)
		{
			//Do Nothing
		}
		//If the selected button is the top one
		else if (this->m_vCustomSlotButtons[0].GetToggle())
		{
			//Move the offset
			this->m_slCustomOffset--;
			this->m_slCustomOffset = clamp<slong>(this->m_slCustomOffset, 0, this->m_vCustomList.size() - 1);
			//Adjust the index
			this->m_slCustomIndex = clamp<slong>(0 + this->m_slCustomOffset, 0, this->m_vCustomList.size() - 1);
		}
		else
		{
			//Adjust the buttons
			ulong index = -1;
			for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
			{
				if (this->m_vCustomSlotButtons[i].GetToggle())
				{
					index = i - 1;
					this->m_vCustomSlotButtons[index].SetToggle(true);
					this->m_vCustomSlotButtons[i].SetToggle(false);
					break;
				}
			}
			//Adjust the index
			this->m_slCustomIndex = clamp<slong>(index + this->m_slCustomOffset, 0, this->m_vCustomList.size() - 1);
		}
	}
	this->DetermineActiveButtons();

	if (original_index != this->m_slCustomIndex)
		this->SendBoard();
}
//Update
void CSetupState::UpdateClient(Time dDelta)
{
	if (this->m_pClient == NULL)
		return;

	this->m_pClient->read_msg();

	std::vector<CNetMsg> vec = this->m_pClient->GetMsgs();
	this->m_pClient->SetMsgsClear();
	ulong size = vec.size();
	for (ulong i = 0; i < size; ++i)
	{
		switch (vec[i].GetType())
		{
		case ID_DISCONNECTION_NOTIFICATION:
		case ID_CONNECTION_LOST:
			this->RecvDisconnect(vec[i]);
			break;
		case NM_LOBBY_DATA:
			this->RecvLobbyData(vec[i]);
			break;
		case NM_TXTMSG:
			this->RecvTxtMsg(vec[i]);
			break;
		case NM_BOARD:
			this->RecvBoard(vec[i]);
			break;
		}
	}

}
void CSetupState::UpdateNames(Time dDelta)
{
	bool bUpdateMarker = false;
	//Clamping text names
	for (ulong i = 0; i < Game::kulMaxPlayers; ++i)
	{
		if (this->m_vNameButtons[i].GetToggle())
		{
			//Check the size of the text fits in the rect
			_RECT<slong> tRect = this->m_vNameButtons[i].GetRect();
			Type2<ulong> tSize = this->m_pText->GetSize((this->m_pInput->Text_Get_String() + std::string("_")).c_str(), this->m_vNameButtons[i].GetTextScale());
			if (slong(tSize.w) > tRect.w - this->m_vNameButtons[i].GetTextOffset().x)
			{
				//Backspace until it fits
				do
				{
					slong slMarker = this->m_pInput->Text_Get_Marker();
					std::string strText = this->m_pInput->Text_Get_String();
					strText.erase(strText.begin() + slMarker - 1);
					this->m_pInput->Text_Set_String(strText.c_str());
					if (ulong(slMarker) < strText.length())
						this->m_pInput->Text_Set_Marker(slMarker - 1);
					tSize = this->m_pText->GetSize((this->m_pInput->Text_Get_String() + std::string("_")).c_str(), this->m_vNameButtons[i].GetTextScale());
				} while (slong(tSize.w) > tRect.w - this->m_vNameButtons[i].GetTextOffset().x);
			}
			this->m_vNameButtons[i].SetText(this->m_pInput->Text_Get_String());
			bUpdateMarker = true;
		}
	}
	if (bUpdateMarker)
	{
		this->m_dMarkerTimer -= dDelta;
		if (this->m_dMarkerTimer < 0.0f)
		{
			this->m_bMarkerVisible = !this->m_bMarkerVisible;
			this->m_dMarkerTimer = kdMarkerRate;
		}
	}
}
void CSetupState::UpdateTxtMsg(Time dDelta)
{
	bool bUpdateMarker = false;
	if (this->m_cTextMsgButton.GetToggle())
	{
		//Check the size of the text fits in the rect
		_RECT<slong> tRect = this->m_cTextMsgButton.GetRect();
		Type2<ulong> tSize = this->m_pText->GetSize((this->m_pInput->Text_Get_String() + std::string("_")).c_str(), this->m_cTextMsgButton.GetTextScale());
		if (slong(tSize.w) > tRect.w - this->m_cTextMsgButton.GetTextOffset().x)
		{
			//Backspace until it fits
			do
			{
				slong slMarker = this->m_pInput->Text_Get_Marker();
				std::string strText = this->m_pInput->Text_Get_String();
				strText.erase(strText.begin() + slMarker - 1);
				this->m_pInput->Text_Set_String(strText.c_str());
				if (ulong(slMarker) < strText.length())
					this->m_pInput->Text_Set_Marker(slMarker - 1);
				tSize = this->m_pText->GetSize((this->m_pInput->Text_Get_String() + std::string("_")).c_str(), this->m_cTextMsgButton.GetTextScale());
			} while (slong(tSize.w) > tRect.w - this->m_cTextMsgButton.GetTextOffset().x);
		}
		this->m_cTextMsgButton.SetText(this->m_pInput->Text_Get_String());
		bUpdateMarker = true;
	}
	if (bUpdateMarker)
	{
		this->m_dMarkerTimer -= dDelta;
		if (this->m_dMarkerTimer < 0.0f)
		{
			this->m_bMarkerVisible = !this->m_bMarkerVisible;
			this->m_dMarkerTimer = kdMarkerRate;
		}
	}
}
//Rendering
void CSetupState::RenderButtons(void)
{
	for (ulong i = 0; i < NUM_BUTTON_GROUPS; i++)
	{
		//Ignore the slots, we do that in 'RenderText()'
		if (i == DEFAULT_SLOTS || i == CUSTOM_SLOTS || i == NAMES || i == CLIENT_TYPES)
			continue;
		this->m_vButtonGroup[i].Render(this->m_pSprite, this->m_pText);
	}
	//GUI
	m_cTxtMsgGui.Render(this->m_pSprite, this->m_pText);
	for (ulong i = 0; i < NUM_LIST_SLOTS - 1; ++i)
	{
		this->m_vMsgSlotNameGui[i].Render(this->m_pSprite, this->m_pText);
		this->m_vMsgSlotTxtGui[i].Render(this->m_pSprite, this->m_pText);
	}
	//Special outline for chat box
	_RECT<slong> rect = this->m_vMsgSlotNameGui[2].GetRect();
	rect.w = GRID::kGrid[11][0].x - GRID::kSpace.w;
	rect.h = rect.h * 3;
	this->m_pSprite->DrawRect(rect, CLR::GOLDENROD, false, 4);
}
void CSetupState::RenderText(void)
{
	Type2<ulong> size = this->m_pText->GetSize("0", 1.0f);
	_RECT<slong> rect;

#pragma region NAMES
	for (ulong i = 0; i < Game::kulMaxPlayers; ++i)
	{
		if (!this->m_vNameButtons[i].GetToggle())
		{
			this->m_vNameButtons[i].Render(this->m_pSprite, this->m_pText);
		}
		else
		{
			rect = this->m_vNameButtons[i].GetRect();
			Type2<slong> offset = this->m_vNameButtons[i].GetTextOffset();
			_COLOR<uchar> clr = this->m_vNameButtons[i].GetTextColor(this->m_vNameButtons[i].GetState());

			slong marker = this->m_pInput->Text_Get_Marker();
			if (marker == this->m_vNameButtons[i].GetText().length() || !this->m_vNameButtons[i].GetToggle())
			{
				//Text
				this->m_vNameButtons[i].Render(this->m_pSprite, this->m_pText);
				//Marker
				if (this->m_bMarkerVisible && this->m_vNameButtons[i].GetActive())
				{
					size = this->m_pText->GetSize(this->m_vNameButtons[i].GetText().c_str(), this->m_vNameButtons[i].GetTextScale());
					rect.x += offset.w + size.w;
					rect.y += offset.h;
					this->m_pText->Write("_", rect, clr, this->m_vNameButtons[i].GetTextScale(), true, Text::EFontAlignment::F_LEFT);
				}
			}
			else
			{
				//Text
				std::string name = this->m_vNameButtons[i].GetText();
				this->m_vNameButtons[i].SetText("");
				std::string alpha, omega;
				alpha = omega = name;
				alpha.erase(alpha.begin() + marker, alpha.end());
				omega.erase(omega.begin(), omega.begin() + marker);

				this->m_vNameButtons[i].Render(this->m_pSprite, this->m_pText);
				this->m_vNameButtons[i].SetText(name);

				rect.x += offset.w;
				rect.y += offset.h;
				this->m_pText->Write(alpha.c_str(), rect, clr, this->m_vNameButtons[i].GetTextScale(), true, Text::EFontAlignment::F_LEFT); //Alpha Word
				rect.x += this->m_pText->GetSize(alpha.c_str(), this->m_vNameButtons[i].GetTextScale(), false).w;
				//if (this->m_bMarkerVisible && this->m_cMapNameButton.GetActive()) //Marker
				this->m_pText->Write("_", rect, clr, this->m_vNameButtons[i].GetTextScale(), true, Text::EFontAlignment::F_LEFT);
				rect.x += this->m_pText->GetSize("_", this->m_vNameButtons[i].GetTextScale(), true).w;
				this->m_pText->Write(omega.c_str(), rect, clr, this->m_vNameButtons[i].GetTextScale(), true, Text::EFontAlignment::F_LEFT); //Omega Word
			}
		}
	}
#pragma endregion
#pragma region TXTMSG
	if (!this->m_cTextMsgButton.GetToggle())
	{
		this->m_cTextMsgButton.Render(this->m_pSprite, this->m_pText);
	}
	else
	{
		rect = this->m_cTextMsgButton.GetRect();
		Type2<slong> offset = this->m_cTextMsgButton.GetTextOffset();
		_COLOR<uchar> clr = this->m_cTextMsgButton.GetTextColor(this->m_cTextMsgButton.GetState());

		slong marker = this->m_pInput->Text_Get_Marker();
		if (marker == this->m_cTextMsgButton.GetText().length() || !this->m_cTextMsgButton.GetToggle())
		{
			//Text
			this->m_cTextMsgButton.Render(this->m_pSprite, this->m_pText);
			//Marker
			if (this->m_bMarkerVisible && this->m_cTextMsgButton.GetActive())
			{
				size = this->m_pText->GetSize(this->m_cTextMsgButton.GetText().c_str(), this->m_cTextMsgButton.GetTextScale());
				rect.x += offset.w + size.w;
				rect.y += offset.h;
				this->m_pText->Write("_", rect, clr, this->m_cTextMsgButton.GetTextScale(), true, Text::EFontAlignment::F_LEFT);
			}
		}
		else
		{
			//Text
			std::string name = this->m_cTextMsgButton.GetText();
			this->m_cTextMsgButton.SetText("");
			std::string alpha, omega;
			alpha = omega = name;
			alpha.erase(alpha.begin() + marker, alpha.end());
			omega.erase(omega.begin(), omega.begin() + marker);

			this->m_cTextMsgButton.Render(this->m_pSprite, this->m_pText);
			this->m_cTextMsgButton.SetText(name);

			rect.x += offset.w;
			rect.y += offset.h;
			this->m_pText->Write(alpha.c_str(), rect, clr, this->m_cTextMsgButton.GetTextScale(), true, Text::EFontAlignment::F_LEFT); //Alpha Word
			rect.x += this->m_pText->GetSize(alpha.c_str(), this->m_cTextMsgButton.GetTextScale(), false).w;
			//if (this->m_bMarkerVisible && this->m_cMapNameButton.GetActive()) //Marker
			this->m_pText->Write("_", rect, clr, this->m_cTextMsgButton.GetTextScale(), true, Text::EFontAlignment::F_LEFT);
			rect.x += this->m_pText->GetSize("_", this->m_cTextMsgButton.GetTextScale(), true).w;
			this->m_pText->Write(omega.c_str(), rect, clr, this->m_cTextMsgButton.GetTextScale(), true, Text::EFontAlignment::F_LEFT); //Omega Word
		}
	}
#pragma endregion

	if (this->Host())
	{
#pragma region Default_Boards
		size = this->m_pText->GetSize("Default Boards", 0.51f);
		rect = GRID::kGrid[13][8];
		rect.w = 4 * (GRID::kSize.w + GRID::kSpace.w) - GRID::kSpace.w;
		this->m_pText->Write("Default Boards", rect, CLR::LAVENDER, 0.51f, true, Text::EFontAlignment::F_CENTER);
		this->m_pSprite->DrawLine(rect.x, rect.y, rect.x + rect.w, rect.y, CLR::WHITE, 2);
#pragma endregion
#pragma region Custom_Boards
		size = this->m_pText->GetSize("Custom Boards", 0.53f);
		rect = GRID::kGrid[13][5];
		rect.w = 4 * (GRID::kSize.w + GRID::kSpace.w) - GRID::kSpace.w;
		this->m_pText->Write("Custom Boards", rect, CLR::LAVENDER, 0.53f, true, Text::EFontAlignment::F_CENTER);
		this->m_pSprite->DrawLine(rect.x, rect.y, rect.x + rect.w, rect.y, CLR::WHITE, 2);
#pragma endregion
#pragma region DEFAULT_LIST
		for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
		{
			if (this->m_vDefaultSlotButtons[i].GetActive())
				this->m_vDefaultSlotButtons[i].Render(this->m_pSprite, this->m_pText);
			else if (!this->m_vDefaultSlotButtons[i].GetToggle())
			{
				GUI::CButton::EButtonState eState = this->m_vDefaultSlotButtons[i].GetState();
				rect = this->m_vDefaultSlotButtons[i].GetRect();
				this->m_pSprite->DrawRect(rect, this->m_vDefaultSlotButtons[i].GetBGColor(eState));
				this->m_pSprite->DrawRect(rect, this->m_vDefaultSlotButtons[i].GetOutlineColor(eState), false, this->m_vDefaultSlotButtons[i].GetOutlineSize(eState));
				rect.x += this->m_vDefaultSlotButtons[i].GetTextOffset().x;
				rect.y += this->m_vDefaultSlotButtons[i].GetTextOffset().y;
				this->m_pText->Write(this->m_vDefaultSlotButtons[i].GetText().c_str(), rect, this->m_vDefaultSlotButtons[i].GetTextColor(eState), this->m_vDefaultSlotButtons[i].GetTextScale());
			}
			else
			{
				GUI::CButton::EButtonState eState = GUI::CButton::EButtonState::ON;
				rect = this->m_vDefaultSlotButtons[i].GetRect();
				this->m_pSprite->DrawRect(rect, this->m_vDefaultSlotButtons[i].GetBGColor(eState));
				this->m_pSprite->DrawRect(rect, this->m_vDefaultSlotButtons[i].GetOutlineColor(eState), false, this->m_vDefaultSlotButtons[i].GetOutlineSize(eState));
				rect.x += this->m_vDefaultSlotButtons[i].GetTextOffset().x;
				rect.y += this->m_vDefaultSlotButtons[i].GetTextOffset().y;
				this->m_pText->Write(this->m_vDefaultSlotButtons[i].GetText().c_str(), rect, this->m_vDefaultSlotButtons[i].GetTextColor(GUI::CButton::EButtonState::ON), this->m_vDefaultSlotButtons[i].GetTextScale());
			}
		}
#pragma endregion
#pragma region CUSTOM_LIST
		for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
		{
			if (this->m_vCustomSlotButtons[i].GetActive())
				this->m_vCustomSlotButtons[i].Render(this->m_pSprite, this->m_pText);
			else if (!this->m_vCustomSlotButtons[i].GetToggle())
			{
				GUI::CButton::EButtonState eState = this->m_vCustomSlotButtons[i].GetState();
				rect = this->m_vCustomSlotButtons[i].GetRect();
				this->m_pSprite->DrawRect(rect, this->m_vCustomSlotButtons[i].GetBGColor(eState));
				this->m_pSprite->DrawRect(rect, this->m_vCustomSlotButtons[i].GetOutlineColor(eState), false, this->m_vCustomSlotButtons[i].GetOutlineSize(eState));
				rect.x += this->m_vCustomSlotButtons[i].GetTextOffset().x;
				rect.y += this->m_vCustomSlotButtons[i].GetTextOffset().y;
				this->m_pText->Write(this->m_vCustomSlotButtons[i].GetText().c_str(), rect, this->m_vCustomSlotButtons[i].GetTextColor(eState), this->m_vCustomSlotButtons[i].GetTextScale());
			}
			else
			{
				GUI::CButton::EButtonState eState = GUI::CButton::EButtonState::ON;
				rect = this->m_vCustomSlotButtons[i].GetRect();
				this->m_pSprite->DrawRect(rect, this->m_vCustomSlotButtons[i].GetBGColor(eState));
				this->m_pSprite->DrawRect(rect, this->m_vCustomSlotButtons[i].GetOutlineColor(eState), false, this->m_vCustomSlotButtons[i].GetOutlineSize(eState));
				rect.x += this->m_vCustomSlotButtons[i].GetTextOffset().x;
				rect.y += this->m_vCustomSlotButtons[i].GetTextOffset().y;
				this->m_pText->Write(this->m_vCustomSlotButtons[i].GetText().c_str(), rect, this->m_vCustomSlotButtons[i].GetTextColor(eState), this->m_vCustomSlotButtons[i].GetTextScale());
			}
		}
		/*float scale = this->m_cMapNameButton.GetTextScale() * 0.588235f;
		Type2<slong> offset = this->m_cMapNameButton.GetOffset();
		offset.x = slong(offset.x * 0.588235f);
		offset.y = slong(offset.y * 0.588235f);
		for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
		{
		if (i + this->m_slCustomOffset >= this->m_vCustomList.size())
		break;
		rect = this->m_vCustomSlotButtons[i].GetRect();
		rect.x += offset.x;
		rect.y += offset.y;

		this->m_pText->Write(this->m_vCustomList[i + this->m_slCustomOffset].GetMapName().c_str(), rect,
		this->m_vCustomSlotButtons[i].GetTextColor(this->m_vCustomSlotButtons[i].GetState()), scale);
		}*/
#pragma endregion
	}
#pragma region Arrows
	for (ulong i = 0; i < NUM_LIST_BUTS; i++)
	{
		//Default Buts
		_COLOR<uchar> clr = this->m_vDefaultListButtons[i].GetOutlineColor(this->m_vDefaultListButtons[i].GetState());
		if (!this->m_vDefaultListButtons[i].GetActive())
			clr = this->m_vDefaultListButtons[i].GetTextColor(GUI::CButton::EButtonState::INACTIVE);
		rect = this->m_vDefaultListButtons[i].GetRect();
		Type2<slong> p0, p1, p2;
		p0 = rect.GetPos() + Type2<slong>(16, 16);
		p1 = rect.GetPos() + rect.GetSize() - Type2<slong>(16, rect.GetSize().h - 16);
		p2 = rect.GetPos() + rect.GetSize() - Type2<slong>(slong(rect.GetSize().w * 0.5f), 16);
		if (i == 1)
		{
			slong y = p2.y;
			p2.y = p0.y;
			p0.y = p1.y = y;
		}
		if (this->Host())
		{
			this->m_pSprite->DrawLine(p0, p1, clr, 3);
			this->m_pSprite->DrawLine(p1, p2, clr, 3);
			this->m_pSprite->DrawLine(p2, p0, clr, 3);
		}

		//Custom Buts
		clr = this->m_vCustomListButtons[i].GetOutlineColor(this->m_vCustomListButtons[i].GetState());
		if (!this->m_vCustomListButtons[i].GetActive())
			clr = this->m_vCustomListButtons[i].GetTextColor(GUI::CButton::EButtonState::INACTIVE);
		rect = this->m_vCustomListButtons[i].GetRect();
		p0 = rect.GetPos() + Type2<slong>(16, 16);
		p1 = rect.GetPos() + rect.GetSize() - Type2<slong>(16, rect.GetSize().h - 16);
		p2 = rect.GetPos() + rect.GetSize() - Type2<slong>(slong(rect.GetSize().w * 0.5f), 16);
		if (i == 1)
		{
			slong y = p2.y;
			p2.y = p0.y;
			p0.y = p1.y = y;
		}
		if (this->Host())
		{
			this->m_pSprite->DrawLine(p0, p1, clr, 3);
			this->m_pSprite->DrawLine(p1, p2, clr, 3);
			this->m_pSprite->DrawLine(p2, p0, clr, 3);
		}
		//Msg Buts
		clr = this->m_vMsgListButtons[i].GetOutlineColor(this->m_vMsgListButtons[i].GetState());
		if (!this->m_vMsgListButtons[i].GetActive())
			clr = this->m_vMsgListButtons[i].GetTextColor(GUI::CButton::EButtonState::INACTIVE);
		rect = this->m_vMsgListButtons[i].GetRect();
		p0 = rect.GetPos() + Type2<slong>(16, 16);
		p1 = rect.GetPos() + rect.GetSize() - Type2<slong>(16, rect.GetSize().h - 16);
		p2 = rect.GetPos() + rect.GetSize() - Type2<slong>(slong(rect.GetSize().w * 0.5f), 16);
		if (i == 1)
		{
			slong y = p2.y;
			p2.y = p0.y;
			p0.y = p1.y = y;
		}
		this->m_pSprite->DrawLine(p0, p1, clr, 3);
		this->m_pSprite->DrawLine(p1, p2, clr, 3);
		this->m_pSprite->DrawLine(p2, p0, clr, 3);
	}
#pragma endregion
}

void CSetupState::RenderClientType(void)
{
	//Early Out
	if (!this->Host())
		return;

	this->m_vButtonGroup[CLIENT_TYPES].Render(this->m_pSprite, this->m_pText);
	for (ulong i = 0; i < Game::kulMaxPlayers; ++i)
	{
		_RECT<slong> rect = this->m_vClientTypeButtons[i].GetRect();
		_COLOR<uchar> clr = this->m_vClientTypeButtons[i].GetOutlineColor(this->m_vClientTypeButtons[i].GetState());
		if (!this->m_vClientTypeButtons[i].GetActive())
			clr = this->m_vClientTypeButtons[i].GetTextColor(GUI::CButton::EButtonState::INACTIVE);
		Type2<slong> p0, p1, p2;
		p0 = rect.GetPos() + Type2<slong>(16, 16);
		p1 = rect.GetPos() + rect.GetSize() + Type2<slong>(-rect.w + 16, -16);
		p2 = rect.GetPos() + rect.GetSize() + Type2<slong>(-16, -slong(GRID::kSize.h * 0.5f));
		this->m_pSprite->DrawLine(p0, p1, clr, 3);
		this->m_pSprite->DrawLine(p1, p2, clr, 3);
		this->m_pSprite->DrawLine(p2, p0, clr, 3);
	}
}
//Saving & Loading
void CSetupState::LoadDefault(void)
{
	this->m_vDefaultList.clear();
	std::ifstream fin;
	fin.open("DefaultBoard.bin", std::ios_base::in | std::ios_base::binary);
	if (fin.is_open())
	{
		//Get length of file
		fin.seekg(0, fin.end);
		ulong size = ulong(fin.tellg());
		fin.seekg(0, fin.beg);

		//Allocate buffer memory
		char* pBuffer = new char[size];
		fin.read(pBuffer, size);
		fin.close();

		//Parse the buffer
		ulong offset = 0;
		ulong list_size = *(ulong*)(pBuffer + offset);
		offset += sizeof(ulong);
		for (ulong i = 0; i < list_size; ++i)
		{
			CBoard board;
			//board.LoadBoard(fin);
			offset += board.LoadBoard(pBuffer + offset);
			this->m_vDefaultList.push_back(board);
		}

		//Cleanup
		delete[]pBuffer;
	}
	this->DetermineActiveButtons();
}
void CSetupState::LoadCustom(void)
{
	this->m_vCustomList.clear();
	std::ifstream fin;
	fin.open("CustomBoard.bin", std::ios_base::in | std::ios_base::binary);
	if (fin.is_open())
	{
		//Get length of file
		fin.seekg(0, fin.end);
		ulong size = ulong(fin.tellg());
		fin.seekg(0, fin.beg);

		//Allocate buffer memory
		char* pBuffer = new char[size];
		fin.read(pBuffer, size);
		fin.close();

		//Parse the buffer
		ulong offset = 0;
		ulong list_size = *(ulong*)(pBuffer + offset);
		offset += sizeof(ulong);
		for (ulong i = 0; i < list_size; ++i)
		{
			CBoard board;
			//board.LoadBoard(fin);
			offset += board.LoadBoard(pBuffer + offset);
			this->m_vCustomList.push_back(board);
		}

		//Cleanup
		delete[]pBuffer;
	}
	this->DetermineActiveButtons();
}
void CSetupState::SortBoards(void)
{
	CBoard cBoard;
	//Default
	if (!this->m_vDefaultList.empty() && this->m_slDefaultIndex != -1)
	{
		if (this->m_bDefault)
		{
			cBoard = this->m_vDefaultList[this->m_slDefaultIndex];
			std::sort(this->m_vDefaultList.begin(), this->m_vDefaultList.end());
			if (!this->m_bDefault)
			{
				for (ulong i = 0; i < this->m_vDefaultList.size(); i++)
				{
					if (cBoard == this->m_vDefaultList[i])
					{
						this->m_slDefaultIndex = i;
						this->m_slDefaultOffset = i;
						break;
					}
				}
			}
		}
	}
	//Custom
	if (!this->m_vCustomList.empty() && this->m_slCustomIndex != -1)
	{
		if (!this->m_bDefault)
		{
			cBoard = this->m_vCustomList[this->m_slCustomIndex];
			std::vector<CBoard> vec = this->m_vCustomList;
			std::sort(this->m_vCustomList.begin(), this->m_vCustomList.end());
			if (!this->m_bDefault && vec != this->m_vCustomList)
			{
				for (ulong i = 0; i < this->m_vCustomList.size(); i++)
				{
					if (cBoard == this->m_vCustomList[i])
					{
						this->m_slCustomIndex = i;
						this->m_slCustomOffset = i;
						break;
					}
				}
			}
		}
	}
}
//Other
void CSetupState::DetermineActiveButtons(void)
{
	//Grid
	this->SetBoard();
	//Name Buttons
	for (ulong i = 0; i < Game::kulMaxPlayers; ++i)
	{
		this->m_vNameButtons[i].SetActive(i == this->m_pClient->GetPeer().m_scID);
		if (this->m_vClientData[i].m_scColor != -1)
		{
			this->m_vNameButtons[i].SetTextColor(kutPlayerColors[this->m_vClientData[i].m_scColor], GUI::CButton::EButtonState::OFF);
			this->m_vNameButtons[i].SetTextColor(kutPlayerColors[this->m_vClientData[i].m_scColor], GUI::CButton::EButtonState::OFF_HOVER);
			this->m_vNameButtons[i].SetTextColor(kutPlayerColors[this->m_vClientData[i].m_scColor], GUI::CButton::EButtonState::OFF_HOVER);
			this->m_vNameButtons[i].SetTextColor(kutPlayerColors[this->m_vClientData[i].m_scColor], GUI::CButton::EButtonState::INACTIVE);
		}
		else if (this->m_vClientData[i].m_eType == Game::EPlayerType::CLIENT)
		{
			this->m_vNameButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF);
			this->m_vNameButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_HOVER);
			this->m_vNameButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_HOVER);
			this->m_vNameButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::INACTIVE);
		}
		else
		{
			this->m_vNameButtons[i].SetTextColor(CLR::LAVENDER, GUI::CButton::EButtonState::INACTIVE);
		}
	}

	//Client Type Buttons
	for (ulong i = 0; i < Game::kulMaxPlayers; ++i)
	{
		if (!this->Host() || this->m_vClientData[i].m_eType == Game::EPlayerType::CLIENT)
			this->m_vClientTypeButtons[i].SetActive(false);
		else
			this->m_vClientTypeButtons[i].SetActive(true);
	}
	//Set List Buttons
	for (ulong i = 0; i < NUM_LIST_BUTS; i++)
	{
		this->m_vDefaultListButtons[i].SetActive(true);
		this->m_vCustomListButtons[i].SetActive(true);
		this->m_vMsgListButtons[i].SetActive(true);
	}
	//Board List Toggle
	if (this->m_bDefault)
	{
		this->m_slCustomIndex = -1;
		for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
			this->m_vCustomSlotButtons[i].SetToggle(false);
	}
	else
	{
		this->m_slDefaultIndex = -1;
		for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
			this->m_vDefaultSlotButtons[i].SetToggle(false);
	}
	//Set Default Slot Buttons
	for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
	{
		this->m_vDefaultSlotButtons[i].SetToggle(i + this->m_slDefaultOffset == this->m_slDefaultIndex);
		this->m_vDefaultSlotButtons[i].SetActive(i + this->m_slDefaultOffset < this->m_vDefaultList.size());
	}
	//Change text for each button
	for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
	{
		slong index = i + this->m_slDefaultOffset;
		if (index >= slong(this->m_vDefaultList.size()))
			this->m_vDefaultSlotButtons[i].SetText("");
		else
			this->m_vDefaultSlotButtons[i].SetText(this->m_vDefaultList[index].GetMapName());
	}
	//Set Custom Slot Buttons
	for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
	{
		this->m_vCustomSlotButtons[i].SetToggle(i + this->m_slCustomOffset == this->m_slCustomIndex);
		this->m_vCustomSlotButtons[i].SetActive(i + this->m_slCustomOffset < this->m_vCustomList.size());
	}
	//Change text for each button
	for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
	{
		slong index = i + this->m_slCustomOffset;
		if (index >= slong(this->m_vCustomList.size()))
			this->m_vCustomSlotButtons[i].SetText("");
		else
			this->m_vCustomSlotButtons[i].SetText(this->m_vCustomList[index].GetMapName());
	}
	//Set Txt Msg List Text 
	for (ulong i = 0; i < 3; i++)
	{
		slong index = i + this->m_slMsgOffset;
		if (index >= slong(this->m_vTxtMsgs.size()))
		{
			this->m_vMsgSlotNameGui[i].SetText("");
			this->m_vMsgSlotTxtGui[i].SetText("");
		}
		else
		{
			//Names
			if (this->m_vTxtMsgs[index].first > -1)
			{
				if (i > 0 && index > 0 && this->m_vTxtMsgs[index - 1].first == this->m_vTxtMsgs[index].first) //If the prev msg, was from the same person, then don't rewrite the name
					this->m_vMsgSlotNameGui[i].SetText("");
				else
					this->m_vMsgSlotNameGui[i].SetText(this->m_vClientData[this->m_vTxtMsgs[index].first].m_strName.c_str());
				if (this->m_vClientData[this->m_vTxtMsgs[index].first].m_scColor != -1)
					this->m_vMsgSlotNameGui[i].SetTextColor(kutPlayerColors[this->m_vClientData[this->m_vTxtMsgs[index].first].m_scColor], GUI::CButton::EButtonState::DEFAULT);
				else
					this->m_vMsgSlotNameGui[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::DEFAULT);
			}
			else
			{
				this->m_vMsgSlotNameGui[i].SetText("GAME");
				this->m_vMsgSlotNameGui[i].SetTextColor(CLR::LAVENDER, GUI::CButton::EButtonState::DEFAULT);
			}
			//Msg
			this->m_vMsgSlotTxtGui[i].SetText(this->m_vTxtMsgs[index].second.c_str());
			if (this->m_vTxtMsgs[index].first > -1)
			{
				if (this->m_vClientData[this->m_vTxtMsgs[index].first].m_scColor != -1)
					this->m_vMsgSlotTxtGui[i].SetTextColor(kutPlayerColors[this->m_vClientData[this->m_vTxtMsgs[index].first].m_scColor], GUI::CButton::EButtonState::DEFAULT);
				else
					this->m_vMsgSlotTxtGui[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::DEFAULT);
			}
			else
				this->m_vMsgSlotTxtGui[i].SetTextColor(CLR::LAVENDER, GUI::CButton::EButtonState::DEFAULT);
		}
	}
	//Set Txt Msg Button
	this->m_cTextMsgButton.SetActive(true);
}
void CSetupState::SetBoard(void)
{
	//if (this->m_bDefault)
	//{
	//	if (this->m_slDefaultIndex == -1 || this->m_vDefaultList.empty())
	//		return;
	//	this->m_cBoard = this->m_vDefaultList[this->m_slDefaultIndex];
	//}
	//else
	//{
	//	if (this->m_slCustomIndex == -1 || this->m_vCustomList.empty())
	//		return;
	//	this->m_cBoard = this->m_vCustomList[this->m_slCustomIndex];
	//}

	//Set the Grid
	for (uchar x = 0; x < Board::kslBoardSize; x++)
	{
		for (uchar y = 0; y < Board::kslBoardSize; y++)
		{
			bool bActive = this->m_cBoard.GetPanel(x, y).GetActive();
			this->m_vGrid[x][y].SetActive(bActive);
			if (bActive)
			{
				CPanel panel = this->m_cBoard.GetPanel(x, y);
				auto vec = panel.GetSlots();
				if (!vec.empty() && vec[0] != -1)
				{
					_COLOR<uchar> clr = kutPlayerColors[vec[0]];
					bool bTaken = false;
					for (uchar i = 0; i < Game::kulMaxPlayers; ++i)
					{
						if (this->m_vClientData[i].m_scColor == vec[0])
							bTaken = true;
					}
					if (!bTaken)
					{
						this->m_vGrid[x][y].SetBGColor(clr * 1.00f, GUI::CButton::EButtonState::DEFAULT);
						this->m_vGrid[x][y].SetBGColor(clr * 0.75f, GUI::CButton::EButtonState::HOVER);
						this->m_vGrid[x][y].SetBGColor(clr * 0.50f, GUI::CButton::EButtonState::MDOWN);
						this->m_vGrid[x][y].SetBGColor(clr * 0.25f, GUI::CButton::EButtonState::INACTIVE);
					}
					else
					{
						this->m_vGrid[x][y].SetBGColor(clr * 0.25f, GUI::CButton::EButtonState::DEFAULT);
						this->m_vGrid[x][y].SetBGColor(clr * 0.25f, GUI::CButton::EButtonState::HOVER);
						this->m_vGrid[x][y].SetBGColor(clr * 0.25f, GUI::CButton::EButtonState::MDOWN);
						this->m_vGrid[x][y].SetBGColor(clr * 0.25f, GUI::CButton::EButtonState::INACTIVE);
					}
				}
				else
				{
					this->m_vGrid[x][y].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::DEFAULT);
					this->m_vGrid[x][y].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::HOVER);
					this->m_vGrid[x][y].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::MDOWN);
					this->m_vGrid[x][y].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
					this->m_vGrid[x][y].SetActive(false);
				}
				this->m_vGrid[x][y].SetText(std::to_string(vec.size()));

				//Test for text mode
				if (this->m_cTextMsgButton.GetToggle())
					this->m_vGrid[x][y].SetActive(false);
			}
			else
			{
				this->m_vGrid[x][y].SetText("");
				this->m_vGrid[x][y].SetBGColor(CLR::CLEAR, GUI::CButton::EButtonState::DEFAULT);
				this->m_vGrid[x][y].SetBGColor(CLR::CLEAR, GUI::CButton::EButtonState::HOVER);
				this->m_vGrid[x][y].SetBGColor(CLR::CLEAR, GUI::CButton::EButtonState::MDOWN);
				this->m_vGrid[x][y].SetBGColor(CLR::CLEAR, GUI::CButton::EButtonState::INACTIVE);
			}
		}
	}
}
//Network
void CSetupState::SetLobbyData(void)
{
	for (ulong i = 0; i < Game::kulMaxPlayers; ++i)
	{
		switch (this->m_vClientData[i].m_eType)
		{
		case Game::EPlayerType::CLIENT:
			this->m_vNameButtons[i].SetText(this->m_vClientData[i].m_strName.c_str());
			m_vNameButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::INACTIVE);
			break;
		case Game::EPlayerType::CLOSED:
			this->m_vNameButtons[i].SetText("CLOSED");
			this->m_vNameButtons[i].SetTextColor(CLR::LAVENDER, GUI::CButton::EButtonState::INACTIVE);
			break;
		case Game::EPlayerType::COM:
			this->m_vNameButtons[i].SetText("COM");
			this->m_vNameButtons[i].SetTextColor(CLR::LAVENDER, GUI::CButton::EButtonState::INACTIVE);
			break;
		case Game::EPlayerType::OPEN:
			this->m_vNameButtons[i].SetText("OPEN");
			this->m_vNameButtons[i].SetTextColor(CLR::LAVENDER, GUI::CButton::EButtonState::INACTIVE);
			break;
			break;
		}
	}
	this->DetermineActiveButtons();
}
void CSetupState::RecvLobbyData(Network::CNetMsg &msg)
{
	for (ulong i = 0; i < Game::kulMaxPlayers; ++i)
	{
		this->m_vClientData[i].m_strName = msg.ReadString();
		this->m_vClientData[i].m_eType = Game::EPlayerType(msg.ReadByte());
		this->m_vClientData[i].m_scColor = msg.ReadByte();
	}
	this->SetLobbyData();
	this->SetBoard();
}
void CSetupState::RecvDisconnect(Network::CNetMsg &msg)
{
	//this->m_tResult.Prev(IGameState::EGameStateType::gs_PRELOBBY);
	this->m_cPUTimedState.SetGotoType(IGameState::EGameStateType::gs_PRELOBBY);
	this->m_cPUTimedState.SetTimer(4.0f);
	GUI::CButton cBut;
	cBut.SetRect(_RECT<slong>(0, slong(Sprite::SCREEN_HEIGHT * 0.5f) - 64, Sprite::SCREEN_WIDTH, 128));
	cBut.SetText("LOBBY CLOSED");
	cBut.SetTextScale(0.9f);
	cBut.SetTextColor(CLR::GREY, GUI::CButton::EButtonState::DEFAULT);
	this->m_cPUTimedState.SetHeader(cBut);
	this->m_tResult.Push(&this->m_cPUTimedState);
}
void CSetupState::RecvTxtMsg(Network::CNetMsg &msg)
{
	bool bSentAll = msg.ReadBool();
	if (bSentAll)
	{
		ulong size = msg.ReadLong();
		this->m_vTxtMsgs.clear();
		this->m_vTxtMsgs.reserve(size);
		for (ulong i = 0; i < size; ++i)
		{
			std::pair < schar, std::string> pair;
			pair.first = msg.ReadByte();
			pair.second = msg.ReadString();
			this->m_vTxtMsgs.push_back(pair);
		}
	}
	else
	{
		std::pair < schar, std::string> pair;
		pair.first = msg.ReadByte();
		pair.second = msg.ReadString();
		this->m_vTxtMsgs.push_back(pair);
	}
	this->m_pAudio->Play(this->m_slSfxMsg, false, true);
	//If we are not currently 'using' the up/down arrow buttons for the list, then reset the list to the top
	if (this->m_vMsgListButtons[INC].GetState() == GUI::CButton::EButtonState::DEFAULT && this->m_vMsgListButtons[DEC].GetState() == GUI::CButton::EButtonState::DEFAULT)
		this->m_slMsgOffset = clamp<slong>(this->m_vTxtMsgs.size() - 3, 0, this->m_vTxtMsgs.size() - 1);
	this->DetermineActiveButtons();
}
void CSetupState::RecvBoard(Network::CNetMsg &msg)
{
	this->m_cBoard.LoadMsg(&msg);
	this->SetBoard();
}
void CSetupState::SendClientData()
{
	schar id = this->m_pClient->GetPeer().m_scID;
	CNetMsg msg(NM_CLIENT_DATA);
	msg.WriteString(this->m_vClientData[id].m_strName.c_str());
	msg.WriteByte(this->m_vClientData[id].m_eType);
	msg.WriteByte(this->m_vClientData[id].m_scColor);
	this->m_pClient->send_msg(msg);
}
void CSetupState::SendTxtMsg(const char* strTxt)
{
	CNetMsg msg(NM_TXTMSG);
	msg.WriteString(strTxt);
	this->m_pClient->send_msg(msg);
}
void CSetupState::SendSlotRequest(const schar scSlot)
{
	CNetMsg msg(NM_SLOT_REQUEST);
	msg.WriteByte(scSlot);
	this->m_pClient->send_msg(msg);
}
void CSetupState::SendBoard(void)
{
	CBoard cBoard;
	if (this->m_bDefault)
	{
		if (this->m_slDefaultIndex == -1 || this->m_vDefaultList.empty())
			return;
		cBoard = this->m_vDefaultList[this->m_slDefaultIndex];
	}
	else
	{
		if (this->m_slCustomIndex == -1 || this->m_vCustomList.empty())
			return;
		cBoard = this->m_vCustomList[this->m_slCustomIndex];
	}

	CNetMsg msg(NM_BOARD);
	cBoard.FillMsg(&msg);

	this->m_pClient->send_msg(msg);
}
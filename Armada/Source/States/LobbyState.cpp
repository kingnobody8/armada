/*
Author:			Daniel Habig
Date:			11-15-13
File:			IntroState.cpp
Purpose:		gamestate that begins the game, shows logos, transitions to StartState
*/
#include "LobbyState.h"
#include "../SDL/Sprite.h"
#include "../Paker/Paker.h"
#include "../Input/InputCMD.h"
#include "../Audio/Audio.h"
#include "../Text/Text.h"
#include "../Board/Board.h"
#include "../Network/Client.h"
#include <algorithm>
using namespace Input;
using namespace Network;
using namespace Board;

const Time kdBroadcastRate = 0.1f;
const slong kslJoinOffset = -28;

CLobbyState::CLobbyState(void)
{
	this->m_eType = EGameStateType::gs_LOBBY;
	this->m_dBroadCastTimer = -1.0f;
	this->m_slServerIndex = -1;
	this->m_slServerOffset = 0;
	this->m_pClient = NULL;
}
CLobbyState::~CLobbyState(void)
{
}

void CLobbyState::Enter(Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText, Audio::CAudio* pAudio)
{
	IGameState::Enter(pSprite, pInput, pText, pAudio);

	//Sfx
	slong sfxBack = this->m_pAudio->LoadAudio("Assets/Sfx/back.wav", Audio::EAudioType::SFX_2D);
	slong sfxPush = this->m_pAudio->LoadAudio("Assets/Sfx/push.wav", Audio::EAudioType::SFX_2D);
	slong sfxPull = this->m_pAudio->LoadAudio("Assets/Sfx/pull.wav", Audio::EAudioType::SFX_2D);
	slong sfxHover = this->m_pAudio->LoadAudio("Assets/Sfx/hover.wav", Audio::EAudioType::SFX_2D);
	slong sfxGood = this->m_pAudio->LoadAudio("Assets/Sfx/beep_good.wav", Audio::EAudioType::SFX_2D);
	slong sfxBad = this->m_pAudio->LoadAudio("Assets/Sfx/beep_bad.wav", Audio::EAudioType::SFX_2D);

	//Setup for the spacing
	Type2<slong> offset = Type2<slong>(16, 44);
	Type2<slong> tile_size = Type2<slong>(16, 16);
	Type2<slong> tile_space = Type2<slong>(16, 16);
	Type2<slong> panel_space = Type2<slong>(16, 16);
	Type2<slong> panel_size = (tile_size + tile_space) * 3;

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
		m_vButtons[i].SetSfx(sfxPush, GUI::CButton::EButtonSfx::SFX_PULL);

		vButs.push_back(&m_vButtons[i]);
	}

	//Back Button
	Type2<ulong> tSize;
	tSize = this->m_pText->GetSize("Back", 1.0f);
	m_vButtons[BACK].SetText("Back");
	m_vButtons[BACK].SetRect(_RECT<slong>(Sprite::SCREEN_WIDTH - tSize.x - 32, 0, tSize.x, tSize.y));
	m_vButtons[BACK].SetSfx(sfxBack, GUI::CButton::EButtonSfx::SFX_PULL);

	//Group
	this->m_vButtonGroup[PRIMARY].SetButtonList(vButs);
	this->m_vButtonGroup[PRIMARY].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[PRIMARY].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[PRIMARY].SetInputCMD(this->m_pInput);
	this->m_vButtonGroup[PRIMARY].SetFindClosestButtonGroup(true);
	this->m_vButtonGroup[PRIMARY].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
	vButs.clear();
#pragma endregion
#pragma region SERVER_BUTS
	//NOTE: Hack turning off slider
#if(0)
	//Slider
	{
		this->m_cServerSlider.DefaultColor(CLR::CLEAR);
		this->m_cServerSlider.SetOrientation(GUI::CSlider::EOrientation::VERT);
		Type2<slong> pos = Type2<slong>(Sprite::SCREEN_WIDTH - offset.x - panel_size.x, offset.y + this->m_vButtons[BACK].GetRect().h + panel_size.h);
		Type2<slong> size = Type2<slong>(panel_size.w, (NUM_LIST_SLOTS - 2) * panel_size.h + (NUM_LIST_SLOTS - 3) * panel_space.h);
		this->m_cServerSlider.SetRect(_RECT<slong>(pos.x, pos.y, size.w, size.h));
		//Background
		this->m_cServerSlider.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::DEFAULT);
		this->m_cServerSlider.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::HOVER);
		this->m_cServerSlider.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::MDOWN);
		//Outline
		this->m_cServerSlider.SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
		this->m_cServerSlider.SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
		this->m_cServerSlider.SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
		//BarL
		this->m_cServerSlider.SetBarColor(CLR::GREEN * 0.50f, GUI::CButton::EButtonState::DEFAULT);
		this->m_cServerSlider.SetBarColor(CLR::GREEN * 0.40f, GUI::CButton::EButtonState::HOVER);
		this->m_cServerSlider.SetBarColor(CLR::GREEN * 0.30f, GUI::CButton::EButtonState::MDOWN);
		//Slider
		this->m_cServerSlider.SetSliderColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
		this->m_cServerSlider.SetSliderColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
		this->m_cServerSlider.SetSliderColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
		//Outline Size
		this->m_cServerSlider.SetOutlineSize(6, GUI::CButton::EButtonState::DEFAULT);
		this->m_cServerSlider.SetOutlineSize(6, GUI::CButton::EButtonState::HOVER);
		this->m_cServerSlider.SetOutlineSize(6, GUI::CButton::EButtonState::MDOWN);
		//Sfx
		this->m_cServerSlider.SetSfx(sfxHover, GUI::CButton::EButtonSfx::SFX_HOVER);
		//Push Back
		vButs.push_back(&m_cServerSlider);
	}
#endif
	//Arrows
	for (ulong i = 0; i < NUM_LIST_BUTS; i++)
	{
		for (ulong j = 0; j < GUI::CButton::EButtonState::NUM_STATES; j++)
			m_vServerListButtons[i].DefaultColor(CLR::CLEAR);
		//Text
		this->m_vServerListButtons[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
		//Background
		this->m_vServerListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
		this->m_vServerListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::DEFAULT);
		this->m_vServerListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::HOVER);
		this->m_vServerListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::MDOWN);
		//Outline
		this->m_vServerListButtons[i].SetOutlineColor(CLR::CLEAR, GUI::CButton::EButtonState::INACTIVE);
		this->m_vServerListButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
		this->m_vServerListButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
		this->m_vServerListButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
		//Outline Size
		this->m_vServerListButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::HOVER);
		this->m_vServerListButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::MDOWN);
		//Alignment
		this->m_vServerListButtons[i].SetTextAlignment(Text::EFontAlignment::F_CENTER);
		//Rect
		_RECT<slong> rect = _RECT<slong>(0, 0, panel_size.w, panel_size.h);
		rect.x = Sprite::SCREEN_WIDTH - offset.x - rect.w;
		if (i == 0)
			rect.y = Sprite::SCREEN_HEIGHT - offset.y - rect.h;
		else
			rect.y = Sprite::SCREEN_HEIGHT - offset.y - NUM_LIST_SLOTS * panel_size.h - (NUM_LIST_SLOTS - 1) * panel_space.h;
		this->m_vServerListButtons[i].SetRect(rect);
		//Push Back
		vButs.push_back(&this->m_vServerListButtons[i]);
	}

	//Group
	this->m_vButtonGroup[SERVER_BUTS].SetButtonList(vButs);
	this->m_vButtonGroup[SERVER_BUTS].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[SERVER_BUTS].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[SERVER_BUTS].SetInputCMD(this->m_pInput);
	this->m_vButtonGroup[SERVER_BUTS].SetButtonGroupDir(DOWN, PRIMARY);
	this->m_vButtonGroup[SERVER_BUTS].SetButtonGroupDir(LEFT, SERVER_SLOTS);
	//this->m_vButtonGroup[DEFAULT_BUTS].SetFindClosestButtonGroup(true);
	this->m_vButtonGroup[SERVER_BUTS].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
	vButs.clear();
#pragma endregion
#pragma region SERVER_SLOTS
	for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
	{
		m_vServerSlotButtons[i].DefaultColor(CLR::CLEAR);
		m_vServerSlotButtons[i].SetToggle(false);

		//Text
		this->m_vServerSlotButtons[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
		this->m_vServerSlotButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON);
		this->m_vServerSlotButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_HOVER);
		this->m_vServerSlotButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_vServerSlotButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF);
		this->m_vServerSlotButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_vServerSlotButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_MDOWN);
		//Background
		this->m_vServerSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
		this->m_vServerSlotButtons[i].SetBGColor(CLR::SLATE, GUI::CButton::EButtonState::ON);
		this->m_vServerSlotButtons[i].SetBGColor(CLR::SLATE, GUI::CButton::EButtonState::ON_HOVER);
		this->m_vServerSlotButtons[i].SetBGColor(CLR::SLATE, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_vServerSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF);
		this->m_vServerSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_vServerSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_MDOWN);
		//Outline
		this->m_vServerSlotButtons[i].SetOutlineColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
		this->m_vServerSlotButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::ON);
		this->m_vServerSlotButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::ON_HOVER);
		this->m_vServerSlotButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_vServerSlotButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::OFF);
		this->m_vServerSlotButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_vServerSlotButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::OFF_MDOWN);
		//Outline Size
		this->m_vServerSlotButtons[i].SetOutlineSize(2, GUI::CButton::EButtonState::ON);
		this->m_vServerSlotButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::ON_HOVER);
		this->m_vServerSlotButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_vServerSlotButtons[i].SetOutlineSize(2, GUI::CButton::EButtonState::OFF);
		this->m_vServerSlotButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_vServerSlotButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::OFF_MDOWN);
		//Alignment
		this->m_vServerSlotButtons[i].SetTextAlignment(Text::EFontAlignment::F_LEFT);
		//Rect
		//_RECT<slong> rect = _RECT<slong>(0, 0, panel_size.w * 4 - panel_space.w, panel_size.h);
		//rect.x = Sprite::SCREEN_WIDTH - offset.x - (panel_size.w + panel_space.w) - rect.w;
		//rect.y = Sprite::SCREEN_HEIGHT - offset.y - (i + 1) * panel_size.h - i * panel_space.h;
		_RECT<slong> rect = GRID::kGrid[12][8];
		rect.w = 4 * GRID::kSize.w + 3 * GRID::kSpace.w;
		rect.h = GRID::kSize.h;
		rect.y -= i * (GRID::kSize.h + GRID::kSpace.h);
		//Sprite::SCREEN_HEIGHT - offset.y - NUM_LIST_SLOTS * panel_size.h - (NUM_LIST_SLOTS - 1) * panel_space.h;
		this->m_vServerSlotButtons[i].SetRect(rect);
		//Text
		this->m_vServerSlotButtons[i].SetTextScale(0.56f);
		this->m_vServerSlotButtons[i].SetTextOffset(Type2<slong>(-12, 0));
		this->m_vServerSlotButtons[i].SetTextAlignment(Text::EFontAlignment::F_RIGHT);
		//Push Back
		vButs.push_back(&this->m_vServerSlotButtons[i]);
	}

	this->m_vServerSlotButtons[FIRST].SetButtonGroupDir(-2, UP);

	//Group
	this->m_vButtonGroup[SERVER_SLOTS].SetButtonList(vButs);
	this->m_vButtonGroup[SERVER_SLOTS].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[SERVER_SLOTS].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[SERVER_SLOTS].SetInputCMD(this->m_pInput);
	this->m_vButtonGroup[SERVER_SLOTS].SetButtonGroupDir(DOWN, PRIMARY);
	this->m_vButtonGroup[SERVER_SLOTS].SetButtonGroupDir(RIGHT, SERVER_BUTS);
	this->m_vButtonGroup[SERVER_SLOTS].SetFindClosestButtonGroup(true);
	this->m_vButtonGroup[SERVER_SLOTS].SetMoveType(GUI::CButton::EMoveType::PREVIOUS);
	vButs.clear();
#pragma endregion
#pragma region JOIN
	this->m_cJoinButton.DefaultColor(CLR::CLEAR);
	this->m_cJoinButton.SetActive(false);

	//Text
	this->m_cJoinButton.SetTextColor(CLR::LAVENDER, GUI::CButton::EButtonState::INACTIVE);
	this->m_cJoinButton.SetTextColor(CLR::GREY, GUI::CButton::EButtonState::DEFAULT);
	this->m_cJoinButton.SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::HOVER);
	this->m_cJoinButton.SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::MDOWN);
	//Background
	this->m_cJoinButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
	this->m_cJoinButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::DEFAULT);
	this->m_cJoinButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::HOVER);
	this->m_cJoinButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::MDOWN);
	//Outline
	this->m_cJoinButton.SetOutlineColor(CLR::CLEAR, GUI::CButton::EButtonState::INACTIVE);
	this->m_cJoinButton.SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
	this->m_cJoinButton.SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
	this->m_cJoinButton.SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
	//Outline Size
	this->m_cJoinButton.SetOutlineSize(6, GUI::CButton::EButtonState::DEFAULT);
	this->m_cJoinButton.SetOutlineSize(6, GUI::CButton::EButtonState::HOVER);
	this->m_cJoinButton.SetOutlineSize(6, GUI::CButton::EButtonState::MDOWN);
	//Sfx
	this->m_cJoinButton.SetSfx(sfxGood, GUI::CButton::EButtonSfx::SFX_PULL);
	//Rect
	_RECT<slong> rect = GRID::kGrid[1][3];
	rect.w = 8 * GRID::kSize.w + 7 * GRID::kSpace.w;
	//_RECT<slong>(offset.x + 4 * (panel_size.x + panel_space.x), offset.y + 3 * (panel_size.x + panel_space.x), panel_size.x * 8 + panel_space.x * 7, panel_size.y);
	this->m_cJoinButton.SetRect(rect);
	this->m_cJoinButton.SetTextScale(1.0f);
	this->m_cJoinButton.SetTextOffset(Type2<slong>(0, kslJoinOffset));
	this->m_cJoinButton.SetTextAlignment(Text::EFontAlignment::F_CENTER);
	this->m_cJoinButton.SetText("JOIN");
	vButs.push_back(&this->m_cJoinButton);

	//Group
	this->m_vButtonGroup[JOINING].SetButtonList(vButs);
	this->m_vButtonGroup[JOINING].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[JOINING].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[JOINING].SetInputCMD(this->m_pInput);
	this->m_vButtonGroup[JOINING].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
	vButs.clear();
#pragma endregion

	//Build the Button  manager
	std::vector<GUI::CButtonGroup> vGroups;
	for (ulong i = 0; i < NUM_BUTTON_GROUPS; i++)
		vGroups.push_back(this->m_vButtonGroup[i]);
	this->m_cButMan.SetGroups(vGroups);

#pragma region NAMES
	for (ulong i = 0; i < Game::kulMaxPlayers; ++i)
	{
		m_vBroadcastNames[i].DefaultColor(CLR::CLEAR);
		m_vBroadcastNames[i].SetActive(false);
		//Background
		m_vBroadcastNames[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
		//Outline
		m_vBroadcastNames[i].SetOutlineColor(CLR::GOLDENROD * 0.5f, GUI::CButton::EButtonState::INACTIVE);
		//Outline Size
		m_vBroadcastNames[i].SetOutlineSize(4, GUI::CButton::EButtonState::INACTIVE);
		//Text
		m_vBroadcastNames[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::INACTIVE);
		//Text Size
		m_vBroadcastNames[i].SetTextScale(0.56f);
		m_vBroadcastNames[i].SetTextAlignment(Text::EFontAlignment::F_LEFT);
		m_vBroadcastNames[i].SetTextOffset(Type2<slong>(12, 0));
		//Rect
		_RECT<slong> rect = this->m_cJoinButton.GetRect();
		rect.x += (i % 2) * (4 * panel_size.w + 4 * panel_space.w);
		rect.y += rect.h + panel_size.h + 2 * panel_space.h - (i / 2) * (panel_size.h + panel_space.h);
		rect.w = 4 * panel_size.w + 3 * panel_space.w;
		//PushBack
		this->m_vBroadcastNames[i].SetRect(rect);
	}

	//_RECT<slong>(offset.x + panel_size.x * 4 + panel_space.x * 3, offset.y + panel_size.x + panel_space.x, panel_size.x * 8 + panel_space.x * 7, panel_size.y);
#pragma endregion

	//Start the client
	this->m_pClient->init();
	//Set the index and offset
	this->m_slServerIndex = -1;
	this->m_slServerOffset = 0;
	//Set the Buttons
	this->DetermineActiveButtons();
}
void CLobbyState::Exit(void)
{
	IGameState::Exit();
	if (this->m_pStarField)
		this->m_pStarField = NULL;
	if (this->m_pClient)
	{
		this->m_pClient->stop();
		this->m_pClient = NULL;
	}
	this->m_vServerList.clear();
	this->m_dBroadCastTimer = -1.0f;
}

void CLobbyState::Input(void)
{
	Input::eInputType eType = this->m_pInput->GetFinalInputType();
	this->m_pInput->Cursor_Lock(eType != MOUSE);

	//Selection
	this->m_cButMan.Selection(this->m_pInput);

	//Confirmation
	bool confirm = false;
	if (eType == Input::eInputType::MOUSE)
		confirm = this->m_pInput->Key_Held(VK_LBUTTON);
	else
		confirm = this->m_pInput->Key_Held(VK_RETURN) || this->m_pInput->Key_Held(VK_SPACE) ||
		this->m_pInput->Ctrl_Held(CTRL_KEYS::XB_A) || this->m_pInput->Ctrl_Held(CTRL_KEYS::START);
	for (ulong i = 0; i < NUM_BUTTON_GROUPS; i++)
		this->m_vButtonGroup[i].Confirmation(confirm);

	//Results
	{
		InputPrimary();
		InputServerButs();
		InputServerSlots();
		InputJoin();
	}
}
void CLobbyState::Update(Time delta, Time gameTime)
{
	this->m_pStarField->Update(delta);
	this->m_cButMan.Update(delta, this->m_pAudio);
	this->UpdateRecvMsg(delta);
	this->UpdateBroadcast(delta);
}
void CLobbyState::Render(void)
{
	this->m_pStarField->Render();
	this->RenderButtons();
	this->RenderText();
	//for (slong x = 0; x < GRID::kGridSize.x; ++x)
	//{
	//	for (slong y = 0; y < GRID::kGridSize.y; ++y)
	//	{
	//		this->m_pSprite->DrawRect(GRID::kGrid[x][y], CLR::CHARTREUSE, false, 4);
	//	}
	//}
}

//Input
void CLobbyState::InputPrimary(void)
{
	if (this->m_vButtons[BACK].GetClick().Pull())
		this->m_tResult.Prev(IGameState::EGameStateType::gs_PRELOBBY);
}
void CLobbyState::InputServerSlots(void)
{
	for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
	{
		if (this->m_vServerSlotButtons[i].GetClick().Pull())
		{
			if (this->m_vServerSlotButtons[i].GetToggle())
			{
				for (ulong j = 0; j < NUM_LIST_SLOTS; j++)
					this->m_vServerSlotButtons[j].SetToggle(false);
				this->m_vServerSlotButtons[i].SetToggle(true);
				this->m_slServerIndex = i + this->m_slServerOffset;
			}
			else
			{
				this->m_slServerIndex = -1;
			}
			this->DetermineActiveButtons();
		}
	}
}
void CLobbyState::InputServerButs(void)
{
	slong dir = 0;
	if (this->m_vServerListButtons[INC].GetClick().Pull())
		dir++;
	else if (this->m_vServerListButtons[DEC].GetClick().Pull())
		dir--;

	//Early Out
	if (dir == 0)
		return;

	//If the index is -1, then just move the offset
	if (dir != 0 && this->m_slServerIndex == -1)
	{
		this->m_slServerOffset -= dir;
		this->m_slServerOffset = clamp<slong>(this->m_slServerOffset, 0, this->m_vServerList.size() - NUM_LIST_SLOTS);
		dir = 0;
	}

	//If Dec
	if (dir < 0)
	{
		//If the index is at the very end of the list
		if (this->m_slServerIndex == this->m_vServerList.size() - 1)
		{
			//If the offset has room to move
			//if (this->m_slServerOffset < slong(this->m_vServerList.size()) - 1)
			//{
			//	//Move the offset
			//	this->m_slServerOffset++;
			//	this->m_slServerOffset = clamp<slong>(this->m_slServerOffset, 0, this->m_vServerList.size() - 1);
			//	//Adjust the buttons
			//	ulong index = -1;
			//	for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
			//	{
			//		if (this->m_vServerSlotButtons[i].GetToggle())
			//		{
			//			index = i - 1;
			//			this->m_vServerSlotButtons[index].SetToggle(true);
			//			this->m_vServerSlotButtons[i].SetToggle(false);
			//			break;
			//		}
			//	}
			//	//Adjust the index
			//	this->m_slServerIndex = clamp<slong>(index + this->m_slServerOffset, 0, this->m_vServerList.size() - 1);
			//}
			//else
			//{
			//	//Do nothing, 
			//}
		}
		//The selected button is on the bottom
		else if (this->m_vServerSlotButtons[NUM_LIST_SLOTS - 1].GetToggle())
		{
			//Move the offset
			this->m_slServerOffset++;
			this->m_slServerOffset = clamp<slong>(this->m_slServerOffset, 0, this->m_vServerList.size() - 1);
			//Adjust the index
			this->m_slServerIndex = clamp<slong>(NUM_LIST_SLOTS - 1 + this->m_slServerOffset, 0, this->m_vServerList.size() - 1);
		}
		else
		{
			slong index = -1;
			for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
			{
				if (this->m_vServerSlotButtons[i].GetToggle())
				{
					index = i + 1;
					this->m_vServerSlotButtons[index].SetToggle(true);
					this->m_vServerSlotButtons[i].SetToggle(false);
					break;
				}
			}
			//Adjust the index
			this->m_slServerIndex = clamp<slong>(index + this->m_slServerOffset, 0, this->m_vServerList.size() - 1);
		}
	}
	//If Inc
	else if (dir > 0)
	{
		//If the index is at the very front of the list
		if (this->m_slServerIndex == 0)
		{
			//Do Nothing
		}
		//If the selected button is the top one
		else if (this->m_vServerSlotButtons[0].GetToggle())
		{
			//Move the offset
			this->m_slServerOffset--;
			this->m_slServerOffset = clamp<slong>(this->m_slServerOffset, 0, this->m_vServerList.size() - 1);
			//Adjust the index
			this->m_slServerIndex = clamp<slong>(0 + this->m_slServerOffset, 0, this->m_vServerList.size() - 1);
		}
		else
		{
			//Adjust the buttons
			slong index = -1;
			for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
			{
				if (this->m_vServerSlotButtons[i].GetToggle())
				{
					index = i - 1;
					this->m_vServerSlotButtons[index].SetToggle(true);
					this->m_vServerSlotButtons[i].SetToggle(false);
					break;
				}
			}
			//Adjust the index
			this->m_slServerIndex = clamp<slong>(index + this->m_slServerOffset, 0, this->m_vServerList.size() - 1);
		}
	}

	this->DetermineActiveButtons();
}
void CLobbyState::InputJoin(void)
{
	if (this->m_cJoinButton.GetClick().Pull())
	{
		TPeer peer = this->m_vServerList[this->m_slServerIndex].GetPeer();
		this->m_pClient->connect_lan(peer.m_Addr.ToString(false), SERVER_PORT);
	}
}
//Update
void CLobbyState::UpdateRecvMsg(Time dDelta)
{
	if (!this->m_pClient->read_msg())
		return;

	auto vec = this->m_pClient->GetMsgs();
	this->m_pClient->SetMsgsClear();
	for (ulong i = 0; i < vec.size(); ++i)
	{
		switch (vec[i].GetType())
		{
		case ID_CONNECTION_LOST:
		case ID_DISCONNECTION_NOTIFICATION:
			this->m_pClient->disconnect();
			break;
		case NM_CLIENT_ID:
			m_cSetupState.SetClient(this->m_pClient);
			m_cSetupState.SetStarField(this->m_pStarField);
			this->m_tResult.Push(&m_cSetupState);
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			this->RecvNoSlots();
			break;
		}
	}
}
void CLobbyState::UpdateBroadcast(Time dDelta)
{
	this->m_dBroadCastTimer -= dDelta;
	if (this->m_dBroadCastTimer < 0.0f)
	{
		this->m_dBroadCastTimer = kdBroadcastRate;
		TPeer prev;
		if (this->m_slServerIndex != -1)
			prev = this->m_vServerList[this->m_slServerIndex].GetPeer();
		//Set the server data
		this->m_vServerList.clear();
		this->m_vServerList = this->m_pClient->GetBroadcast();
		this->m_pClient->SetBroadcastClear();
		//Sort by guid
		std::sort(this->m_vServerList.begin(), this->m_vServerList.end());
		if (this->m_slServerIndex != -1)
		{
			//Find the prev
			this->m_slServerIndex = -1;
			ulong size = this->m_vServerList.size();
			for (ulong i = 0; i < size; ++i)
			{
				if (this->m_vServerList[i].GetPeer() == prev)
					this->m_slServerIndex = i;
			}
		}
		//Reset the buttons
		if (this->m_vServerList.empty())
		{
			this->m_slServerIndex = -1;
			this->m_slServerOffset = 0;
		}
		this->DetermineActiveButtons();
		//Send another broadcast
		this->m_pClient->broadcast(SERVER_PORT);
	}

}
//Rendering
void CLobbyState::RenderButtons(void)
{
	for (ulong i = 0; i < NUM_BUTTON_GROUPS; i++)
	{
		if (i == SERVER_SLOTS)
			continue;
		this->m_vButtonGroup[i].Render(this->m_pSprite, this->m_pText);
	}
}
void CLobbyState::RenderText(void)
{
	Type2<ulong> size = this->m_pText->GetSize("0", 1.0f);
	_RECT<slong> rect;

#pragma region Arrows
	for (ulong i = 0; i < NUM_LIST_BUTS; i++)
	{
		//Default Buts
		_COLOR<uchar> clr = this->m_vServerListButtons[i].GetOutlineColor(this->m_vServerListButtons[i].GetState());
		if (!this->m_vServerListButtons[i].GetActive())
			clr = this->m_vServerListButtons[i].GetTextColor(GUI::CButton::EButtonState::INACTIVE);
		rect = this->m_vServerListButtons[i].GetRect();
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
		this->m_pSprite->DrawLine(p0, p1, clr, 3);
		this->m_pSprite->DrawLine(p1, p2, clr, 3);
		this->m_pSprite->DrawLine(p2, p0, clr, 3);
	}
#pragma endregion
#pragma region SERVER_LIST
	for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
	{
		if (this->m_vServerSlotButtons[i].GetActive())
			this->m_vServerSlotButtons[i].Render(this->m_pSprite, this->m_pText);
		else if (!this->m_vServerSlotButtons[i].GetToggle())
		{
			GUI::CButton::EButtonState eState = this->m_vServerSlotButtons[i].GetState();
			rect = this->m_vServerSlotButtons[i].GetRect();
			this->m_pSprite->DrawRect(rect, this->m_vServerSlotButtons[i].GetBGColor(eState));
			this->m_pSprite->DrawRect(rect, this->m_vServerSlotButtons[i].GetOutlineColor(eState), false, this->m_vServerSlotButtons[i].GetOutlineSize(eState));
			rect.x += this->m_vServerSlotButtons[i].GetTextOffset().x;
			rect.y += this->m_vServerSlotButtons[i].GetTextOffset().y;
			this->m_pText->Write(this->m_vServerSlotButtons[i].GetText().c_str(), rect, this->m_vServerSlotButtons[i].GetTextColor(eState), this->m_vServerSlotButtons[i].GetTextScale());
		}
		else
		{
			GUI::CButton::EButtonState eState = GUI::CButton::EButtonState::ON;
			rect = this->m_vServerSlotButtons[i].GetRect();
			this->m_pSprite->DrawRect(rect, this->m_vServerSlotButtons[i].GetBGColor(eState));
			this->m_pSprite->DrawRect(rect, this->m_vServerSlotButtons[i].GetOutlineColor(eState), false, this->m_vServerSlotButtons[i].GetOutlineSize(eState));
			rect.x += this->m_vServerSlotButtons[i].GetTextOffset().x;
			rect.y += this->m_vServerSlotButtons[i].GetTextOffset().y;
			this->m_pText->Write(this->m_vServerSlotButtons[i].GetText().c_str(), rect, this->m_vServerSlotButtons[i].GetTextColor(GUI::CButton::EButtonState::ON), this->m_vServerSlotButtons[i].GetTextScale());
		}
	}
#pragma endregion
#pragma region BROADCAST_DATA
	for (ulong i = 0; i < Game::kulMaxPlayers; ++i)
		this->m_vBroadcastNames[i].Render(this->m_pSprite, this->m_pText);
#pragma endregion
}
//Active Buttons
void CLobbyState::DetermineActiveButtons(void)
{
	//Set All to True
	for (ulong i = 0; i < NUM_BUTTON_GROUPS; i++)
		this->m_vButtonGroup[i].SetActive(true);

	//If any Server is selected, then all are off except for 'Join' & 'Back'
	if (this->m_slServerIndex != -1)
	{
		//Set all to false
		for (ulong i = 0; i < NUM_BUTTON_GROUPS; i++)
			this->m_vButtonGroup[i].SetActive(false);
		for (ulong i = 0; i < NUM_LIST_BUTS; i++)
			this->m_vServerListButtons[i].SetActive(true);
		this->m_vButtons[BACK].SetActive(true);
		this->m_cJoinButton.SetActive(true);
		//Set Client Data
		this->m_vServerList[this->m_slServerIndex].SetRead(0);
		for (ulong i = 0; i < Game::kulMaxPlayers; ++i)
		{
			this->m_vClientData[i].m_strName = this->m_vServerList[this->m_slServerIndex].ReadString();
			this->m_vClientData[i].m_eType = Game::EPlayerType(this->m_vServerList[this->m_slServerIndex].ReadByte());
		}
		this->m_cJoinButton.SetText("JOIN");
	}
	else
	{
		this->m_cJoinButton.SetText("No Lobby Selected");
		this->m_cJoinButton.SetActive(false);
		for (ulong i = 0; i < Game::kulMaxPlayers; ++i)
			this->m_vClientData[i] = Game::TClientData();
	}

	//Set Server List Buttons
	for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
	{
		this->m_vServerSlotButtons[i].SetToggle(i + this->m_slServerOffset == this->m_slServerIndex);
		this->m_vServerSlotButtons[i].SetActive(i + this->m_slServerOffset < this->m_vServerList.size());
	}
	//Change text for each button
	for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
	{
		slong index = i + this->m_slServerOffset;
		if (index >= slong(this->m_vServerList.size()))
			this->m_vServerSlotButtons[i].SetText("");
		else
			this->m_vServerSlotButtons[i].SetText(std::string("Lobby #") + std::to_string(i + this->m_slServerOffset));
	}

	//Set the Server Arrow Buttons
	this->m_vServerListButtons[INC].SetActive(!this->m_vServerList.empty());
	this->m_vServerListButtons[DEC].SetActive(!this->m_vServerList.empty());

	//Set Join Button Text
	if (this->m_vServerList.empty())
	{
		float scale = 0.7f;
		this->m_cJoinButton.SetActive(false);
		this->m_cJoinButton.SetTextScale(scale);
		this->m_cJoinButton.SetTextOffset(Type2<slong>(16, slong(scale * kslJoinOffset)));
		this->m_cJoinButton.SetText("NO LOCAL LOBBIES");
	}
	else if (this->m_slServerIndex == -1)
	{
		float scale = 0.7f;
		this->m_cJoinButton.SetActive(false);
		this->m_cJoinButton.SetTextScale(scale);
		this->m_cJoinButton.SetTextOffset(Type2<slong>(8, slong(scale * kslJoinOffset)));
		this->m_cJoinButton.SetText("NO LOBBY SELECTED");
	}
	else
	{
		bool open = false;
		for (ulong i = 0; i < Game::kulMaxPlayers; ++i)
		{
			if (this->m_vClientData[i].m_eType == Game::EPlayerType::OPEN)
			{
				open = true;
				break;
			}
		}
		if (open)
		{
			float scale = 1.0f;
			this->m_cJoinButton.SetActive(true);
			this->m_cJoinButton.SetTextScale(scale);
			this->m_cJoinButton.SetTextOffset(Type2<slong>(0, slong(scale * kslJoinOffset)));
			this->m_cJoinButton.SetText("JOIN");
		}
		else
		{
			float scale = 0.7f;
			this->m_cJoinButton.SetActive(false);
			this->m_cJoinButton.SetTextScale(scale);
			this->m_cJoinButton.SetTextOffset(Type2<slong>(0, slong(scale * kslJoinOffset)));
			this->m_cJoinButton.SetText("NO OPEN SLOTS");
		}
	}

	//Set the Slot Text
	for (ulong i = 0; i < Game::kulMaxPlayers; ++i)
	{
		std::string word;
		_COLOR<uchar> clr;
		switch (this->m_vClientData[i].m_eType)
		{
		case Game::EPlayerType::CLIENT:
			word = this->m_vClientData[i].m_strName;
			clr = CLR::GREY;
			break;
		case Game::EPlayerType::OPEN:
			word = "OPEN";
			clr = CLR::LAVENDER;
			break;
		case Game::EPlayerType::CLOSED:
			word = "CLOSED";
			clr = CLR::LAVENDER;
			break;
		case Game::EPlayerType::COM:
			word = "COM";
			clr = CLR::LAVENDER;
			break;
		}
		if (this->m_slServerIndex != -1)
		{
			this->m_vBroadcastNames[i].SetText(word.c_str());
			this->m_vBroadcastNames[i].SetTextColor(clr, GUI::CButton::EButtonState::INACTIVE);
		}
		else
			this->m_vBroadcastNames[i].SetText("");
	}
}
//Msg Recv
void CLobbyState::RecvNoSlots(void)
{
	this->m_cPUTimedState.SetGotoType(IGameState::EGameStateType::gs_LOBBY);
	this->m_cPUTimedState.SetTimer(1.0f);
	GUI::CButton cBut;
	cBut.SetRect(_RECT<slong>(0, slong(Sprite::SCREEN_HEIGHT * 0.5f) - 64, Sprite::SCREEN_WIDTH, 128));
	cBut.SetText("No Free Slots");
	cBut.SetTextScale(0.9f);
	cBut.SetTextColor(CLR::GREY, GUI::CButton::EButtonState::DEFAULT);
	this->m_cPUTimedState.SetHeader(cBut);
	this->m_tResult.Push(&this->m_cPUTimedState);
}

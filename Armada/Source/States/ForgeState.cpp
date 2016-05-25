/*
Author:			Daniel Habig
Date:			11-15-13
File:			IntroState.cpp
Purpose:		gamestate that allows selection of sub-states
*/
#include "ForgeState.h"
#include "../SDL/Sprite.h"
#include "../Text/Text.h"
#include "../Input/InputCMD.h"
#include "../Util/Collision.h"
#include "../Audio/Audio.h"
#include <algorithm>
#include <fstream>
using namespace Board;
using namespace Input;

const Time kdMarkerRate = 0.5l;

CForgeState::CForgeState(void)
{
	this->m_eType = EGameStateType::gs_FORGE;
	this->m_eSelectedSlot = ESlotButtons::NO_SBUT;
	this->m_eSelectedPlayer = EPlayerButtons::NO_PBUT;
	this->m_bMarkerVisible = false;
	this->m_dMarkerTimer = kdMarkerRate;
	this->m_bDefault = true;
	this->m_slDefaultOffset = 0;
	this->m_slDefaultIndex = -1;
	this->m_slCustomOffset = 0;
	this->m_slCustomIndex = -1;
	this->m_pStarField = NULL;
}
CForgeState::~CForgeState(void)
{
}

void CForgeState::Enter(Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText, Audio::CAudio* pAudio)
{
	IGameState::Enter(pSprite, pInput, pText, pAudio);

	//Build Buttons
	this->EnterPrimary();
	this->EnterSlots();
	this->EnterSpawns();
	this->EnterGrid();
	this->EnterMapName();
	this->EnterMapButs();
	this->EnterDefaultSlots();
	this->EnterDefaultButs();
	this->EnterCustomSlots();
	this->EnterCustomButs();

	//Build the Button  manager
	std::vector<GUI::CButtonGroup> vGroups;
	for (ulong i = 0; i < NUM_BUTTON_GROUPS; i++)
		vGroups.push_back(this->m_vButtonGroup[i]);
	this->m_cButMan.SetGroups(vGroups);

	//Initialize Buttons
	this->m_slDefaultIndex = 0;
	this->m_vDefaultSlotButtons[0].SetToggle(true);

	//Load Boards
	this->LoadCustom();
	this->LoadDefault();
	this->SortBoards();

	/* Old button setup */
	//
	//#pragma region PRIMARY
	//	//for (ulong i = 0; i < NUM_BUTTONS; i++)
	//	//{
	//	//	m_vButtons[i].DefaultColor(CLR::CLEAR);
	//	//	m_vButtons[i].SetTextColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
	//	//	m_vButtons[i].SetTextColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
	//	//	m_vButtons[i].SetTextColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
	//
	//	//	m_vButtons[i].SetSfx(sfxHover, GUI::CButton::EButtonSfx::SFX_HOVER);
	//	//	m_vButtons[i].SetSfx(sfxPush, GUI::CButton::EButtonSfx::SFX_PUSH);
	//	//	m_vButtons[i].SetSfx(sfxPush, GUI::CButton::EButtonSfx::SFX_PULL);
	//
	//	//	vButs.push_back(&m_vButtons[i]);
	//	//}
	//
	//	////Back Button
	//	//Type2<ulong> tSize;
	//	//tSize = this->m_pText->GetSize("Back", 1.0f);
	//	//m_vButtons[BACK].SetText("Back");
	//	//m_vButtons[BACK].SetRect(_RECT<slong>(Sprite::SCREEN_WIDTH - tSize.x - 32, 0, tSize.x, tSize.y));
	//	//m_vButtons[BACK].SetSfx(sfxBack, GUI::CButton::EButtonSfx::SFX_PULL);
	//
	//	////Group
	//	//this->m_vButtonGroup[PRIMARY].SetButtonList(vButs);
	//	//this->m_vButtonGroup[PRIMARY].SetMove(Type2<bool>(true, true));
	//	//this->m_vButtonGroup[PRIMARY].SetWrap(Type2<bool>(false, false));
	//	//this->m_vButtonGroup[PRIMARY].SetInputCMD(this->m_pInput);
	//	//this->m_vButtonGroup[PRIMARY].SetButtonGroupDir(LEFT, SPAWNS);
	//	//this->m_vButtonGroup[PRIMARY].SetFindClosestButtonGroup(true);
	//	//this->m_vButtonGroup[PRIMARY].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
	//	//vButs.clear();
	//#pragma endregion
	//#pragma region GRID
	//	//Alignment
	//	//for (ulong x = 0; x < Board::kslBoardSize; x++)
	//	//{
	//	//	for (ulong y = 0; y < Board::kslBoardSize; y++)
	//	//	{
	//	//		for (ulong j = 0; j < GUI::CButton::EButtonState::NUM_STATES; j++)
	//	//			this->m_vGrid[x][y].DefaultColor(CLR::CLEAR);
	//	//
	//	//		Type2<slong> panel_offset = offset + panel_size * Type2<slong>(x, y) + panel_space * Type2<slong>(x, y);
	//	//		_RECT<slong> rect = _RECT<slong>(panel_offset.x, panel_offset.y, panel_size.x, panel_size.y);
	//	//		this->m_vGrid[x][y].SetRect(rect);
	//	//
	//	//		this->m_vGrid[x][y].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
	//	//		this->m_vGrid[x][y].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
	//	//		this->m_vGrid[x][y].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
	//	//		this->m_vGrid[x][y].SetOutlineSize(6, GUI::CButton::EButtonState::HOVER);
	//	//		this->m_vGrid[x][y].SetOutlineSize(6, GUI::CButton::EButtonState::MDOWN);
	//	//
	//	//		if (y == 0)
	//	//			this->m_vGrid[x][y].SetButtonGroupDir(-2, DOWN);
	//	//
	//	//		this->m_vGrid[x][y].SetSfx(sfxGood, GUI::CButton::EButtonSfx::SFX_PULL);
	//	//
	//	//		vButs.push_back(&m_vGrid[x][y]);
	//	//	}
	//	//}
	//	//
	//	//this->m_vGrid[7][7].SetButtonGroupDir(MAPNAME, UP);
	//	//
	//	////Group
	//	//this->m_vButtonGroup[GRID].SetButtonList(vButs);
	//	//this->m_vButtonGroup[GRID].SetMove(Type2<bool>(true, true));
	//	//this->m_vButtonGroup[GRID].SetWrap(Type2<bool>(false, false));
	//	//this->m_vButtonGroup[GRID].SetInputCMD(this->m_pInput);
	//	//this->m_vButtonGroup[GRID].SetButtonGroupDir(RIGHT, SLOTS);
	//	//this->m_vButtonGroup[GRID].SetButtonGroupDir(UP, MAPNAME);
	//	//this->m_vButtonGroup[GRID].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
	//	//this->m_vButtonGroup[GRID].SetFindClosestButtonGroup(true);
	//	//vButs.clear();
	//#pragma endregion
	//#pragma region SLOT
	//	////Alignment
	//	//for (ulong i = 0; i < NUM_SLOT_BUTTONS; i++)
	//	//{
	//	//	for (ulong j = 0; j < GUI::CButton::EButtonState::NUM_STATES; j++)
	//	//		m_vSlotButtons[i].DefaultColor(CLR::CLEAR);
	//
	//	//	//Text
	//	//	this->m_vSlotButtons[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
	//	//	this->m_vSlotButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON);
	//	//	this->m_vSlotButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_HOVER);
	//	//	this->m_vSlotButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_MDOWN);
	//	//	this->m_vSlotButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF);
	//	//	this->m_vSlotButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_HOVER);
	//	//	this->m_vSlotButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_MDOWN);
	//	//	//Background
	//	//	this->m_vSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
	//	//	this->m_vSlotButtons[i].SetBGColor(CLR::SLATE, GUI::CButton::EButtonState::ON);
	//	//	this->m_vSlotButtons[i].SetBGColor(CLR::SLATE, GUI::CButton::EButtonState::ON_HOVER);
	//	//	this->m_vSlotButtons[i].SetBGColor(CLR::SLATE, GUI::CButton::EButtonState::ON_MDOWN);
	//	//	this->m_vSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF);
	//	//	this->m_vSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_HOVER);
	//	//	this->m_vSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_MDOWN);
	//	//	//Outline
	//	//	this->m_vSlotButtons[i].SetOutlineColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
	//	//	this->m_vSlotButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::ON);
	//	//	this->m_vSlotButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::ON_HOVER);
	//	//	this->m_vSlotButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::ON_MDOWN);
	//	//	this->m_vSlotButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::OFF);
	//	//	this->m_vSlotButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::OFF_HOVER);
	//	//	this->m_vSlotButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::OFF_MDOWN);
	//
	//	//	this->m_vSlotButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::ON_HOVER);
	//	//	this->m_vSlotButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::ON_MDOWN);
	//	//	this->m_vSlotButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::OFF_HOVER);
	//	//	this->m_vSlotButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::OFF_MDOWN);
	//
	//	//	this->m_vSlotButtons[i].SetSfx(sfxGood, GUI::CButton::EButtonSfx::SFX_PULL);
	//
	//	//	Type2<slong> index = Type2<slong>(i % 3, i / 3);
	//	//	Type2<ulong> size = this->m_pText->GetSize("0", 1.0f);
	//	//	_RECT<slong> rect = this->m_vGrid[0][0].GetRect();
	//	//	Type2<slong> dim = Type2<slong>(rect.w + panel_space.w, rect.h + panel_space.h);
	//	//	rect.x = offset.x + dim.w * kslBoardSize + dim.w + index.x * dim.w;
	//	//	rect.y = offset.y + dim.h * 4 - index.y * dim.h;
	//	//	this->m_vSlotButtons[i].SetRect(rect);
	//	//	this->m_vSlotButtons[i].SetText(std::to_string(i));
	//	//	this->m_vSlotButtons[i].SetTextOffset(Type2<slong>(0, -(slong)(size.h * 0.22f)));
	//	//	vButs.push_back(&this->m_vSlotButtons[i]);
	//	//}
	//	////X Button
	//	//this->m_vSlotButtons[OFF].SetText("X");
	//
	//
	//	////Group
	//	//this->m_vButtonGroup[SLOTS].SetButtonList(vButs);
	//	//this->m_vButtonGroup[SLOTS].SetMove(Type2<bool>(true, true));
	//	//this->m_vButtonGroup[SLOTS].SetWrap(Type2<bool>(false, false));
	//	//this->m_vButtonGroup[SLOTS].SetInputCMD(this->m_pInput);
	//	//this->m_vButtonGroup[SLOTS].SetButtonGroupDir(RIGHT, CUSTOM_SLOTS);
	//	//this->m_vButtonGroup[SLOTS].SetButtonGroupDir(LEFT, GRID);
	//	//this->m_vButtonGroup[SLOTS].SetButtonGroupDir(DOWN, SPAWNS);
	//	//this->m_vButtonGroup[SLOTS].SetButtonGroupDir(UP, MAPBUTS);
	//	//this->m_vButtonGroup[SLOTS].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
	//	//vButs.clear();
	//#pragma endregion
	//#pragma region SPAWNS
	//	////Alignment
	//	//for (ulong i = 0; i < NUM_PLAYER_BUTTONS; i++)
	//	//{
	//	//	for (ulong j = 0; j < GUI::CButton::EButtonState::NUM_STATES; j++)
	//	//		this->m_vPlayerButtons[i].DefaultColor(CLR::CLEAR);
	//
	//	//	//Text
	//	//	this->m_vPlayerButtons[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
	//	//	this->m_vPlayerButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON);
	//	//	this->m_vPlayerButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_HOVER);
	//	//	this->m_vPlayerButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_MDOWN);
	//	//	this->m_vPlayerButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF);
	//	//	this->m_vPlayerButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_HOVER);
	//	//	this->m_vPlayerButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_MDOWN);
	//	//	//Background
	//	//	this->m_vPlayerButtons[i].SetBGColor(kutPlayerColors[i] * 0.25f, GUI::CButton::EButtonState::INACTIVE);
	//	//	this->m_vPlayerButtons[i].SetBGColor(kutPlayerColors[i] * 1.00f, GUI::CButton::EButtonState::ON);
	//	//	this->m_vPlayerButtons[i].SetBGColor(kutPlayerColors[i] * 0.75f, GUI::CButton::EButtonState::ON_HOVER);
	//	//	this->m_vPlayerButtons[i].SetBGColor(kutPlayerColors[i] * 0.50f, GUI::CButton::EButtonState::ON_MDOWN);
	//	//	this->m_vPlayerButtons[i].SetBGColor(kutPlayerColors[i] * 1.00f, GUI::CButton::EButtonState::OFF);
	//	//	this->m_vPlayerButtons[i].SetBGColor(kutPlayerColors[i] * 0.75f, GUI::CButton::EButtonState::OFF_HOVER);
	//	//	this->m_vPlayerButtons[i].SetBGColor(kutPlayerColors[i] * 0.50f, GUI::CButton::EButtonState::OFF_MDOWN);
	//	//	//Outline
	//	//	this->m_vPlayerButtons[i].SetOutlineColor(CLR::CLEAR, GUI::CButton::EButtonState::INACTIVE);
	//	//	this->m_vPlayerButtons[i].SetOutlineColor(CLR::DARK_GREY, GUI::CButton::EButtonState::ON);
	//	//	this->m_vPlayerButtons[i].SetOutlineColor(CLR::GREY, GUI::CButton::EButtonState::ON_HOVER);
	//	//	this->m_vPlayerButtons[i].SetOutlineColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_MDOWN);
	//	//	this->m_vPlayerButtons[i].SetOutlineColor(CLR::DARK_GREY, GUI::CButton::EButtonState::OFF);
	//	//	this->m_vPlayerButtons[i].SetOutlineColor(CLR::GREY, GUI::CButton::EButtonState::OFF_HOVER);
	//	//	this->m_vPlayerButtons[i].SetOutlineColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::OFF_MDOWN);
	//
	//	//	this->m_vPlayerButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::ON_HOVER);
	//	//	this->m_vPlayerButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::ON_MDOWN);
	//	//	this->m_vPlayerButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::OFF_HOVER);
	//	//	this->m_vPlayerButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::OFF_MDOWN);
	//
	//	//	this->m_vPlayerButtons[i].SetSfx(sfxGood, GUI::CButton::EButtonSfx::SFX_PULL);
	//
	//	//	Type2<slong> index = Type2<slong>(i % 2, i / 2);
	//	//	Type2<ulong> size = this->m_pText->GetSize("0", 1.0f);
	//	//	_RECT<slong> rect = this->m_vGrid[0][0].GetRect();
	//	//	Type2<slong> dim = Type2<slong>(rect.w + panel_space.w, rect.h + panel_space.h);
	//	//	rect.x = offset.x + slong(dim.w * 0.5f) + dim.w * kslBoardSize + dim.w + index.x * dim.w;
	//	//	rect.y = offset.y + dim.h - index.y * dim.h;
	//	//	this->m_vPlayerButtons[i].SetRect(rect);
	//	//	//this->m_vPlayerButtons[i].SetText(std::string(std::string("P") + std::to_string(i + 1)).c_str());
	//	//	//this->m_vPlayerButtons[i].SetTextScale(0.5f);
	//	//	//this->m_vPlayerButtons[i].SetOffset(Type2<slong>(0, -(slong)(size.h * 0.22f * 0.5f)));
	//	//	vButs.push_back(&this->m_vPlayerButtons[i]);
	//	//}
	//
	//	////Group
	//	//this->m_vButtonGroup[SPAWNS].SetButtonList(vButs);
	//	//this->m_vButtonGroup[SPAWNS].SetMove(Type2<bool>(true, true));
	//	//this->m_vButtonGroup[SPAWNS].SetWrap(Type2<bool>(false, false));
	//	//this->m_vButtonGroup[SPAWNS].SetInputCMD(this->m_pInput);
	//	//this->m_vButtonGroup[SPAWNS].SetButtonGroupDir(RIGHT, PRIMARY);
	//	//this->m_vButtonGroup[SPAWNS].SetButtonGroupDir(LEFT, GRID);
	//	//this->m_vButtonGroup[SPAWNS].SetButtonGroupDir(UP, SLOTS);
	//	//this->m_vButtonGroup[SPAWNS].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
	//	//vButs.clear();
	//#pragma endregion
	//#pragma region MAPNAME
	//	////Alignment
	//	//{
	//	//	this->m_cMapNameButton.DefaultColor(CLR::CLEAR);
	//
	//	//	//Text
	//	//	this->m_cMapNameButton.SetTextColor(CLR::GREY, GUI::CButton::EButtonState::INACTIVE);
	//	//	this->m_cMapNameButton.SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON);
	//	//	this->m_cMapNameButton.SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_HOVER);
	//	//	this->m_cMapNameButton.SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_MDOWN);
	//	//	this->m_cMapNameButton.SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF);
	//	//	this->m_cMapNameButton.SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_HOVER);
	//	//	this->m_cMapNameButton.SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_MDOWN);
	//	//	//Background
	//	//	this->m_cMapNameButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
	//	//	this->m_cMapNameButton.SetBGColor(CLR::GREY, GUI::CButton::EButtonState::ON);
	//	//	this->m_cMapNameButton.SetBGColor(CLR::GREY, GUI::CButton::EButtonState::ON_HOVER);
	//	//	this->m_cMapNameButton.SetBGColor(CLR::GREY, GUI::CButton::EButtonState::ON_MDOWN);
	//	//	this->m_cMapNameButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF);
	//	//	this->m_cMapNameButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_HOVER);
	//	//	this->m_cMapNameButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_MDOWN);
	//	//	//Outline
	//	//	this->m_cMapNameButton.SetOutlineColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
	//	//	this->m_cMapNameButton.SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::ON);
	//	//	this->m_cMapNameButton.SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::ON_HOVER);
	//	//	this->m_cMapNameButton.SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::ON_MDOWN);
	//	//	this->m_cMapNameButton.SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::OFF);
	//	//	this->m_cMapNameButton.SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::OFF_HOVER);
	//	//	this->m_cMapNameButton.SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::OFF_MDOWN);
	//
	//	//	this->m_cMapNameButton.SetOutlineSize(6, GUI::CButton::EButtonState::ON_HOVER);
	//	//	this->m_cMapNameButton.SetOutlineSize(6, GUI::CButton::EButtonState::ON_MDOWN);
	//	//	this->m_cMapNameButton.SetOutlineSize(6, GUI::CButton::EButtonState::OFF_HOVER);
	//	//	this->m_cMapNameButton.SetOutlineSize(6, GUI::CButton::EButtonState::OFF_MDOWN);
	//
	//	//	this->m_cMapNameButton.SetSfx(sfxGood, GUI::CButton::EButtonSfx::SFX_PULL);
	//
	//	//	_RECT<slong> rect = this->m_vGrid[3][7].GetRect();
	//	//	rect.y += 112;
	//	//	//rect.x += this->m_pText->GetSize("Map Name:", 0.5f).w + panel_space.w;
	//	//	rect.w = this->m_vGrid[7][7].GetRect().GetPos().x + panel_size.w - rect.x;
	//	//	this->m_cMapNameButton.SetRect(rect);
	//	//	this->m_cMapNameButton.SetTextScale(0.56f);
	//	//	this->m_cMapNameButton.SetTextOffset(Type2<slong>(12, 0));
	//	//	this->m_cMapNameButton.SetTextAlignment(Text::EFontAlignment::F_LEFT);
	//	//	vButs.push_back(&this->m_cMapNameButton);
	//	//}
	//
	//	////Group
	//	//this->m_vButtonGroup[MAPNAME].SetButtonList(vButs);
	//	//this->m_vButtonGroup[MAPNAME].SetMove(Type2<bool>(true, true));
	//	//this->m_vButtonGroup[MAPNAME].SetWrap(Type2<bool>(false, false));
	//	//this->m_vButtonGroup[MAPNAME].SetInputCMD(this->m_pInput);
	//	//this->m_vButtonGroup[MAPNAME].SetButtonGroupDir(DOWN, GRID);
	//	//this->m_vButtonGroup[MAPNAME].SetButtonGroupDir(RIGHT, MAPBUTS);
	//	//this->m_vButtonGroup[MAPNAME].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
	//	//vButs.clear();
	//#pragma endregion
	//#pragma region MAPBUTS
	//	////Alignment
	//	//for (ulong i = 0; i < NUM_MAP_BUTTONS; i++)
	//	//{
	//	//	for (ulong j = 0; j < GUI::CButton::EButtonState::NUM_STATES; j++)
	//	//		m_vMapButtons[i].DefaultColor(CLR::CLEAR);
	//
	//	//	//Text
	//	//	this->m_vMapButtons[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
	//	//	this->m_vMapButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::DEFAULT);
	//	//	this->m_vMapButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::HOVER);
	//	//	this->m_vMapButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::MDOWN);
	//	//	//Background
	//	//	this->m_vMapButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
	//	//	this->m_vMapButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::DEFAULT);
	//	//	this->m_vMapButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::HOVER);
	//	//	this->m_vMapButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::MDOWN);
	//	//	//Outline
	//	//	this->m_vMapButtons[i].SetOutlineColor(CLR::CLEAR, GUI::CButton::EButtonState::INACTIVE);
	//	//	this->m_vMapButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
	//	//	this->m_vMapButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
	//	//	this->m_vMapButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
	//
	//	//	this->m_vMapButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::HOVER);
	//	//	this->m_vMapButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::MDOWN);
	//
	//	//	this->m_vMapButtons[i].SetTextAlignment(Text::EFontAlignment::F_CENTER);
	//
	//	//	this->m_vMapButtons[i].SetSfx(sfxGood, GUI::CButton::EButtonSfx::SFX_PULL);
	//
	//	//	Type2<ulong> size = this->m_pText->GetSize("0", 0.7f);
	//	//	_RECT<slong> rect = GRID::kGrid[14][1];
	//	//	rect.y += (NUM_MAP_BUTTONS - i - 1) * (GRID::kSize.h + GRID::kSpace.h);
	//	//	rect.w = 3 * (GRID::kSize.w + GRID::kSpace.w) - GRID::kSpace.w;
	//	//	// this->m_vGrid[0][0].GetRect();
	//	//	//Type2<slong> dim = Type2<slong>(rect.w + panel_space.w, rect.h + panel_space.h);
	//	//	//rect.x = offset.x + dim.w * kslBoardSize + dim.w;
	//	//	//rect.y = offset.y + dim.h * kslBoardSize - i * dim.h;
	//	//	//rect.w = dim.w * 3 - panel_space.w;
	//	//	this->m_vMapButtons[i].SetRect(rect);
	//	//	this->m_vMapButtons[i].SetTextOffset(Type2<slong>(0, -(slong)(size.h * 0.12f)));
	//	//	this->m_vMapButtons[i].SetTextScale(0.7f);
	//	//	vButs.push_back(&this->m_vMapButtons[i]);
	//	//}
	//
	//	////New Button
	//	//Type2<ulong> tSize = this->m_pText->GetSize("NEW", 1.0f);
	//	//m_vMapButtons[NEW].SetText("NEW");
	//	//m_vMapButtons[NEW].SetButtonGroupDir(MAPNAME, LEFT);
	//	//m_vMapButtons[NEW].SetButtonGroupDir(DEFAULT_SLOTS, RIGHT);
	//	////Copy Button
	//	//tSize = this->m_pText->GetSize("COPY", 1.0f);
	//	//m_vMapButtons[COPY].SetText("COPY");
	//	//m_vMapButtons[COPY].SetButtonGroupDir(DEFAULT_SLOTS, RIGHT);
	//	////Del Button
	//	//tSize = this->m_pText->GetSize("DELETE", 1.0f);
	//	//m_vMapButtons[DEL].SetText("DELETE");
	//	//m_vMapButtons[DEL].SetButtonGroupDir(DEFAULT_SLOTS, RIGHT);
	//	//Type2<slong> textOffset = this->m_vMapButtons[DEL].GetTextOffset();
	//	//textOffset.x = 4;
	//	//m_vMapButtons[DEL].SetTextOffset(textOffset);
	//
	//	////Group
	//	//this->m_vButtonGroup[MAPBUTS].SetButtonList(vButs);
	//	//this->m_vButtonGroup[MAPBUTS].SetMove(Type2<bool>(true, true));
	//	//this->m_vButtonGroup[MAPBUTS].SetWrap(Type2<bool>(false, false));
	//	//this->m_vButtonGroup[MAPBUTS].SetInputCMD(this->m_pInput);
	//	//this->m_vButtonGroup[MAPBUTS].SetButtonGroupDir(LEFT, GRID);
	//	//this->m_vButtonGroup[MAPBUTS].SetButtonGroupDir(RIGHT, DEFAULT_SLOTS);
	//	//this->m_vButtonGroup[MAPBUTS].SetButtonGroupDir(DOWN, SLOTS);
	//	//this->m_vButtonGroup[MAPBUTS].SetFindClosestButtonGroup(true);
	//	//this->m_vButtonGroup[MAPBUTS].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
	//	//vButs.clear();
	//#pragma endregion
	//#pragma region DEFAULT_BUTS
	//	//for (ulong i = 0; i < NUM_LIST_BUTS; i++)
	//	//{
	//	//	for (ulong j = 0; j < GUI::CButton::EButtonState::NUM_STATES; j++)
	//	//		m_vDefaultListButtons[i].DefaultColor(CLR::CLEAR);
	//	//	//Text
	//	//	this->m_vDefaultListButtons[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
	//	//	//Background
	//	//	this->m_vDefaultListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
	//	//	this->m_vDefaultListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::DEFAULT);
	//	//	this->m_vDefaultListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::HOVER);
	//	//	this->m_vDefaultListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::MDOWN);
	//	//	//Outline
	//	//	this->m_vDefaultListButtons[i].SetOutlineColor(CLR::CLEAR, GUI::CButton::EButtonState::INACTIVE);
	//	//	this->m_vDefaultListButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
	//	//	this->m_vDefaultListButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
	//	//	this->m_vDefaultListButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
	//	//	//Outline Size
	//	//	this->m_vDefaultListButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::HOVER);
	//	//	this->m_vDefaultListButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::MDOWN);
	//	//	//Alignment
	//	//	this->m_vDefaultListButtons[i].SetTextAlignment(Text::EFontAlignment::F_CENTER);
	//	//	//Rect
	//	//	Type2<ulong> size = this->m_pText->GetSize("0", 0.7f);
	//	//	_RECT<slong> rect = GRID::kGrid[11][6];
	//	//	rect.y += (NUM_LIST_BUTS - i - 1) * (GRID::kSize.h + GRID::kSpace.h);
	//	//	rect.x += slong((GRID::kSize.w + GRID::kSpace.w) * 0.5f);
	//
	//	//	//this->m_vGrid[7][7].GetRect();
	//	//	//Type2<slong> dim = Type2<slong>(rect.w + panel_space.w, rect.h + panel_space.h);
	//	//	//rect.x = offset.x + dim.w * kslBoardSize + dim.w * 8;
	//	//	//rect.y = offset.y + dim.h * kslBoardSize - (i + 1) * dim.h;
	//	//	this->m_vDefaultListButtons[i].SetRect(rect);
	//	//	this->m_vDefaultListButtons[i].SetTextOffset(Type2<slong>(0, -(slong)(size.h * 0.12f)));
	//	//	this->m_vDefaultListButtons[i].SetTextScale(0.7f);
	//	//	vButs.push_back(&this->m_vDefaultListButtons[i]);
	//	//}
	//
	//	////Group
	//	//this->m_vButtonGroup[DEFAULT_BUTS].SetButtonList(vButs);
	//	//this->m_vButtonGroup[DEFAULT_BUTS].SetMove(Type2<bool>(true, true));
	//	//this->m_vButtonGroup[DEFAULT_BUTS].SetWrap(Type2<bool>(false, false));
	//	//this->m_vButtonGroup[DEFAULT_BUTS].SetInputCMD(this->m_pInput);
	//	//this->m_vButtonGroup[DEFAULT_BUTS].SetButtonGroupDir(LEFT, DEFAULT_SLOTS);
	//	//this->m_vButtonGroup[DEFAULT_BUTS].SetButtonGroupDir(DOWN, CUSTOM_BUTS);
	//	////this->m_vButtonGroup[DEFAULT_BUTS].SetFindClosestButtonGroup(true);
	//	//this->m_vButtonGroup[DEFAULT_BUTS].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
	//	//vButs.clear();
	//#pragma endregion
	//#pragma region DEFAULT_SLOTS
	//	//for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
	//	//{
	//	//	for (ulong j = 0; j < GUI::CButton::EButtonState::NUM_STATES; j++)
	//	//		m_vDefaultSlotButtons[i].DefaultColor(CLR::CLEAR);
	//
	//	//	//Text
	//	//	this->m_vDefaultSlotButtons[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
	//	//	this->m_vDefaultSlotButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON);
	//	//	this->m_vDefaultSlotButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_HOVER);
	//	//	this->m_vDefaultSlotButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_MDOWN);
	//	//	this->m_vDefaultSlotButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF);
	//	//	this->m_vDefaultSlotButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_HOVER);
	//	//	this->m_vDefaultSlotButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_MDOWN);
	//	//	//Background
	//	//	this->m_vDefaultSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
	//	//	this->m_vDefaultSlotButtons[i].SetBGColor(CLR::SLATE, GUI::CButton::EButtonState::ON);
	//	//	this->m_vDefaultSlotButtons[i].SetBGColor(CLR::SLATE, GUI::CButton::EButtonState::ON_HOVER);
	//	//	this->m_vDefaultSlotButtons[i].SetBGColor(CLR::SLATE, GUI::CButton::EButtonState::ON_MDOWN);
	//	//	this->m_vDefaultSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF);
	//	//	this->m_vDefaultSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_HOVER);
	//	//	this->m_vDefaultSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_MDOWN);
	//	//	//Outline
	//	//	this->m_vDefaultSlotButtons[i].SetOutlineColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
	//	//	this->m_vDefaultSlotButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::ON);
	//	//	this->m_vDefaultSlotButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::ON_HOVER);
	//	//	this->m_vDefaultSlotButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::ON_MDOWN);
	//	//	this->m_vDefaultSlotButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::OFF);
	//	//	this->m_vDefaultSlotButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::OFF_HOVER);
	//	//	this->m_vDefaultSlotButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::OFF_MDOWN);
	//	//	//Outline Size
	//	//	this->m_vDefaultSlotButtons[i].SetOutlineSize(2, GUI::CButton::EButtonState::ON);
	//	//	this->m_vDefaultSlotButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::ON_HOVER);
	//	//	this->m_vDefaultSlotButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::ON_MDOWN);
	//	//	this->m_vDefaultSlotButtons[i].SetOutlineSize(2, GUI::CButton::EButtonState::OFF);
	//	//	this->m_vDefaultSlotButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::OFF_HOVER);
	//	//	this->m_vDefaultSlotButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::OFF_MDOWN);
	//	//	//Alignment
	//	//	this->m_vDefaultSlotButtons[i].SetTextAlignment(Text::EFontAlignment::F_LEFT);
	//	//	//Rect
	//	//	Type2<ulong> size = this->m_pText->GetSize("0", 0.7f);
	//	//	_RECT<slong> rect = GRID::kGrid[8][6];
	//	//	rect.x += slong((GRID::kSize.w + GRID::kSpace.w) * 0.5f);
	//	//	rect.y += (NUM_LIST_SLOTS - i - 1) * 52;
	//	//	rect.w = 3 * (GRID::kSize.w + GRID::kSpace.w) - GRID::kSpace.w;
	//	//	rect.h = 48;
	//	//	//this->m_vGrid[7][7].GetRect();
	//	//	//Type2<slong> dim = Type2<slong>(rect.w + panel_space.w, rect.h + panel_space.h);
	//	//	//rect.x = offset.x + dim.w * kslBoardSize + dim.w * 5;
	//	//	//rect.y = offset.y + dim.h * kslBoardSize - panel_space.h - (i + 1) * 52;
	//	//	//rect.w = dim.w * 3 - 16;
	//	//	//rect.h = 48;
	//	//	this->m_vDefaultSlotButtons[i].SetRect(rect);
	//	//	Type2<slong> text_offset = this->m_cMapNameButton.GetTextOffset();
	//	//	text_offset.x = slong(text_offset.x * 0.588235f);
	//	//	text_offset.y = slong(text_offset.y * 0.588235f);
	//	//	this->m_vDefaultSlotButtons[i].SetTextOffset(text_offset);
	//	//	this->m_vDefaultSlotButtons[i].SetTextScale(this->m_cMapNameButton.GetTextScale() * 0.588235f);
	//	//	this->m_vDefaultSlotButtons[i].SetButtonGroupDir(MAPBUTS, LEFT);
	//	//	vButs.push_back(&this->m_vDefaultSlotButtons[i]);
	//	//}
	//
	//	//this->m_vDefaultSlotButtons[FIRST].SetButtonGroupDir(-2, UP);
	//
	//	////Group
	//	//this->m_vButtonGroup[DEFAULT_SLOTS].SetButtonList(vButs);
	//	//this->m_vButtonGroup[DEFAULT_SLOTS].SetMove(Type2<bool>(true, true));
	//	//this->m_vButtonGroup[DEFAULT_SLOTS].SetWrap(Type2<bool>(false, false));
	//	//this->m_vButtonGroup[DEFAULT_SLOTS].SetInputCMD(this->m_pInput);
	//	//this->m_vButtonGroup[DEFAULT_SLOTS].SetButtonGroupDir(LEFT, MAPBUTS);
	//	//this->m_vButtonGroup[DEFAULT_SLOTS].SetButtonGroupDir(RIGHT, DEFAULT_BUTS);
	//	//this->m_vButtonGroup[DEFAULT_SLOTS].SetButtonGroupDir(DOWN, PRIMARY);
	//	//this->m_vButtonGroup[DEFAULT_SLOTS].SetFindClosestButtonGroup(true);
	//	//this->m_vButtonGroup[DEFAULT_SLOTS].SetMoveType(GUI::CButton::EMoveType::PREVIOUS);
	//	//vButs.clear();
	//#pragma endregion
	//#pragma region CUSTOM_BUTS
	//	//for (ulong i = 0; i < NUM_LIST_BUTS; i++)
	//	//{
	//	//	for (ulong j = 0; j < GUI::CButton::EButtonState::NUM_STATES; j++)
	//	//		m_vCustomListButtons[i].DefaultColor(CLR::CLEAR);
	//
	//	//	//Background
	//	//	this->m_vCustomListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
	//	//	this->m_vCustomListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::DEFAULT);
	//	//	this->m_vCustomListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::HOVER);
	//	//	this->m_vCustomListButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::MDOWN);
	//	//	//Outline
	//	//	this->m_vCustomListButtons[i].SetOutlineColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
	//	//	this->m_vCustomListButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
	//	//	this->m_vCustomListButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
	//	//	this->m_vCustomListButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
	//	//	//Outline Size
	//	//	this->m_vCustomListButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::HOVER);
	//	//	this->m_vCustomListButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::MDOWN);
	//	//	//Alignment
	//	//	this->m_vCustomListButtons[i].SetTextAlignment(Text::EFontAlignment::F_CENTER);
	//	//	//Rect
	//	//	Type2<ulong> size = this->m_pText->GetSize("0", 0.7f);
	//	//	//_RECT<slong> rect = this->m_vGrid[7][7].GetRect();
	//	//	//Type2<slong> dim = Type2<slong>(rect.w + panel_space.w, rect.h + panel_space.h);
	//	//	//rect.x = offset.x + dim.w * kslBoardSize + dim.w * 8;
	//	//	//rect.y = offset.y + dim.h * kslBoardSize - (i + 4) * dim.h;
	//	//	_RECT<slong> rect = GRID::kGrid[16][6];
	//	//	rect.y += (NUM_LIST_BUTS - i - 1) * (GRID::kSize.h + GRID::kSpace.h);
	//	//	this->m_vCustomListButtons[i].SetRect(rect);
	//	//	this->m_vCustomListButtons[i].SetTextOffset(Type2<slong>(0, -(slong)(size.h * 0.12f)));
	//	//	this->m_vCustomListButtons[i].SetTextScale(0.7f);
	//	//	vButs.push_back(&this->m_vCustomListButtons[i]);
	//	//}
	//
	//	////Group
	//	//this->m_vButtonGroup[CUSTOM_BUTS].SetButtonList(vButs);
	//	//this->m_vButtonGroup[CUSTOM_BUTS].SetMove(Type2<bool>(true, true));
	//	//this->m_vButtonGroup[CUSTOM_BUTS].SetWrap(Type2<bool>(false, false));
	//	//this->m_vButtonGroup[CUSTOM_BUTS].SetInputCMD(this->m_pInput);
	//	//this->m_vButtonGroup[CUSTOM_BUTS].SetButtonGroupDir(UP, DEFAULT_BUTS);
	//	//this->m_vButtonGroup[CUSTOM_BUTS].SetButtonGroupDir(DOWN, PRIMARY);
	//	//this->m_vButtonGroup[CUSTOM_BUTS].SetButtonGroupDir(LEFT, CUSTOM_SLOTS);
	//	//this->m_vButtonGroup[CUSTOM_BUTS].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
	//	//vButs.clear();
	//#pragma endregion
	//#pragma region CUSTOM_SLOTS
	//	//for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
	//	//{
	//	//	for (ulong j = 0; j < GUI::CButton::EButtonState::NUM_STATES; j++)
	//	//		m_vCustomSlotButtons[i].DefaultColor(CLR::CLEAR);
	//
	//	//	//Text
	//	//	this->m_vCustomSlotButtons[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
	//	//	this->m_vCustomSlotButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON);
	//	//	this->m_vCustomSlotButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_HOVER);
	//	//	this->m_vCustomSlotButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_MDOWN);
	//	//	this->m_vCustomSlotButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF);
	//	//	this->m_vCustomSlotButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_HOVER);
	//	//	this->m_vCustomSlotButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_MDOWN);
	//	//	//Background
	//	//	this->m_vCustomSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
	//	//	this->m_vCustomSlotButtons[i].SetBGColor(CLR::SLATE, GUI::CButton::EButtonState::ON);
	//	//	this->m_vCustomSlotButtons[i].SetBGColor(CLR::SLATE, GUI::CButton::EButtonState::ON_HOVER);
	//	//	this->m_vCustomSlotButtons[i].SetBGColor(CLR::SLATE, GUI::CButton::EButtonState::ON_MDOWN);
	//	//	this->m_vCustomSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF);
	//	//	this->m_vCustomSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_HOVER);
	//	//	this->m_vCustomSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_MDOWN);
	//	//	//Outline
	//	//	this->m_vCustomSlotButtons[i].SetOutlineColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
	//	//	this->m_vCustomSlotButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::ON);
	//	//	this->m_vCustomSlotButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::ON_HOVER);
	//	//	this->m_vCustomSlotButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::ON_MDOWN);
	//	//	this->m_vCustomSlotButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::OFF);
	//	//	this->m_vCustomSlotButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::OFF_HOVER);
	//	//	this->m_vCustomSlotButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::OFF_MDOWN);
	//	//	//Outline Size
	//	//	this->m_vCustomSlotButtons[i].SetOutlineSize(2, GUI::CButton::EButtonState::ON);
	//	//	this->m_vCustomSlotButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::ON_HOVER);
	//	//	this->m_vCustomSlotButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::ON_MDOWN);
	//	//	this->m_vCustomSlotButtons[i].SetOutlineSize(2, GUI::CButton::EButtonState::OFF);
	//	//	this->m_vCustomSlotButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::OFF_HOVER);
	//	//	this->m_vCustomSlotButtons[i].SetOutlineSize(4, GUI::CButton::EButtonState::OFF_MDOWN);
	//	//	//Alignment
	//	//	this->m_vCustomSlotButtons[i].SetTextAlignment(Text::EFontAlignment::F_LEFT);
	//	//	//Rect
	//	//	Type2<ulong> size = this->m_pText->GetSize("0", 0.7f);
	//	//	//_RECT<slong> rect = this->m_vGrid[7][7].GetRect();
	//	//	//Type2<slong> dim = Type2<slong>(rect.w + panel_space.w, rect.h + panel_space.h);
	//	//	//rect.x = offset.x + dim.w * kslBoardSize + dim.w * 5;
	//	//	//rect.y = offset.y + dim.h * kslBoardSize - dim.h * 3 - panel_space.h - (i + 1) * 52;
	//	//	//rect.w = dim.w * 3 - 16;
	//	//	//rect.h = 48;
	//	//	_RECT<slong> rect = GRID::kGrid[13][6];
	//	//	rect.y += (NUM_LIST_SLOTS - i - 1) * 52;
	//	//	rect.w = 3 * (GRID::kSize.w + GRID::kSpace.w) - GRID::kSpace.w;
	//	//	rect.h = 48;
	//	//	this->m_vCustomSlotButtons[i].SetRect(rect);
	//	//	Type2<slong>text_offset = this->m_cMapNameButton.GetTextOffset();
	//	//	text_offset.x = slong(text_offset.x * 0.588235f);
	//	//	text_offset.y = slong(text_offset.y * 0.588235f);
	//	//	this->m_vCustomSlotButtons[i].SetTextOffset(text_offset);
	//	//	this->m_vCustomSlotButtons[i].SetTextScale(this->m_cMapNameButton.GetTextScale() * 0.588235f);
	//	//	vButs.push_back(&this->m_vCustomSlotButtons[i]);
	//	//}
	//
	//	////Group
	//	//this->m_vButtonGroup[CUSTOM_SLOTS].SetButtonList(vButs);
	//	//this->m_vButtonGroup[CUSTOM_SLOTS].SetMove(Type2<bool>(true, true));
	//	//this->m_vButtonGroup[CUSTOM_SLOTS].SetWrap(Type2<bool>(false, false));
	//	//this->m_vButtonGroup[CUSTOM_SLOTS].SetInputCMD(this->m_pInput);
	//	//this->m_vButtonGroup[CUSTOM_SLOTS].SetButtonGroupDir(UP, DEFAULT_SLOTS);
	//	//this->m_vButtonGroup[CUSTOM_SLOTS].SetButtonGroupDir(DOWN, PRIMARY);
	//	//this->m_vButtonGroup[CUSTOM_SLOTS].SetButtonGroupDir(LEFT, SLOTS);
	//	//this->m_vButtonGroup[CUSTOM_SLOTS].SetButtonGroupDir(RIGHT, CUSTOM_BUTS);
	//	//this->m_vButtonGroup[CUSTOM_SLOTS].SetFindClosestButtonGroup(true);
	//	//this->m_vButtonGroup[CUSTOM_SLOTS].SetMoveType(GUI::CButton::EMoveType::PREVIOUS);
	//	//vButs.clear();
	//#pragma endregion
}
void CForgeState::Exit(void)
{
	IGameState::Exit();
	this->SaveCutsom();
	this->SaveDefault();
}

void CForgeState::Input(void)
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
			confirm = this->m_pInput->Key_Held(VK_RETURN) || (this->m_pInput->Key_Held(VK_SPACE) && !this->m_cMapNameButton.GetToggle()) || this->m_pInput->Ctrl_Held(CTRL_KEYS::XB_A) || this->m_pInput->Ctrl_Held(CTRL_KEYS::START);

		for (ulong i = 0; i < NUM_BUTTON_GROUPS; i++)
			this->m_vButtonGroup[i].Confirmation(confirm);

		//Special Case MapNameButton
		if (this->m_cMapNameButton.GetToggle() && eType == Input::eInputType::MOUSE && this->m_pInput->Key_Push(VK_RETURN))
		{
			this->m_cMapNameButton.Selection(true);
			this->m_cMapNameButton.Confirmation(true);
			this->m_cMapNameButton.Selection(true);
			this->m_cMapNameButton.Confirmation(false);
		}
	}
	//Results
	{
		InputPrimary();
		InputSlots();
		InputSpawns();
		InputGrid();
		InputMapName();
		InputMapButs();
		InputDefaultSlots();
		InputDefaultButs();
		InputCustomSlots();
		InputCustomButs();
		InputSpecialCase_Grid();
	}
}
void CForgeState::Update(Time dDelta, Time gameTime)
{
	this->m_cButMan.Update(dDelta, this->m_pAudio);
	this->m_pStarField->Update(dDelta);
	this->m_cBoard.Update(dDelta);


	//Map Name Special Case - text clamping
	if (this->m_cMapNameButton.GetToggle())
	{
		//Check the size of the text fits in the rect
		_RECT<slong> tRect = this->m_cMapNameButton.GetRect();
		Type2<ulong> tSize = this->m_pText->GetSize((this->m_pInput->Text_Get_String() + std::string("_")).c_str(), this->m_cMapNameButton.GetTextScale());
		if (slong(tSize.w) > tRect.w - this->m_cMapNameButton.GetTextOffset().x)
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
				tSize = this->m_pText->GetSize((this->m_pInput->Text_Get_String() + std::string("_")).c_str(), this->m_cMapNameButton.GetTextScale());
			} while (slong(tSize.w) > tRect.w - this->m_cMapNameButton.GetTextOffset().x);
		}
		this->m_cMapNameButton.SetText(this->m_pInput->Text_Get_String());
	}
	this->m_dMarkerTimer -= dDelta;
	if (this->m_dMarkerTimer < 0.0f)
	{
		this->m_bMarkerVisible = !this->m_bMarkerVisible;
		this->m_dMarkerTimer = kdMarkerRate;
	}
}
void CForgeState::Render(void)
{
	this->m_pStarField->Render();
	this->RenderButtons();
	this->RenderGrid();
	this->RenderText();
	//for (slong x = 0; x < GRID::kGridSize.x; ++x)
	//{
	//	for (slong y = 0; y < GRID::kGridSize.y; ++y)
	//	{
	//		this->m_pSprite->DrawRect(GRID::kGrid[x][y], CLR::CHARTREUSE, false, 4);
	//	}
	//}
}

//Enter
void CForgeState::EnterPrimary(void)
{
	//Button vec
	std::vector<GUI::CButton*> vButs;
	//Grab Sfx
	slong sfxBack = this->m_pAudio->LoadAudio("Assets/Sfx/back.wav", Audio::EAudioType::SFX_2D);
	slong sfxPush = this->m_pAudio->LoadAudio("Assets/Sfx/push.wav", Audio::EAudioType::SFX_2D);
	slong sfxPull = this->m_pAudio->LoadAudio("Assets/Sfx/pull.wav", Audio::EAudioType::SFX_2D);
	slong sfxHover = this->m_pAudio->LoadAudio("Assets/Sfx/hover.wav", Audio::EAudioType::SFX_2D);
	//Shared types
	Type2<ulong> tSize;

	/*Shared Settings*/
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

	/*Individual Settings*/
	//Back Button
	tSize = this->m_pText->GetSize("Back", 1.0f);
	m_vButtons[BACK].SetText("Back");
	m_vButtons[BACK].SetRect(_RECT<slong>(Sprite::SCREEN_WIDTH - tSize.x - 32, 0, tSize.x, tSize.y));
	m_vButtons[BACK].SetSfx(sfxBack, GUI::CButton::EButtonSfx::SFX_PULL);

	/*Group Settings*/
	this->m_vButtonGroup[PRIMARY].SetButtonList(vButs);
	this->m_vButtonGroup[PRIMARY].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[PRIMARY].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[PRIMARY].SetInputCMD(this->m_pInput);
	this->m_vButtonGroup[PRIMARY].SetButtonGroupDir(UP, MAPBUTS);
	this->m_vButtonGroup[PRIMARY].SetButtonGroupDir(LEFT, SPAWNS);
	//this->m_vButtonGroup[PRIMARY].SetFindClosestButtonGroup(true);
	this->m_vButtonGroup[PRIMARY].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
}
void CForgeState::EnterSlots(void)
{
	//Button vec
	std::vector<GUI::CButton*> vButs;
	//Grab Sfx
	slong sfxGood = this->m_pAudio->LoadAudio("Assets/Sfx/beep_good.wav", Audio::EAudioType::SFX_2D);

	/*Shared Settings*/
	for (ulong i = 0; i < NUM_SLOT_BUTTONS; i++)
	{
		//Clear All Colors
		m_vSlotButtons[i].DefaultColor(CLR::CLEAR);
		//Text Color
		this->m_vSlotButtons[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
		this->m_vSlotButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON);
		this->m_vSlotButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_HOVER);
		this->m_vSlotButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_vSlotButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF);
		this->m_vSlotButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_vSlotButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_MDOWN);
		//Background Color
		this->m_vSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
		this->m_vSlotButtons[i].SetBGColor(CLR::SLATE, GUI::CButton::EButtonState::ON);
		this->m_vSlotButtons[i].SetBGColor(CLR::SLATE, GUI::CButton::EButtonState::ON_HOVER);
		this->m_vSlotButtons[i].SetBGColor(CLR::SLATE, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_vSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF);
		this->m_vSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_vSlotButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_MDOWN);
		//Outline Color
		this->m_vSlotButtons[i].SetOutlineColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
		this->m_vSlotButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::ON);
		this->m_vSlotButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::ON_HOVER);
		this->m_vSlotButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_vSlotButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::OFF);
		this->m_vSlotButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_vSlotButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::OFF_MDOWN);
		//Outline Size
		this->m_vSlotButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::ON_HOVER);
		this->m_vSlotButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_vSlotButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_vSlotButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::OFF_MDOWN);
		//Sfx
		this->m_vSlotButtons[i].SetSfx(sfxGood, GUI::CButton::EButtonSfx::SFX_PULL);
		//Rect
		Type2<slong> index = Type2<slong>(i % 3, i / 3);
		_RECT<slong> rect = GRID::kGrid[9][2];
		rect.x += index.x * (GRID::kSize.w + GRID::kSpace.w);
		rect.y += (3 - index.y - 1) * (GRID::kSize.h + GRID::kSpace.h);
		this->m_vSlotButtons[i].SetRect(rect);
		//Text
		Type2<ulong> size = this->m_pText->GetSize("0", 1.0f);
		this->m_vSlotButtons[i].SetText(std::to_string(i));
		this->m_vSlotButtons[i].SetTextOffset(Type2<slong>(0, -(slong)(size.h * 0.22f)));
		//Push Back
		vButs.push_back(&this->m_vSlotButtons[i]);
	}

	/*Individual Settings*/
	//X Button
	this->m_vSlotButtons[OFF].SetText("X");

	/*Group Settings*/
	this->m_vButtonGroup[SLOTS].SetButtonList(vButs);
	this->m_vButtonGroup[SLOTS].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[SLOTS].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[SLOTS].SetInputCMD(this->m_pInput);
	this->m_vButtonGroup[SLOTS].SetButtonGroupDir(UP, DEFAULT_SLOTS);
	this->m_vButtonGroup[SLOTS].SetButtonGroupDir(DOWN, SPAWNS);
	this->m_vButtonGroup[SLOTS].SetButtonGroupDir(RIGHT, MAPBUTS);
	this->m_vButtonGroup[SLOTS].SetButtonGroupDir(LEFT, GRID);
	this->m_vButtonGroup[SLOTS].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
}
void CForgeState::EnterSpawns(void)
{
	//Button vec
	std::vector<GUI::CButton*> vButs;
	//Grab Sfx
	slong sfxGood = this->m_pAudio->LoadAudio("Assets/Sfx/beep_good.wav", Audio::EAudioType::SFX_2D);

	/*Shared Settings*/
	for (ulong i = 0; i < NUM_PLAYER_BUTTONS; i++)
	{
		//Clear All Colors
		this->m_vPlayerButtons[i].DefaultColor(CLR::CLEAR);
		//Text Color
		this->m_vPlayerButtons[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
		this->m_vPlayerButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON);
		this->m_vPlayerButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_HOVER);
		this->m_vPlayerButtons[i].SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_vPlayerButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF);
		this->m_vPlayerButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_vPlayerButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_MDOWN);
		//Background Color
		this->m_vPlayerButtons[i].SetBGColor(kutPlayerColors[i] * 0.25f, GUI::CButton::EButtonState::INACTIVE);
		this->m_vPlayerButtons[i].SetBGColor(kutPlayerColors[i] * 1.00f, GUI::CButton::EButtonState::ON);
		this->m_vPlayerButtons[i].SetBGColor(kutPlayerColors[i] * 0.75f, GUI::CButton::EButtonState::ON_HOVER);
		this->m_vPlayerButtons[i].SetBGColor(kutPlayerColors[i] * 0.50f, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_vPlayerButtons[i].SetBGColor(kutPlayerColors[i] * 1.00f, GUI::CButton::EButtonState::OFF);
		this->m_vPlayerButtons[i].SetBGColor(kutPlayerColors[i] * 0.75f, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_vPlayerButtons[i].SetBGColor(kutPlayerColors[i] * 0.50f, GUI::CButton::EButtonState::OFF_MDOWN);
		//Outline Color
		this->m_vPlayerButtons[i].SetOutlineColor(CLR::CLEAR, GUI::CButton::EButtonState::INACTIVE);
		this->m_vPlayerButtons[i].SetOutlineColor(CLR::DARK_GREY, GUI::CButton::EButtonState::ON);
		this->m_vPlayerButtons[i].SetOutlineColor(CLR::GREY, GUI::CButton::EButtonState::ON_HOVER);
		this->m_vPlayerButtons[i].SetOutlineColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_vPlayerButtons[i].SetOutlineColor(CLR::DARK_GREY, GUI::CButton::EButtonState::OFF);
		this->m_vPlayerButtons[i].SetOutlineColor(CLR::GREY, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_vPlayerButtons[i].SetOutlineColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::OFF_MDOWN);
		//Outline Size
		this->m_vPlayerButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::ON_HOVER);
		this->m_vPlayerButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_vPlayerButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_vPlayerButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::OFF_MDOWN);
		//Sfx
		this->m_vPlayerButtons[i].SetSfx(sfxGood, GUI::CButton::EButtonSfx::SFX_PULL);
		//Rect
		Type2<slong> index = Type2<slong>(i % 2, i / 2);
		_RECT<slong> rect = GRID::kGrid[9 + index.x][1 - index.y];
		rect.x += slong((GRID::kSize.w + GRID::kSpace.w) * 0.5f);
		this->m_vPlayerButtons[i].SetRect(rect);
		//Push Back
		vButs.push_back(&this->m_vPlayerButtons[i]);
	}

	/*Group Settings*/
	this->m_vButtonGroup[SPAWNS].SetButtonList(vButs);
	this->m_vButtonGroup[SPAWNS].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[SPAWNS].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[SPAWNS].SetInputCMD(this->m_pInput);
	this->m_vButtonGroup[SPAWNS].SetButtonGroupDir(RIGHT, PRIMARY);
	this->m_vButtonGroup[SPAWNS].SetButtonGroupDir(LEFT, GRID);
	this->m_vButtonGroup[SPAWNS].SetButtonGroupDir(UP, SLOTS);
	this->m_vButtonGroup[SPAWNS].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
}
void CForgeState::EnterGrid(void)
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
			//Outline Color
			this->m_vGrid[x][y].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
			this->m_vGrid[x][y].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
			this->m_vGrid[x][y].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
			//Outline Size
			this->m_vGrid[x][y].SetOutlineSize(6, GUI::CButton::EButtonState::HOVER);
			this->m_vGrid[x][y].SetOutlineSize(6, GUI::CButton::EButtonState::MDOWN);
			//Sfx
			this->m_vGrid[x][y].SetSfx(sfxGood, GUI::CButton::EButtonSfx::SFX_PULL);
			this->m_vGrid[x][y].SetSfx(sfxHover, GUI::CButton::EButtonSfx::SFX_HOVER);
			//Rect
			this->m_vGrid[x][y].SetRect(GRID::kGrid[x][y]);
			//Special Case Button Group Movement
			if (y == 0)
				this->m_vGrid[x][y].SetButtonGroupDir(-2, DOWN);
			//Push Back
			vButs.push_back(&m_vGrid[x][y]);
		}
	}
	/*Individual Settings*/
	this->m_vGrid[7][7].SetButtonGroupDir(MAPNAME, UP);

	/*Group Settings*/
	this->m_vButtonGroup[GRID].SetButtonList(vButs);
	this->m_vButtonGroup[GRID].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[GRID].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[GRID].SetInputCMD(this->m_pInput);
	this->m_vButtonGroup[GRID].SetButtonGroupDir(RIGHT, SLOTS);
	this->m_vButtonGroup[GRID].SetButtonGroupDir(UP, MAPNAME);
	this->m_vButtonGroup[GRID].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
	this->m_vButtonGroup[GRID].SetFindClosestButtonGroup(true);
}
void CForgeState::EnterMapName(void)
{
	//Button vec
	std::vector<GUI::CButton*> vButs;
	//Grab Sfx
	slong sfxGood = this->m_pAudio->LoadAudio("Assets/Sfx/beep_good.wav", Audio::EAudioType::SFX_2D);

	/*Shared Settings*/
	{
		//Clear all colors
		this->m_cMapNameButton.DefaultColor(CLR::CLEAR);
		//Text Color
		this->m_cMapNameButton.SetTextColor(CLR::GREY, GUI::CButton::EButtonState::INACTIVE);
		this->m_cMapNameButton.SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON);
		this->m_cMapNameButton.SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_HOVER);
		this->m_cMapNameButton.SetTextColor(CLR::LIGHT_GREY, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_cMapNameButton.SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF);
		this->m_cMapNameButton.SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_cMapNameButton.SetTextColor(CLR::GREY, GUI::CButton::EButtonState::OFF_MDOWN);
		//Background Color
		this->m_cMapNameButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
		this->m_cMapNameButton.SetBGColor(CLR::GREY, GUI::CButton::EButtonState::ON);
		this->m_cMapNameButton.SetBGColor(CLR::GREY, GUI::CButton::EButtonState::ON_HOVER);
		this->m_cMapNameButton.SetBGColor(CLR::GREY, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_cMapNameButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF);
		this->m_cMapNameButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_cMapNameButton.SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::OFF_MDOWN);
		//Outline Color
		this->m_cMapNameButton.SetOutlineColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
		this->m_cMapNameButton.SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::ON);
		this->m_cMapNameButton.SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::ON_HOVER);
		this->m_cMapNameButton.SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_cMapNameButton.SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::OFF);
		this->m_cMapNameButton.SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_cMapNameButton.SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::OFF_MDOWN);
		//Outline Size
		this->m_cMapNameButton.SetOutlineSize(6, GUI::CButton::EButtonState::ON_HOVER);
		this->m_cMapNameButton.SetOutlineSize(6, GUI::CButton::EButtonState::ON_MDOWN);
		this->m_cMapNameButton.SetOutlineSize(6, GUI::CButton::EButtonState::OFF_HOVER);
		this->m_cMapNameButton.SetOutlineSize(6, GUI::CButton::EButtonState::OFF_MDOWN);
		//Sfx
		this->m_cMapNameButton.SetSfx(sfxGood, GUI::CButton::EButtonSfx::SFX_PULL);
		//Rect
		_RECT<slong> rect = this->m_vGrid[3][7].GetRect();
		rect.y += 112;
		rect.w = 5 * (GRID::kSize.w + GRID::kSpace.w) - GRID::kSpace.w;
		this->m_cMapNameButton.SetRect(rect);
		//Text
		this->m_cMapNameButton.SetTextScale(0.56f);
		this->m_cMapNameButton.SetTextOffset(Type2<slong>(12, 0));
		this->m_cMapNameButton.SetTextAlignment(Text::EFontAlignment::F_LEFT);
		//Push Back
		vButs.push_back(&this->m_cMapNameButton);
	}

	/*Group Settings*/
	this->m_vButtonGroup[MAPNAME].SetButtonList(vButs);
	this->m_vButtonGroup[MAPNAME].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[MAPNAME].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[MAPNAME].SetInputCMD(this->m_pInput);
	this->m_vButtonGroup[MAPNAME].SetButtonGroupDir(DOWN, GRID);
	this->m_vButtonGroup[MAPNAME].SetButtonGroupDir(RIGHT, DEFAULT_SLOTS);
	this->m_vButtonGroup[MAPNAME].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
}
void CForgeState::EnterMapButs(void)
{
	//Button vec
	std::vector<GUI::CButton*> vButs;
	//Grab Sfx
	slong sfxGood = this->m_pAudio->LoadAudio("Assets/Sfx/beep_good.wav", Audio::EAudioType::SFX_2D);

	/*Shared Settings*/
	for (ulong i = 0; i < NUM_MAP_BUTTONS; i++)
	{
		//Clear all colors
		m_vMapButtons[i].DefaultColor(CLR::CLEAR);
		//Text Color
		this->m_vMapButtons[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::INACTIVE);
		this->m_vMapButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::DEFAULT);
		this->m_vMapButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::HOVER);
		this->m_vMapButtons[i].SetTextColor(CLR::GREY, GUI::CButton::EButtonState::MDOWN);
		//Background Color
		this->m_vMapButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::INACTIVE);
		this->m_vMapButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::DEFAULT);
		this->m_vMapButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::HOVER);
		this->m_vMapButtons[i].SetBGColor(CLR::CHARCOAL, GUI::CButton::EButtonState::MDOWN);
		//Outline Color
		this->m_vMapButtons[i].SetOutlineColor(CLR::CLEAR, GUI::CButton::EButtonState::INACTIVE);
		this->m_vMapButtons[i].SetOutlineColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
		this->m_vMapButtons[i].SetOutlineColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
		this->m_vMapButtons[i].SetOutlineColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
		//Outline Size
		this->m_vMapButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::HOVER);
		this->m_vMapButtons[i].SetOutlineSize(6, GUI::CButton::EButtonState::MDOWN);
		//Sfx
		this->m_vMapButtons[i].SetSfx(sfxGood, GUI::CButton::EButtonSfx::SFX_PULL);
		//Rect
		_RECT<slong> rect = GRID::kGrid[14][1];
		rect.y += (NUM_MAP_BUTTONS - i - 1) * (GRID::kSize.h + GRID::kSpace.h);
		rect.w = 3 * (GRID::kSize.w + GRID::kSpace.w) - GRID::kSpace.w;
		this->m_vMapButtons[i].SetRect(rect);
		//Text
		Type2<ulong> size = this->m_pText->GetSize("0", 0.7f);
		this->m_vMapButtons[i].SetTextOffset(Type2<slong>(0, -(slong)(size.h * 0.12f)));
		this->m_vMapButtons[i].SetTextScale(0.7f);
		this->m_vMapButtons[i].SetTextAlignment(Text::EFontAlignment::F_CENTER);
		//Push Back
		vButs.push_back(&this->m_vMapButtons[i]);
	}

	/*Individual Settings*/
	//New Button
	Type2<ulong> tSize = this->m_pText->GetSize("NEW", 1.0f);
	m_vMapButtons[NEW].SetText("NEW");
	m_vMapButtons[NEW].SetButtonGroupDir(DEFAULT_SLOTS, RIGHT);
	//Copy Button
	tSize = this->m_pText->GetSize("COPY", 1.0f);
	m_vMapButtons[COPY].SetText("COPY");
	m_vMapButtons[COPY].SetButtonGroupDir(DEFAULT_SLOTS, RIGHT);
	//Del Button
	tSize = this->m_pText->GetSize("DELETE", 1.0f);
	m_vMapButtons[DEL].SetText("DELETE");
	m_vMapButtons[DEL].SetButtonGroupDir(DEFAULT_SLOTS, RIGHT);
	Type2<slong> textOffset = this->m_vMapButtons[DEL].GetTextOffset();
	textOffset.x = 4;
	m_vMapButtons[DEL].SetTextOffset(textOffset);

	/*Group Settings*/
	this->m_vButtonGroup[MAPBUTS].SetButtonList(vButs);
	this->m_vButtonGroup[MAPBUTS].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[MAPBUTS].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[MAPBUTS].SetInputCMD(this->m_pInput);
	this->m_vButtonGroup[MAPBUTS].SetButtonGroupDir(UP, CUSTOM_SLOTS);
	this->m_vButtonGroup[MAPBUTS].SetButtonGroupDir(DOWN, PRIMARY);
	this->m_vButtonGroup[MAPBUTS].SetButtonGroupDir(LEFT, SPAWNS);
	//this->m_vButtonGroup[MAPBUTS].SetFindClosestButtonGroup(true);
	this->m_vButtonGroup[MAPBUTS].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
}
void CForgeState::EnterDefaultSlots(void)
{
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
		_RECT<slong> rect = GRID::kGrid[8][6];
		rect.x += slong((GRID::kSize.w + GRID::kSpace.w) * 0.5f);
		rect.y += (NUM_LIST_SLOTS - i - 1) * 52;
		rect.w = 3 * (GRID::kSize.w + GRID::kSpace.w) - GRID::kSpace.w;
		rect.h = 48;
		this->m_vDefaultSlotButtons[i].SetRect(rect);
		//Text
		Type2<ulong> size = this->m_pText->GetSize("0", 0.7f);
		Type2<slong> text_offset = this->m_cMapNameButton.GetTextOffset();
		text_offset.x = slong(text_offset.x * 0.588235f);
		text_offset.y = slong(text_offset.y * 0.588235f);
		this->m_vDefaultSlotButtons[i].SetTextOffset(text_offset);
		this->m_vDefaultSlotButtons[i].SetTextScale(this->m_cMapNameButton.GetTextScale() * 0.588235f);
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
	this->m_vButtonGroup[DEFAULT_SLOTS].SetButtonGroupDir(LEFT, GRID);
	this->m_vButtonGroup[DEFAULT_SLOTS].SetButtonGroupDir(RIGHT, DEFAULT_BUTS);
	this->m_vButtonGroup[DEFAULT_SLOTS].SetButtonGroupDir(DOWN, SLOTS);
	//this->m_vButtonGroup[DEFAULT_SLOTS].SetFindClosestButtonGroup(true);
	this->m_vButtonGroup[DEFAULT_SLOTS].SetMoveType(GUI::CButton::EMoveType::PREVIOUS);
}
void CForgeState::EnterDefaultButs(void)
{
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
		_RECT<slong> rect = GRID::kGrid[11][6];
		rect.y += (NUM_LIST_BUTS - i - 1) * (GRID::kSize.h + GRID::kSpace.h);
		rect.x += slong((GRID::kSize.w + GRID::kSpace.w) * 0.5f);
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
	this->m_vButtonGroup[DEFAULT_BUTS].SetButtonGroupDir(DOWN, SLOTS);
	this->m_vButtonGroup[DEFAULT_BUTS].SetButtonGroupDir(LEFT, DEFAULT_SLOTS);
	this->m_vButtonGroup[DEFAULT_BUTS].SetButtonGroupDir(RIGHT, CUSTOM_SLOTS);
	this->m_vButtonGroup[DEFAULT_BUTS].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
}
void CForgeState::EnterCustomSlots(void)
{
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
		_RECT<slong> rect = GRID::kGrid[13][6];
		rect.y += (NUM_LIST_SLOTS - i - 1) * 52;
		rect.w = 3 * (GRID::kSize.w + GRID::kSpace.w) - GRID::kSpace.w;
		rect.h = 48;
		this->m_vCustomSlotButtons[i].SetRect(rect);
		//Text
		Type2<ulong> size = this->m_pText->GetSize("0", 0.7f);
		Type2<slong>text_offset = this->m_cMapNameButton.GetTextOffset();
		text_offset.x = slong(text_offset.x * 0.588235f);
		text_offset.y = slong(text_offset.y * 0.588235f);
		this->m_vCustomSlotButtons[i].SetTextOffset(text_offset);
		this->m_vCustomSlotButtons[i].SetTextScale(this->m_cMapNameButton.GetTextScale() * 0.588235f);
		this->m_vCustomSlotButtons[i].SetTextAlignment(Text::EFontAlignment::F_LEFT);
		vButs.push_back(&this->m_vCustomSlotButtons[i]);
	}

	/*Group Settings*/
	this->m_vButtonGroup[CUSTOM_SLOTS].SetButtonList(vButs);
	this->m_vButtonGroup[CUSTOM_SLOTS].SetMove(Type2<bool>(true, true));
	this->m_vButtonGroup[CUSTOM_SLOTS].SetWrap(Type2<bool>(false, false));
	this->m_vButtonGroup[CUSTOM_SLOTS].SetInputCMD(this->m_pInput);
	this->m_vButtonGroup[CUSTOM_SLOTS].SetButtonGroupDir(DOWN, PRIMARY);
	this->m_vButtonGroup[CUSTOM_SLOTS].SetButtonGroupDir(LEFT, SLOTS);
	this->m_vButtonGroup[CUSTOM_SLOTS].SetButtonGroupDir(RIGHT, CUSTOM_BUTS);
	//this->m_vButtonGroup[CUSTOM_SLOTS].SetFindClosestButtonGroup(true);
	this->m_vButtonGroup[CUSTOM_SLOTS].SetMoveType(GUI::CButton::EMoveType::PREVIOUS);
}
void CForgeState::EnterCustomButs(void)
{
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
		_RECT<slong> rect = GRID::kGrid[16][6];
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
	this->m_vButtonGroup[CUSTOM_BUTS].SetButtonGroupDir(DOWN, MAPBUTS);
	this->m_vButtonGroup[CUSTOM_BUTS].SetButtonGroupDir(LEFT, CUSTOM_SLOTS);
	this->m_vButtonGroup[CUSTOM_BUTS].SetMoveType(GUI::CButton::EMoveType::CLOSEST);
}
//Input
void CForgeState::InputPrimary(void)
{
	if (this->m_vButtons[BACK].GetClick().Pull())
		this->m_tResult.Prev(IGameState::EGameStateType::gs_MAIN_MENU);
}
void CForgeState::InputSlots(void)
{
	for (ulong i = 0; i < NUM_SLOT_BUTTONS; i++)
	{
		if (this->m_vSlotButtons[i].GetClick().Pull())
		{
			//If it is on, then turn all the other buttons off
			if (this->m_vSlotButtons[i].GetToggle())
			{
				//Set Active
				for (ulong j = 0; j < NUM_BUTTON_GROUPS; j++)
					this->m_vButtonGroup[j].SetActive(false);
				this->m_vButtonGroup[PRIMARY].SetActive(true);
				this->m_vButtonGroup[GRID].SetActive(true);
				this->m_vSlotButtons[i].SetActive(true);
				//Set Toggle
				for (ulong j = 0; j < NUM_SLOT_BUTTONS; j++)
					this->m_vSlotButtons[j].SetToggle(i == j);
				for (ulong j = 0; j < NUM_PLAYER_BUTTONS; j++)
					this->m_vPlayerButtons[j].SetToggle(false);
				//Set Selected
				this->m_eSelectedSlot = ESlotButtons(i);
				this->m_eSelectedPlayer = EPlayerButtons::NO_PBUT;
			}
			//Otherwise, turn all the other buttons back on
			else
			{
				//Set Active
				this->DetermineActiveButtons();
				//Set Toggle
				for (ulong j = 0; j < NUM_SLOT_BUTTONS; j++)
					this->m_vSlotButtons[j].SetToggle(false);
				for (ulong j = 0; j < NUM_PLAYER_BUTTONS; j++)
					this->m_vPlayerButtons[j].SetToggle(false);
				//Set Selected
				this->m_eSelectedSlot = ESlotButtons::NO_SBUT;
				this->m_eSelectedPlayer = EPlayerButtons::NO_PBUT;
			}
		}
	}
}
void CForgeState::InputSpawns(void)
{
	for (ulong i = 0; i < NUM_PLAYER_BUTTONS; i++)
	{
		if (this->m_vPlayerButtons[i].GetClick().Pull())
		{
			//turn off all the other buttons
			if (this->m_vPlayerButtons[i].GetToggle())
			{
				//Set Active
				for (ulong j = 0; j < NUM_BUTTON_GROUPS; j++)
					this->m_vButtonGroup[j].SetActive(false);
				this->m_vButtonGroup[PRIMARY].SetActive(true);
				this->m_vButtonGroup[GRID].SetActive(true);
				this->m_vPlayerButtons[i].SetActive(true);
				//Set Toggle
				for (ulong j = 0; j < NUM_SLOT_BUTTONS; j++)
					this->m_vSlotButtons[j].SetToggle(false);
				for (ulong j = 0; j < NUM_PLAYER_BUTTONS; j++)
					this->m_vPlayerButtons[j].SetToggle(i == j);
				//Set Selected
				this->m_eSelectedSlot = ESlotButtons::NO_SBUT;
				this->m_eSelectedPlayer = EPlayerButtons(i);
			}
			else
			{
				//Set Active
				this->DetermineActiveButtons();
				//Set Toggle
				for (ulong j = 0; j < NUM_SLOT_BUTTONS; j++)
					this->m_vSlotButtons[j].SetToggle(false);
				for (ulong j = 0; j < NUM_PLAYER_BUTTONS; j++)
					this->m_vPlayerButtons[j].SetToggle(false);
				//Set Selected
				this->m_eSelectedSlot = ESlotButtons::NO_SBUT;
				this->m_eSelectedPlayer = EPlayerButtons::NO_PBUT;
			}
		}
	}
}
void CForgeState::InputGrid(void)
{
	for (ulong x = 0; x < Board::kslBoardSize; x++)
	{
		for (ulong y = 0; y < Board::kslBoardSize; y++)
		{
			if (this->m_vGrid[x][y].GetClick().Pull())
			{
				//Adjust the selected panel
				CPanel panel = this->m_cBoard.GetPanel(uchar(x), uchar(y));
				{
					//Adjust it's slots
					if (this->m_eSelectedSlot != NO_SBUT)
					{
						auto vec = panel.GetSlots();
						if (!panel.GetActive() || vec.size() != this->m_eSelectedSlot)
						{
							panel.SetActive(this->m_eSelectedSlot != OFF);
							schar spawn = (vec.empty()) ? -1 : vec[0];
							vec.clear();
							for (slong i = 0; i < this->m_eSelectedSlot; i++)
								vec.push_back(-1);
							if (!vec.empty())
								vec[0] = spawn;
							panel.SetSlots(vec);
						}
						else
						{
							panel.SetSlots(std::vector<schar>());
							panel.SetActive(false);
						}
					}
					//Adjust it's player spawns
					else if (this->m_eSelectedPlayer != NO_PBUT)
					{
						auto vec = panel.GetSlots();
						if (!vec.empty())
						{
							//Clear out the grid of this spawn
							for (slong j = 0; j < Board::kslBoardSize; j++)
							{
								for (slong k = 0; k < Board::kslBoardSize; k++)
								{
									if (j == x && k == y)
										continue;

									CPanel pTemp = this->m_cBoard.GetPanel(uchar(j), uchar(k));
									auto vTemp = pTemp.GetSlots();
									if (!vTemp.empty() && vTemp[0] == this->m_eSelectedPlayer)
										vTemp[0] = -1;
									pTemp.SetSlots(vTemp);
									this->m_cBoard.SetPanel(pTemp, uchar(j), uchar(k));
								}
							}
							if (vec[0] == this->m_eSelectedPlayer)
								vec[0] = -1;
							else
								vec[0] = this->m_eSelectedPlayer;
							panel.SetSlots(vec);
						}
					}
					//Turn it on/off
					else
					{
						panel.SetSlots(std::vector<schar>());
						panel.SetActive(!panel.GetActive());
					}
				}
				this->m_cBoard.SetPanel(panel, uchar(x), uchar(y));
				this->GetBoard();
			}
		}
	}
}
void CForgeState::InputMapName(void)
{
	if (this->m_cMapNameButton.GetClick().Pull())
	{
		if (this->m_cMapNameButton.GetToggle())
		{
			//Set Active
			for (ulong i = 0; i < NUM_BUTTON_GROUPS; i++)
				this->m_vButtonGroup[i].SetActive(false);
			this->m_vButtonGroup[PRIMARY].SetActive(true);
			this->m_vButtonGroup[MAPNAME].SetActive(true);
			//Set Text
			this->m_pInput->Text_Set_Mode(true);
			this->m_pInput->Text_Set_String(this->m_cMapNameButton.GetText().c_str());
			this->m_pInput->Text_Set_Marker(this->m_cMapNameButton.GetText().length());
		}
		else
		{
			std::string original_name = this->m_cBoard.GetMapName();
			//Set Text
			this->m_pInput->Text_Set_Mode(false);
			this->m_cBoard.SetMapName(this->m_cMapNameButton.GetText());
			this->GetBoard();
			//Set Active
			if (this->m_cBoard.GetMapName() != original_name)
				this->SortBoards();
			this->DetermineActiveButtons();
		}
	}
}
void CForgeState::InputMapButs(void)
{
	if (this->m_vMapButtons[NEW].GetClick().Pull() && this->m_vCustomList.size() < 32)
	{
		CBoard cBoard;
		cBoard.SetMapName("New Map");
		cBoard.SetDefault(false);
		this->m_vCustomList.push_back(cBoard);
		this->SortBoards();
		for (slong i = this->m_vCustomList.size() - 1; i >= 0; i--)
		{
			if (cBoard == this->m_vCustomList[i])
			{
				this->m_slCustomIndex = i;
				this->m_slCustomOffset = i;
			}
		}
		for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
			this->m_vCustomSlotButtons[i].SetToggle(i == 0);
		this->m_bDefault = false;
		this->DetermineActiveButtons();
	}
	if (this->m_vMapButtons[COPY].GetClick().Pull() && this->m_vCustomList.size() < 32)
	{
		CBoard cBoard;
		if (this->m_bDefault && this->m_slDefaultIndex != -1)
			cBoard = this->m_vDefaultList[this->m_slDefaultIndex];
		else if (this->m_slCustomIndex != -1)
			cBoard = this->m_vCustomList[this->m_slCustomIndex];
		cBoard.SetDefault(false);
		this->m_vCustomList.push_back(cBoard);
		this->SortBoards();
		for (slong i = this->m_vCustomList.size() - 1; i >= 0; i--)
		{
			if (cBoard == this->m_vCustomList[i])
			{
				this->m_slCustomIndex = i;
				this->m_slCustomOffset = i;
			}
		}
		for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
			this->m_vCustomSlotButtons[i].SetToggle(i == 0);
		this->m_bDefault = false;
		this->DetermineActiveButtons();
	}
	if (this->m_vMapButtons[DEL].GetClick().Pull() && !this->m_bDefault)
	{
		if (this->m_slCustomIndex == -1)
			return;
		this->m_vCustomList.erase(this->m_vCustomList.begin() + this->m_slCustomIndex);
		if (!this->m_vCustomList.empty())
		{
			this->m_slCustomIndex = clamp<slong>(this->m_slCustomIndex--, 0, this->m_vCustomList.size() - 1);
			this->m_slCustomOffset = clamp<slong>(this->m_slCustomOffset, 0, this->m_vCustomList.size() - 1);
		}
		else
		{
			this->m_bDefault = true;
			this->m_slDefaultIndex = 0;
			this->m_slDefaultOffset = 0;
			for (ulong i = 0; i < NUM_LIST_SLOTS; i++)
				this->m_vDefaultSlotButtons[i].SetToggle(i == 0);
			this->m_slCustomIndex = -1;
			this->m_slCustomOffset = 0;
		}
		this->SortBoards();
		this->DetermineActiveButtons();
	}
}
void CForgeState::InputDefaultSlots(void)
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
				this->m_slDefaultIndex = i + this->m_slDefaultOffset;
				this->m_bDefault = true;
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
void CForgeState::InputDefaultButs(void)
{
	slong dir = 0;
	if (this->m_vDefaultListButtons[INC].GetClick().Pull())
		dir++;
	else if (this->m_vDefaultListButtons[DEC].GetClick().Pull())
		dir--;

	//Early Out
	if (dir == 0)
		return;

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
}
void CForgeState::InputCustomSlots(void)
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
				this->m_slCustomIndex = i + this->m_slCustomOffset;
				this->m_bDefault = false;
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
void CForgeState::InputCustomButs(void)
{
	slong dir = 0;
	if (this->m_vCustomListButtons[INC].GetClick().Pull())
		dir++;
	else if (this->m_vCustomListButtons[DEC].GetClick().Pull())
		dir--;

	//Early Out
	if (dir == 0)
		return;

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
}
void CForgeState::InputSpecialCase_Grid(void)
{
	//If we click anywhere but a button and we are currently in edit grid mode, then exit edit grid mode
	//check if we are clicking inside the 'grid' area, if we are then don't exit edit mode
	_RECT<slong> rect = _RECT<slong>(this->m_vGrid[0][0].GetRect().x, this->m_vGrid[0][0].GetRect().y,
		this->m_vGrid[7][7].GetRect().x + this->m_vGrid[7][7].GetRect().w - this->m_vGrid[0][0].GetRect().x,
		this->m_vGrid[7][7].GetRect().y + this->m_vGrid[7][7].GetRect().h - this->m_vGrid[0][0].GetRect().y);
	bool confirm = (this->m_pInput->Key_Pull(VK_LBUTTON) && !Collision::Intersect_Type2_AABB<slong>(this->m_pInput->Cursor_SDL(), Collision::tAABB<slong>(rect))) || (this->m_pInput->Ctrl_Pull(CTRL_KEYS::XB_B) || this->m_pInput->Ctrl_Pull(CTRL_KEYS::XB_BACK));
	if (!this->m_cMapNameButton.GetToggle() && confirm && (this->m_eSelectedPlayer != NO_PBUT || this->m_eSelectedSlot != NO_SBUT))
	{
		bool hit = false;
		for (ulong i = 0; i < NUM_BUTTON_GROUPS; i++)
		{
			if (this->m_vButtonGroup[i].GetAnyPull())
			{
				hit = true;
				break;
			}
		}
		if (!hit)
		{
			//Set Active
			this->DetermineActiveButtons();
			//Set Toggle
			for (ulong j = 0; j < NUM_SLOT_BUTTONS; j++)
				this->m_vSlotButtons[j].SetToggle(false);
			for (ulong j = 0; j < NUM_PLAYER_BUTTONS; j++)
				this->m_vPlayerButtons[j].SetToggle(false);
			//Set Selected
			this->m_eSelectedSlot = ESlotButtons::NO_SBUT;
			this->m_eSelectedPlayer = EPlayerButtons::NO_PBUT;
		}
	}
}
//Rendering
void CForgeState::RenderButtons(void)
{
	for (ulong i = 0; i < NUM_BUTTON_GROUPS; i++)
	{
		//Ignore the grid, we do that in 'RenderGrid()'
		//Ignore the slots, we do that in 'RenderText()'
		if (i == GRID || i == DEFAULT_SLOTS || i == CUSTOM_SLOTS)
			continue;
		this->m_vButtonGroup[i].Render(this->m_pSprite, this->m_pText);
	}
}
void CForgeState::RenderText(void)
{
	Type2<ulong> size = this->m_pText->GetSize("0", 1.0f);

#pragma region Quantum_Slots
	size = this->m_pText->GetSize("Quantum Slots", 0.56f);
	_RECT<slong> rect = _RECT<slong>(this->m_vSlotButtons[S0].GetRect().GetPos().x, this->m_vSlotButtons[S0].GetRect().GetPos().y, 112 * 3, size.h);
	slong center = rect.x + slong(rect.w * 0.5f) - 8;
	rect.y += 112;
	rect.x = center - slong(size.w * 0.5f);
	rect.w = size.w;
	this->m_pText->Write("Quantum Slots", rect, CLR::LAVENDER, 0.56f, true, Text::EFontAlignment::F_LEFT);
	this->m_pSprite->DrawLine(rect.x, rect.y, rect.x + rect.w, rect.y, CLR::WHITE, 2);
#pragma endregion
#pragma region Player_Spawns
	size = this->m_pText->GetSize("Player Spawns", 0.56f);
	rect.y = this->m_vPlayerButtons[P1].GetRect().GetPos().y + 112;
	rect.x = center - slong(size.w * 0.5f);
	rect.w = size.w;
	this->m_pText->Write("Player Spawns", rect, CLR::LAVENDER, 0.56f, true, Text::EFontAlignment::F_LEFT);
	this->m_pSprite->DrawLine(rect.x, rect.y, rect.x + rect.w, rect.y, CLR::WHITE, 2);
#pragma endregion
#pragma region Board_Name
	size = this->m_pText->GetSize("Board Name:", 0.47f);
	rect.x = this->m_vGrid[0][7].GetRect().GetPos().x;
	rect.y = this->m_vGrid[0][7].GetRect().GetPos().y + 112;
	rect.w = size.w;
	this->m_pText->Write("Board Name:", rect, CLR::LAVENDER, 0.47f, true, Text::EFontAlignment::F_LEFT);
	this->m_pSprite->DrawLine(rect.x, rect.y, rect.x + rect.w, rect.y, CLR::WHITE, 2);
#pragma endregion
#pragma region Default_Boards
	size = this->m_pText->GetSize("Default Boards", 0.51f);
	rect = GRID::kGrid[8][8];
	rect.x += slong((GRID::kSize.w + GRID::kSpace.w) * 0.5f);
	rect.w = 4 * (GRID::kSize.w + GRID::kSpace.w) - GRID::kSpace.w;
	this->m_pText->Write("Default Boards", rect, CLR::LAVENDER, 0.51f, true, Text::EFontAlignment::F_CENTER);
	this->m_pSprite->DrawLine(rect.x, rect.y, rect.x + rect.w, rect.y, CLR::WHITE, 2);
#pragma endregion
#pragma region Custom_Boards
	size = this->m_pText->GetSize("Custom Boards", 0.53f);
	rect = GRID::kGrid[13][8];
	rect.w = 4 * (GRID::kSize.w + GRID::kSpace.w) - GRID::kSpace.w;
	this->m_pText->Write("Custom Boards", rect, CLR::LAVENDER, 0.53f, true, Text::EFontAlignment::F_CENTER);
	this->m_pSprite->DrawLine(rect.x, rect.y, rect.x + rect.w, rect.y, CLR::WHITE, 2);
#pragma endregion
#pragma region Board_Name_Text
	//Board Name
	{
		rect = this->m_cMapNameButton.GetRect();
		Type2<slong> offset = this->m_cMapNameButton.GetTextOffset();
		_COLOR<uchar> clr = this->m_cMapNameButton.GetTextColor(this->m_cMapNameButton.GetState());

		slong marker = this->m_pInput->Text_Get_Marker();
		if (marker == this->m_cMapNameButton.GetText().length() || !this->m_cMapNameButton.GetToggle())
		{
			//Text
			this->m_cMapNameButton.Render(this->m_pSprite, this->m_pText);
			//Marker
			if (this->m_bMarkerVisible && this->m_cMapNameButton.GetActive())
			{
				size = this->m_pText->GetSize(this->m_cMapNameButton.GetText().c_str(), this->m_cMapNameButton.GetTextScale());
				rect.x += offset.w + size.w;
				this->m_pText->Write("_", rect, clr, this->m_cMapNameButton.GetTextScale(), true, Text::EFontAlignment::F_LEFT);
			}
		}
		else
		{
			//Text
			std::string mapname = this->m_cMapNameButton.GetText();
			this->m_cMapNameButton.SetText("");
			std::string alpha, omega;
			alpha = omega = mapname;
			alpha.erase(alpha.begin() + marker, alpha.end());
			omega.erase(omega.begin(), omega.begin() + marker);

			this->m_cMapNameButton.Render(this->m_pSprite, this->m_pText);
			this->m_cMapNameButton.SetText(mapname);

			rect.x += offset.w;
			this->m_pText->Write(alpha.c_str(), rect, clr, this->m_cMapNameButton.GetTextScale(), true, Text::EFontAlignment::F_LEFT); //Alpha Word
			rect.x += this->m_pText->GetSize(alpha.c_str(), this->m_cMapNameButton.GetTextScale(), false).w;
			//if (this->m_bMarkerVisible && this->m_cMapNameButton.GetActive()) //Marker
			this->m_pText->Write("_", rect, clr, this->m_cMapNameButton.GetTextScale(), true, Text::EFontAlignment::F_LEFT);
			rect.x += this->m_pText->GetSize("_", this->m_cMapNameButton.GetTextScale(), true).w;
			this->m_pText->Write(omega.c_str(), rect, clr, this->m_cMapNameButton.GetTextScale(), true, Text::EFontAlignment::F_LEFT); //Omega Word
		}
	}
#pragma endregion
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
		this->m_pSprite->DrawLine(p0, p1, clr, 3);
		this->m_pSprite->DrawLine(p1, p2, clr, 3);
		this->m_pSprite->DrawLine(p2, p0, clr, 3);

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
		this->m_pSprite->DrawLine(p0, p1, clr, 3);
		this->m_pSprite->DrawLine(p1, p2, clr, 3);
		this->m_pSprite->DrawLine(p2, p0, clr, 3);
	}
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
void CForgeState::RenderGrid(void)
{
	Type2<ulong> size = this->m_pText->GetSize("0", 1.0f);

	for (ulong x = 0; x < Board::kslBoardSize; x++)
	{
		for (ulong y = 0; y < Board::kslBoardSize; y++)
		{
			CPanel panel = this->m_cBoard.GetPanel(uchar(x), uchar(y));
			_RECT<slong> rect = this->m_vGrid[x][y].GetRect();
			if (!panel.GetActive())
			{
				this->m_pSprite->DrawRect(rect, CLR::GREY);
				rect.y -= (slong)(size.y * 0.22f);
				this->m_pText->Write("X", rect, CLR::CHARCOAL, 1.0f, true, Text::F_CENTER);
			}
			else
			{
				auto vec = panel.GetSlots();
				if (!vec.empty() && vec[0] != -1)
					this->m_pSprite->DrawRect(rect, kutPlayerColors[vec[0]]);
				else
					this->m_pSprite->DrawRect(rect, CLR::CHARCOAL);
				rect.y -= (slong)(size.y * 0.22f);
				this->m_pText->Write(std::to_string(vec.size()).c_str(), rect, CLR::GREY, 1.0f, true, Text::F_CENTER);
			}
			this->m_vGrid[x][y].Render(this->m_pSprite, this->m_pText);
		}
	}
}
//Saving & Loading
void CForgeState::LoadDefault(void)
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
void CForgeState::SaveDefault(void)
{
}
void CForgeState::LoadCustom(void)
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
void CForgeState::SaveCutsom(void)
{
	std::ofstream fout;
	fout.open("CustomBoard.bin", std::ios_base::out | std::ios_base::binary);
	if (fout.is_open())
	{
		//Number of boards
		ulong size = this->m_vCustomList.size();
		fout.write((const char*)(&size), sizeof(ulong));
		for (ulong i = 0; i < size; i++)
			this->m_vCustomList[i].SaveBoard(fout);
	}
}
//Active Buttons
void CForgeState::DetermineActiveButtons(void)
{
	//Set All to True
	for (ulong i = 0; i < NUM_BUTTON_GROUPS; i++)
		this->m_vButtonGroup[i].SetActive(true);

	//Now figure out which ones to turn off
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
	//If any default map is selected, then all are off except for 'New' & 'Copy' & 'Back'
	if (this->m_slDefaultIndex != -1)
	{
		//Set all to false
		for (ulong i = 0; i < NUM_BUTTON_GROUPS; i++)
			this->m_vButtonGroup[i].SetActive(false);
		for (ulong i = 0; i < NUM_LIST_BUTS; i++)
		{
			this->m_vDefaultListButtons[i].SetActive(true);
			this->m_vCustomListButtons[i].SetActive(true);
		}
		this->m_vMapButtons[NEW].SetActive(true);
		this->m_vMapButtons[COPY].SetActive(true);
		this->m_vButtons[BACK].SetActive(true);

	}
	//if any custom map is selected, then all are on
	else
	{

	}

	//Set Default List Buttons
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
	//Set Custom List Buttons
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


	//Set the board
	this->SetBoard();

	//Set the map name text
	if (this->m_bDefault && this->m_slDefaultIndex != -1 && !this->m_vDefaultList.empty())
		this->m_cMapNameButton.SetText(this->m_vDefaultList[this->m_slDefaultIndex].GetMapName());
	else if (this->m_slCustomIndex != -1 && !this->m_vCustomList.empty())
		this->m_cMapNameButton.SetText(this->m_vCustomList[this->m_slCustomIndex].GetMapName());
	else
		this->m_cMapNameButton.SetText("");
}
void CForgeState::SetBoard(void)
{
	if (this->m_bDefault)
	{
		if (this->m_slDefaultIndex == -1 || this->m_vDefaultList.empty())
			return;
		this->m_cBoard = this->m_vDefaultList[this->m_slDefaultIndex];
	}
	else
	{
		if (this->m_slCustomIndex == -1 || this->m_vCustomList.empty())
			return;
		this->m_cBoard = this->m_vCustomList[this->m_slCustomIndex];
	}
}
void CForgeState::GetBoard(void)
{
	//if (this->m_bDefault)
	//this->m_vDefaultList[this->m_slDefaultIndex] = this->m_cBoard;
	//else
	if (!this->m_bDefault)
		this->m_vCustomList[this->m_slCustomIndex] = this->m_cBoard;
}
void CForgeState::SortBoards(void)
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
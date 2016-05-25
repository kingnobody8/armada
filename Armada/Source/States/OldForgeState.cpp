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

CForgeState::CForgeState(void)
{
	this->m_eType = EGameStateType::gs_MAIN_MENU;
	this->m_eSelectedSlot = ESlotButtons::NO_SBUT;
	this->m_eSelectedPlayer = EPlayerButtons::NO_PBUT;
	this->m_eCurrButtonSect = EButtonSects::PRIMARY;
}
CForgeState::~CForgeState(void)
{
}

void CForgeState::Enter(Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText)
{
	IGameState::Enter(pSprite, pInput, pText);

	//Ensure the player has mouse control
	this->m_pInput->Cursor_Lock(false);

	//Setup the board
	//this->m_cBoard.SetDimensions(Type2<slong>(32, 32), Type2<slong>(16, 16), Type2<slong>(16, 16), Type2<slong>(16, 16));
	//this->m_cBoard.SetWinAmount(3);
	//
	////Setup the Amount
	//{
	//	Type2<ulong> size = this->m_pText->GetSize("W", 1.0f);
	//	this->m_tAmount.SetRect(_RECT<slong>(1536, 64, size.x, size.y));
	//	this->m_tAmount.SetText(std::to_string(this->m_cBoard.GetWinAmount()).c_str());
	//	this->m_tAmount.SetTextColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
	//	this->m_tAmount.SetTextAlignment(Text::EFontAlignment::F_LEFT);
	//}
	//
	////Setup the primary buttons
	//for (ulong i = 0; i < NUM_BUTTONS; i++)
	//{
	//	m_vButtons[i].SetTextColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
	//	m_vButtons[i].SetTextColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
	//	m_vButtons[i].SetTextColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
	//	//m_vButtons[i].SetAlignment(Text::EFontAlignment::F_LEFT);
	//}
	//Type2<ulong> tSize;
	////Back
	//tSize = this->m_pText->GetSize("Back", 1.0f);
	//m_vButtons[BACK].SetText("Back");
	//m_vButtons[BACK].SetRect(_RECT<slong>(Sprite::SCREEN_WIDTH - tSize.x, 384, tSize.x, tSize.y));
	//
	////Setup the Slot buttons
	//for (ulong i = 0; i < NUM_SLOT_BUTTONS; i++)
	//{
	//	Type2<slong> index = Type2<slong>(i % 3, i / 3);
	//	Type2<ulong> size = this->m_pText->GetSize("0", 1.0f);
	//	_RECT<slong> board = this->m_cBoard.GetRect();
	//	_RECT<slong> rect = this->m_cBoard.GetPanel(0, 0).GetButton().GetRect();
	//	rect.x = board.w + 64 + index.x * (rect.w + 16);
	//	rect.y = 768 - index.y * (rect.h + 16);
	//	this->m_vSlotButtons[i].SetClickType(GUI::CButton::EClickType::PUSH);
	//	this->m_vSlotButtons[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::DEFAULT);
	//	this->m_vSlotButtons[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::HOVER);
	//	this->m_vSlotButtons[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::MDOWN);
	//	this->m_vSlotButtons[i].SetStateColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
	//	this->m_vSlotButtons[i].SetStateColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
	//	this->m_vSlotButtons[i].SetStateColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
	//	this->m_vSlotButtons[i].SetRect(rect);
	//	this->m_vSlotButtons[i].SetText(std::to_string(i));
	//	this->m_vSlotButtons[i].SetOffset(Type2<slong>(0, -(slong)(size.h * 0.2f)));
	//	this->m_vSlotButtons[i].SetOutlineSize(4);
	//	//this->m_vSlotButtons[i].SetActive(false);
	//}
	//this->m_vSlotButtons[OFF].SetText("X");
	//
	//
	//
	//
	////Setup Map Name (Not Button)
	//{
	//	_RECT<slong> rect = this->m_cBoard.GetRect();
	//	Type2<ulong> size = this->m_pText->GetSize("Map Name:", 0.5f);
	//	rect.y = size.h + rect.h;
	//	rect.h += size.h;
	//	rect.w = size.w;
	//	this->m_tMapName.SetText("Map Name:");
	//	this->m_tMapName.SetTextScale(0.5f);
	//	this->m_tMapName.SetRect(rect);
	//	this->m_tMapName.SetTextColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
	//	this->m_tMapName.SetTextAlignment(Text::EFontAlignment::F_LEFT);
	//}
	////Setup Panels (Not Button)
	//{
	//	_RECT<slong> rect = this->m_cBoard.GetRect();
	//	Type2<ulong> size = this->m_pText->GetSize("Panels", 0.75f);
	//	rect.x += rect.w + 64;
	//	rect.y += rect.h - size.h;
	//	rect.h = size.h;
	//	rect.w = size.w;
	//	this->m_tPanels.SetText("Panels");
	//	this->m_tPanels.SetTextScale(0.75f);
	//	this->m_tPanels.SetRect(rect);
	//	this->m_tPanels.SetTextColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
	//	this->m_tPanels.SetTextAlignment(Text::EFontAlignment::F_LEFT);
	//}
	////Setup Spawns (Not Button)
	//{
	//	_RECT<slong> rect = this->m_tPanels.GetRect();
	//	Type2<ulong> size = this->m_pText->GetSize("Spawns", 0.75f);
	//	rect.y -= rect.h + size.h + (2 * this->m_cBoard.GetPanel(0, 0).GetButton().GetRect().h);
	//	rect.h = size.h;
	//	rect.w = size.w;
	//	this->m_tSpawns.SetText("Spawns");
	//	this->m_tSpawns.SetTextScale(0.75f);
	//	this->m_tSpawns.SetRect(rect);
	//	this->m_tSpawns.SetTextColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
	//	this->m_tSpawns.SetTextAlignment(Text::EFontAlignment::F_LEFT);
	//}
	////Setup the Player Buttons
	//for (ulong i = 0; i < NUM_PLAYER_BUTTONS; i++)
	//{
	//	Type2<slong> index = Type2<slong>(i % 2, i / 2);
	//	Type2<ulong> size = this->m_pText->GetSize("0", 1.0f);
	//	_RECT<slong> board = this->m_cBoard.GetRect();
	//	_RECT<slong> rect = this->m_cBoard.GetPanel(0, 0).GetButton().GetRect();
	//	rect.x = board.w + 64 + index.x * (rect.w + 16) + (slong)(rect.w * 0.5f);
	//	rect.y = this->m_tSpawns.GetRect().y - rect.h - index.y * (rect.h + 16);
	//	this->m_vPlayerButtons[i].SetClickType(GUI::CButton::EClickType::PUSH);
	//	this->m_vPlayerButtons[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::DEFAULT);
	//	this->m_vPlayerButtons[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::HOVER);
	//	this->m_vPlayerButtons[i].SetTextColor(CLR::DARK_GREY, GUI::CButton::EButtonState::MDOWN);
	//	this->m_vPlayerButtons[i].SetStateColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
	//	this->m_vPlayerButtons[i].SetStateColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
	//	this->m_vPlayerButtons[i].SetStateColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
	//	this->m_vPlayerButtons[i].SetRect(rect);
	//	this->m_vPlayerButtons[i].SetText(std::string("P") + std::to_string(i + 1));
	//	this->m_vPlayerButtons[i].SetTextScale(0.5f);
	//	this->m_vPlayerButtons[i].SetOffset(Type2<slong>(0, -(slong)(size.h * 0.01f)));
	//	this->m_vPlayerButtons[i].SetOutlineSize(4);
	//}


}
void CForgeState::Exit(void)
{
	IGameState::Exit();
}

void CForgeState::Input(void)
{
	if (this->m_pInput->Ctrl_Disconnect())
		this->m_pInput->Cursor_Lock(false);

	if (this->m_pInput->Ctrl_PluggedIn())
	{
		this->m_pInput->Cursor_Lock(true);
		bool confirm_selection = false;
#pragma region KEYBOARD
		switch (this->m_eCurrButtonSect)
		{
		case PRIMARY:
			if (this->m_pInput->Key_Push(VK_LEFT))
			{
				this->m_eCurrButtonSect = EButtonSects::PANEL;
				//this->m_eSelectedSlot = 
			}
			break;
		}
#pragma endregion
#pragma region CTRL
#pragma endregion
	}
	else
	{
#pragma region MOUSE
		Type2<slong> cursor = this->m_pInput->Cursor_SDL_PosScreen();
		bool bHeld = this->m_pInput->Key_Held(VK_LBUTTON);
		bool bPush = this->m_pInput->Key_Push(VK_LBUTTON);
		bool bPull = this->m_pInput->Key_Pull(VK_LBUTTON);

		//Board Input
		this->m_cBoard.Input(this->m_pInput);

		//Primary Buttons
		{
			EForgeButtons eBut = NUM_BUTTONS;
			for (slong i = 0; i < NUM_BUTTONS; i++)
			//if (this->m_vButtons[i].Input(this->m_pInput))
				//eBut = EForgeButtons(i);
			//GOTO next state
			switch (eBut)
			{
			case BACK:
				this->m_tResult.Prev(EGameStateType::gs_MAIN_MENU);
				break;
			default:
				break;
			}
		}

		//Slot Buttons
		{
			for (slong i = 0; i < NUM_SLOT_BUTTONS && this->m_eSelectedSlot == NO_SBUT; i++)
			{
				//if (this->m_vSlotButtons[i].Input(this->m_pInput))
				//{
				//	this->m_eSelectedSlot = ESlotButtons(i);
				//	this->m_pInput->Cursor_Set_Visible(false);
				//	break;
				//}
			}
			bool bActive = (this->m_eSelectedSlot == NO_SBUT && this->m_eSelectedPlayer == NO_PBUT);
			for (slong i = 0; i < NUM_SLOT_BUTTONS; i++)
			{
				if (i == this->m_eSelectedSlot)
					continue;
				this->m_vSlotButtons[i].SetActive(bActive);
			}
			for (slong i = 0; i < NUM_PLAYER_BUTTONS; i++)
			{
				if (i == this->m_eSelectedPlayer)
					continue;
				this->m_vPlayerButtons[i].SetActive(bActive);
			}
			if (this->m_eSelectedSlot != NO_SBUT && bPull)
			{
				for (uchar x = 0; x < Board::kslBoardSize; x++)
				{
					for (uchar y = 0; y < Board::kslBoardSize; y++)
					{
						_RECT<slong> but_rect = this->m_cBoard.GetPanel(x, y).GetButton().GetRect();
						if (Collision::Intersect_Type2_AABB<slong>(cursor, Collision::tAABB<slong>(but_rect)))
						{
							Board::CPanel cPanel = this->m_cBoard.GetPanel(x, y);
							std::vector<schar> vSlots = cPanel.GetSlots();
							if (this->m_eSelectedSlot != OFF)
							{
								schar id = -1;
								if (!vSlots.empty())
									id = vSlots[0];
								vSlots.clear();
								for (uchar i = 0; i < this->m_eSelectedSlot; i++)
								{
									if (i == 0)
										vSlots.push_back(id);
									else
										vSlots.push_back(-1);
								}
								cPanel.SetActive(true);
							}
							else
							{
								vSlots.clear();
								cPanel.SetActive(false);
							}
							cPanel.SetSlots(vSlots);
							this->m_cBoard.SetPanel(cPanel, x, y);
						}
					}
				}
				this->m_eSelectedSlot = NO_SBUT;
				this->m_pInput->Cursor_Set_Visible(true);
			}
		}
		//Player Buttons
		{
			for (slong i = 0; i < NUM_PLAYER_BUTTONS && this->m_eSelectedPlayer == NO_PBUT; i++)
			{
				//if (this->m_vPlayerButtons[i].Input(this->m_pInput))
				//{
				//	this->m_eSelectedPlayer = EPlayerButtons(i);
				//	this->m_pInput->Cursor_Set_Visible(false);
				//	break;
				//}
			}
			bool bActive = (this->m_eSelectedSlot == NO_SBUT && this->m_eSelectedPlayer == NO_PBUT);
			for (slong i = 0; i < NUM_SLOT_BUTTONS; i++)
			{
				if (i == this->m_eSelectedSlot)
					continue;
				this->m_vSlotButtons[i].SetActive(bActive);
			}
			for (slong i = 0; i < NUM_PLAYER_BUTTONS; i++)
			{
				if (i == this->m_eSelectedPlayer)
					continue;
				this->m_vPlayerButtons[i].SetActive(bActive);
			}
			if (this->m_eSelectedPlayer != NO_PBUT && bPull)
			{
				for (uchar x = 0; x < Board::kslBoardSize; x++)
				{
					for (uchar y = 0; y < Board::kslBoardSize; y++)
					{
						_RECT<slong> but_rect = this->m_cBoard.GetPanel(x, y).GetButton().GetRect();
						if (Collision::Intersect_Type2_AABB<slong>(cursor, Collision::tAABB<slong>(but_rect)))
						{
							Board::CPanel cPanel = this->m_cBoard.GetPanel(x, y);
							std::vector<schar> vSlots = cPanel.GetSlots();
							if (cPanel.GetActive() && !vSlots.empty())
							{
								//Remove the spawn from any other panels
								{
									for (uchar j = 0; j < Board::kslBoardSize; j++)
									{
										for (uchar k = 0; k < Board::kslBoardSize; k++)
										{
											Board::CPanel cPanel = this->m_cBoard.GetPanel(j, k);
											std::vector<schar> vSlots = cPanel.GetSlots();
											if (!vSlots.empty() && vSlots[0] == this->m_eSelectedPlayer)
											{
												vSlots[0] = -1;
												cPanel.SetSlots(vSlots);
												this->m_cBoard.SetPanel(cPanel, j, k);
											}
										}
									}
								}

								vSlots[0] = this->m_eSelectedPlayer;
								cPanel.SetSlots(vSlots);
								this->m_cBoard.SetPanel(cPanel, x, y);
							}
						}
					}
				}
				this->m_eSelectedPlayer = NO_PBUT;
				this->m_pInput->Cursor_Set_Visible(true);
			}
		}

#pragma endregion
	}
}
void CForgeState::Update(Time delta, Time gameTime)
{
	this->m_pStarField->Update(delta);
	this->m_cBoard.Update(delta);
}
void CForgeState::Render(void)
{
	this->m_pStarField->Render();

	for (ulong i = 0; i < NUM_BUTTONS; i++)
		this->m_vButtons[i].Render(m_pSprite, m_pText);
	for (slong i = 0; i < NUM_SLOT_BUTTONS; i++)
	{
		this->m_vSlotButtons[i].Render(m_pSprite, m_pText);
		//this->m_pSprite->DrawRect(this->m_vSlotButtons[i].GetRect(), CLR::WHITE, false, 4);
	}
	for (slong i = 0; i < NUM_PLAYER_BUTTONS; i++)
	{
		this->m_pSprite->DrawRect(this->m_vPlayerButtons[i].GetRect(), Board::kutPlayerColors[i]);
		this->m_vPlayerButtons[i].Render(m_pSprite, m_pText);
	}

	//Map Name
	this->m_tMapName.Render(this->m_pSprite, this->m_pText);
	//Panel
	this->m_tPanels.Render(this->m_pSprite, this->m_pText);
	//Spawns
	this->m_tSpawns.Render(this->m_pSprite, this->m_pText);

	m_cBoard.Render(this->m_pSprite, this->m_pText);


	//If we are drag and dropping a panel
	if (this->m_eSelectedSlot != NO_SBUT)
	{
		float scale = this->m_vSlotButtons[this->m_eSelectedSlot].GetTextScale();
		Type2<slong> tPos = this->m_pInput->Cursor_SDL_PosScreen();
		_RECT<slong> rect = this->m_cBoard.GetPanel(0, 0).GetButton().GetRect();
		Type2<ulong> text_size = this->m_pText->GetSize("0", scale);

		rect.x = tPos.x - (slong)(rect.w * 0.5f);
		rect.y = tPos.y - (slong)(rect.h * 0.5f);

		this->m_pSprite->DrawRect(rect, CLR::CRIMSON, false, this->m_vSlotButtons[this->m_eSelectedSlot].GetOutlineSize());
		rect.y += this->m_vSlotButtons[this->m_eSelectedSlot].GetOffset().h;
		//(slong)(text_size.h * scale * 0.2f);
		this->m_pText->Write(this->m_vSlotButtons[this->m_eSelectedSlot].GetText().c_str(), rect, CLR::CRIMSON, scale, true, Text::EFontAlignment::F_CENTER);
	}
	if (this->m_eSelectedPlayer != NO_PBUT)
	{
		float scale = this->m_vPlayerButtons[this->m_eSelectedPlayer].GetTextScale();
		Type2<slong> tPos = this->m_pInput->Cursor_SDL_PosScreen();
		_RECT<slong> rect = this->m_cBoard.GetPanel(0, 0).GetButton().GetRect();
		Type2<ulong> text_size = this->m_pText->GetSize("0", scale);

		rect.x = tPos.x - (slong)(rect.w * 0.5f);
		rect.y = tPos.y - (slong)(rect.h * 0.5f);

		this->m_pSprite->DrawRect(rect, Board::kutPlayerColors[this->m_eSelectedPlayer], true);// , this->m_vPlayerButtons[this->m_eSelectedPlayer].GetOutlineSize());
		rect.y += this->m_vPlayerButtons[this->m_eSelectedPlayer].GetOffset().h;
		//(slong)(text_size.h * scale * 0.2f);
		this->m_pText->Write(this->m_vPlayerButtons[this->m_eSelectedPlayer].GetText().c_str(), rect, this->m_vPlayerButtons[this->m_eSelectedPlayer].GetTextColor(GUI::CButton::EButtonState::DEFAULT), scale, true, Text::EFontAlignment::F_CENTER);
	}

	this->m_tAmount.Render(this->m_pSprite, this->m_pText);

}
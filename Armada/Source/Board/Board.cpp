#include "Board.h"
#include "../SDL/Sprite.h"
#include "../Input/InputCMD.h"
#include "../Network/NetMsg.h"
using namespace Input;
using namespace Network;

namespace Board
{
	CBoard::CBoard(void)
	{
		//For each button
		for (int x = 0; x < kslBoardSize; x++)
		{
			for (int y = 0; y < kslBoardSize; y++)
			{
				//this->m_vButtons[x][y].SetStateColor(CLR::GOLDENROD, GUI::CButton::EButtonState::DEFAULT);
				//this->m_vButtons[x][y].SetStateColor(CLR::CRIMSON, GUI::CButton::EButtonState::HOVER);
				//this->m_vButtons[x][y].SetStateColor(CLR::CYAN, GUI::CButton::EButtonState::MDOWN);
			}
		}
		this->m_scWinAmount = -1;
		this->m_bDefault = false;
	}
	CBoard::~CBoard(void)
	{

	}

	/*virtual*/ void CBoard::Input(Input::CInput* pInput)
	{
		//For each panel
		for (int x = 0; x < kslBoardSize; x++)
		{
			for (int y = 0; y < kslBoardSize; y++)
			{
				this->m_vPanel[x][y].Input(pInput);
			}
		}
	}
	/*virtual*/ void CBoard::Update(Time dDelta)
	{
		//For each panel
		for (int x = 0; x < kslBoardSize; x++)
		{
			for (int y = 0; y < kslBoardSize; y++)
			{
				this->m_vPanel[x][y].Update(dDelta);
			}
		}
	}
	/*virtual*/ void CBoard::Render(Sprite::CSprite* pSprite, Text::CText* pText)
	{
		//For each panel
		//for (int x = 0; x < kslBoardSize; x++)
		//{
		//	for (int y = 0; y < kslBoardSize; y++)
		//	{
		//		if (this->m_vPanel[x][y].GetActive())
		//		{
		//			if (this->m_vPanel[x][y].GetSlots().empty() || this->m_vPanel[x][y].GetSlots()[0] == -1)
		//				pSprite->DrawRect(this->m_vPanel[x][y].GetRect(), CLR::CHARCOAL);
		//			else
		//				pSprite->DrawRect(this->m_vPanel[x][y].GetRect(), kutPlayerColors[this->m_vPanel[x][y].GetSlots()[0]]);
		//		}
		//		this->m_vPanel[x][y].Render(pSprite, pText);
		//	}
		//}

		pSprite->DrawRect(this->m_tRect, CLR::DARK_GREY, false, 4);
	}

	void CBoard::SetDimensions(Type2<slong> offset, Type2<slong> tile_size, Type2<slong> tile_space, Type2<slong> panel_space)
	{
		Type2<slong> panel_size = (tile_size + tile_space) * 3;

		this->m_tRect = _RECT<slong>(offset.x, offset.y,
			kslBoardSize * (panel_size.x + panel_space.x) + panel_space.x,
			kslBoardSize * (panel_size.y + panel_space.y) + panel_space.y);

		offset += panel_space;

		//For each panel
		for (int x = 0; x < kslBoardSize; x++)
		{
			for (int y = 0; y < kslBoardSize; y++)
			{
				Type2<slong> panel_offset = offset + panel_size * Type2<slong>(x, y) + panel_space * Type2<slong>(x, y);
				//For each tile in each panel
				//for (int i = 0; i < kusGridSize; i++)
				//{
				//	GUI::COutlineButton tBut = this->m_vPanel[x][y].GetGrid(i).GetButton();
				//	Type2<slong> pos = panel_offset + Type2<slong>((slong)(tile_space.x * 0.5f), (slong)(tile_space.y * 0.5f))
				//		+ Type2<slong>((i % 3)*(tile_size.x + tile_space.x), (i / 3)*(tile_size.y + tile_space.y));
				//	_RECT<slong> rect = _RECT<slong>(pos.x, pos.y, tile_size.x, tile_size.y);
				//	tBut.SetRect(rect);
				//	this->m_vPanel[x][y].GetGrid(i).SetButton(tBut);
				//}

				//Set the panel button as a whole
				_RECT<slong> rect = _RECT<slong>(panel_offset.x, panel_offset.y, panel_size.x, panel_size.y);
				//	this->m_vPanel[x][y].SetRect(rect);
				//this->m_vButtons[x][y].SetRect(rect);
			}
		}
	}
	void CBoard::LoadBoard(std::ifstream& fin)
	{
		//Bool - Default
		fin.read((char*)(&this->m_bDefault), sizeof(bool));
		//Schar - Win Amount
		fin.read((char*)(&this->m_scWinAmount), sizeof(schar));
		//std::string - MapName
		ulong len = this->m_strMapName.length();
		fin.read((char*)(&len), sizeof(ulong));
		char* c = new char[len + 1];
		fin.read((char*)(c), len);
		c[len] = 0;
		this->m_strMapName = c;
		delete[]c;
		//_RECT<slong> - Rect
		fin.read((char*)(&this->m_tRect), sizeof(_RECT<slong>));
		//Panel - Grid
		for (ulong x = 0; x < kslBoardSize; ++x)
		{
			for (ulong y = 0; y < kslBoardSize; ++y)
			{
				this->m_vPanel[x][y].LoadPanel(fin);
			}
		}
	}
	ulong CBoard::LoadBoard(const char* pBuffer)
	{
		ulong offset = 0;
		//Bool - Default
		this->m_bDefault = *(bool*)(pBuffer + offset);
		offset += sizeof(bool);
		//Schar - Win Amount
		this->m_scWinAmount = *(schar*)(pBuffer + offset);
		offset += sizeof(schar);
		//std::string - MapName
		ulong len = *(ulong*)(pBuffer + offset);
		offset += sizeof(ulong);
		this->m_strMapName = (char*)(pBuffer + offset);
		offset += this->m_strMapName.length();
		//_RECT<slong> - Rect
		this->m_tRect = *(_RECT<slong>*)(pBuffer + offset);
		offset += sizeof(_RECT<slong>);
		//Panel - Grid
		for (ulong x = 0; x < kslBoardSize; ++x)
		{
			for (ulong y = 0; y < kslBoardSize; ++y)
			{
				offset += this->m_vPanel[x][y].LoadPanel(pBuffer + offset);
			}
		}
		return offset;
	}
	void CBoard::SaveBoard(std::ofstream& fout)
	{
		//Bool - Default
		fout.write((const char*)(&this->m_bDefault), sizeof(bool));
		//Schar - Win Amount
		fout.write((const char*)(&this->m_scWinAmount), sizeof(schar));
		//std::string - MapName
		ulong len = this->m_strMapName.length();
		fout.write((const char*)(&len), sizeof(ulong));
		fout.write((const char*)(this->m_strMapName.c_str()), len);
		//_RECT<slong> - Rect
		fout.write((const char*)(&this->m_tRect), sizeof(_RECT<slong>));
		//Panel - Grid
		for (ulong x = 0; x < kslBoardSize; ++x)
		{
			for (ulong y = 0; y < kslBoardSize; ++y)
			{
				this->m_vPanel[x][y].SavePanel(fout);
			}
		}
	}

	void CBoard::FillMsg(Network::CNetMsg* pMsg)
	{
		//Bool - Default
		pMsg->WriteBool(this->m_bDefault);
		//Schar - Win Amount
		pMsg->WriteByte(this->m_scWinAmount);
		//std::string - MapName
		pMsg->WriteString(this->m_strMapName.c_str());
		//_RECT<slong> - Rect
		pMsg->WriteRaw((const char*)(&this->m_tRect), sizeof(_RECT<slong>));
		//Panel - Grid
		for (ulong x = 0; x < kslBoardSize; ++x)
		{
			for (ulong y = 0; y < kslBoardSize; ++y)
			{
				this->m_vPanel[x][y].FillMsg(pMsg);
			}
		}
	}
	void CBoard::LoadMsg(Network::CNetMsg* pMsg)
	{
		//Bool - Default
		this->m_bDefault = pMsg->ReadBool();
		//Schar - Win Amount
		this->m_scWinAmount = pMsg->ReadByte();
		//std::string - MapName
		this->m_strMapName = pMsg->ReadString();
		//_RECT<slong> - Rect
		pMsg->ReadRaw((char*)(&this->m_tRect), sizeof(_RECT<slong>));
		//Panel - Grid
		for (ulong x = 0; x < kslBoardSize; ++x)
		{
			for (ulong y = 0; y < kslBoardSize; ++y)
			{
				this->m_vPanel[x][y].LoadMsg(pMsg);
			}
		}
	}

}
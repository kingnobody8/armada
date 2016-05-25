#include "Panel.h"

#include "../SDL/Sprite.h"
using namespace Sprite;
#include "../Text/Text.h"
using namespace Text;
#include "../Input/InputCMD.h"
using namespace Input;
#include "../Network/NetMsg.h"
using namespace Network;

namespace Board
{
#pragma region TILE
	CTile::CTile(void)
	{
		this->m_scID = -1;
	}
	CTile::~CTile(void)
	{

	}

	void CTile::Update(Time dDelta)
	{

	}
	void CTile::Render(Sprite::CSprite* pSprite, Text::CText* pText)
	{
	}
#pragma endregion
#pragma region PANEL
	CPanel::CPanel(void)
	{
		this->m_bActive = false;			//Default is off
	}
	CPanel::~CPanel(void)
	{

	}

	void CPanel::Input(Input::CInput* pInput)
	{
		
	}
	void CPanel::Update(Time dDelta)
	{
		
	}
	void CPanel::Render(Sprite::CSprite* pSprite, Text::CText* pText)
	{

	}

	ulong CPanel::LoadPanel(const char* pBuffer)
	{
		ulong offset = 0;
		//Bool - Active
		this->m_bActive = *(bool*)(pBuffer + offset);
		offset += sizeof(bool);
		//schar - Grid
		for (schar i = 0; i < kucMaxNumQSlots; ++i)
		{
			schar id = *(schar*)(pBuffer + offset);
			offset += sizeof(schar);
			this->m_vGrid[i].SetID(id);
		}
		//schar - Slots
		ulong size = *(ulong*)(pBuffer + offset);
		offset += sizeof(ulong);
		for (ulong i = 0; i < size; ++i)
		{
			schar slot = *(schar*)(pBuffer + offset);
			offset += sizeof(schar);
			this->m_vSlots.push_back(slot);
		}
		return offset;
	}
	void CPanel::LoadPanel(std::ifstream &fin)
	{
		//Bool - Active
		fin.read((char*)(&this->m_bActive), sizeof(bool));
		//schar - Grid
		for (schar i = 0; i < kucMaxNumQSlots; ++i)
		{
			schar id = this->m_vGrid[i].GetID();
			fin.read((char*)(&id), sizeof(schar));
			this->m_vGrid[i].SetID(id);
		}
		//schar - Slots
		ulong size = this->m_vSlots.size();
		fin.read((char*)(&size), sizeof(ulong));
		for (ulong i = 0; i < size; ++i)
		{
			schar slot = -1;
			fin.read((char*)(&slot), sizeof(schar));
			this->m_vSlots.push_back(slot);
		}
	}
	void CPanel::SavePanel(std::ofstream &fout)
	{
		//Bool - Active
		fout.write((const char*)(&this->m_bActive), sizeof(bool));
		//schar - Grid
		for (schar i = 0; i < kucMaxNumQSlots; ++i)
		{
			schar id = this->m_vGrid[i].GetID();
			fout.write((const char*)(&id), sizeof(schar));
		}
		//schar - Slots
		ulong size = this->m_vSlots.size();
		fout.write((const char*)(&size), sizeof(ulong));
		for (ulong i = 0; i < size; ++i)
			fout.write((const char*)(&this->m_vSlots[i]), sizeof(schar));
	}

	void CPanel::FillMsg(Network::CNetMsg* pMsg)
	{
		//Bool - Active
		pMsg->WriteBool(this->m_bActive);
		//schar - Grid
		for (schar i = 0; i < kucMaxNumQSlots; ++i)
		{
			schar id = this->m_vGrid[i].GetID();
			pMsg->WriteByte(id);
		}
		//schar - Slots
		ulong size = this->m_vSlots.size();
		pMsg->WriteLong(size);
		for (ulong i = 0; i < size; ++i)
			pMsg->WriteByte(this->m_vSlots[i]);
	}
	void CPanel::LoadMsg(Network::CNetMsg* pMsg)
	{
		//Bool - Active
		this->m_bActive = pMsg->ReadBool();
		//schar - Grid
		for (schar i = 0; i < kucMaxNumQSlots; ++i)
			this->m_vGrid[i].SetID(pMsg->ReadByte());
		//schar - Slots
		ulong size = pMsg->ReadLong();
		this->m_vSlots.clear();
		for (ulong i = 0; i < size; ++i)
			this->m_vSlots.push_back(pMsg->ReadByte());
	}


	//void CPanel::Input_Play(Input::CInput* pInput)
	//{
	//	//for (ulong i = 0; i < kusGridSize; i++)
	//		//this->m_vGrid[i].Input(pInput);
	//}
	//void CPanel::Input_Edit(Input::CInput* pInput)
	//{
	//	this->m_bLClick = false;
	//	Type2<slong> tMLoc = pInput->Cursor_SDL_PosScreen();
	//	bool bLHeld = pInput->Key_Held(VK_LBUTTON);
	//	bool bLPull = pInput->Key_Pull(VK_LBUTTON);
	//	bool bRHeld = pInput->Key_Held(VK_RBUTTON) && !bLHeld;
	//	bool bRPull = pInput->Key_Pull(VK_RBUTTON) && !bLHeld;

	//	//------------------------------------
	//	if (true)//this->m_tButton.Input(pInput))
	//	{
	//		this->m_bLClick = bLHeld;
	//		if (bRPull)
	//		{
	//			this->m_bActive = false;
	//			this->m_vSlots.clear();
	//		}
	//	}

	//	//if (this->m_tButton.Input(tMLoc, bLButton || bRButton || bMButton))
	//	//{
	//		//if (bLPull)
	//		//{
	//		//	if (!this->m_bActive)
	//		//		this->m_bActive = true;
	//		//	else if (this->m_vSlots.size() == kucMaxNumQSlots)
	//		//	{
	//		//		this->m_vSlots.clear();
	//		//		this->m_bActive = false;
	//		//	}
	//		//	else
	//		//		this->m_vSlots.push_back(-1);
	//		//}
	//		//else if (bRPull)
	//		//{
	//		//	if (!this->m_bActive)
	//		//	{
	//		//		for (uchar i = 0; i < kucMaxNumQSlots; i++)
	//		//			this->m_vSlots.push_back(-1);
	//		//		this->m_bActive = true;
	//		//	}
	//		//	else if (this->m_vSlots.empty())
	//		//		this->m_bActive = false;
	//		//	else
	//		//		this->m_vSlots.erase(this->m_vSlots.begin());
	//		//}
	//		//else if (bMPull)
	//		//{
	//		//	this->m_bActive = !this->m_bActive;
	//		//}
	////	}

	//	//if (this->m_tButton.Update(tMLoc, bLButton))
	//	//{
	//	//	 if (this->m_bActive)
	//	//	 {
	//	//		 if (this->m_vSlots.size() == kucMaxNumQSlots)
	//	//		 {
	//	//			 this->m_vSlots.clear();
	//	//			 this->m_bActive = false;
	//	//		 }
	//	//		 else
	//	//			 this->m_vSlots.push_back(-1);
	//	//	 }
	//	//	 else
	//	//	 {
	//	//		 this->m_bActive = true;
	//	//	 }
	//	//}

	//	//this->m_tButton.Update(tMLoc, false);
	//	//if (this->m_tButton.GetState() == GUI::CButton::EButtonState::HOVER)
	//	//{
	//	//	 if (bLButton && !bRButton)
	//	//	 {
	//	//		 if (this->m_tButton.Update(tMLoc, bLButton))
	//	//		 {
	//	//			 if (this->m_bActive)
	//	//			 {
	//	//				 if (this->m_vSlots.size() == kucMaxNumQSlots)
	//	//					 this->m_bActive = false;
	//	//				 else
	//	//					 this->m_vSlots.push_back(-1);
	//	//			 }
	//	//			 else
	//	//			 {
	//	//				 this->m_bActive = true;
	//	//			 }
	//	//		 }
	//	//	 }
	//	//	 else if (bRButton && !bLButton)
	//	//	 {
	//	//		 if (this->m_tButton.Update(tMLoc, bRButton))
	//	//		 {
	//	//			 if (this->m_bActive)
	//	//			 {
	//	//				 if (this->m_vSlots.empty())
	//	//					 this->m_bActive = false;
	//	//				 else
	//	//					 this->m_vSlots.erase(this->m_vSlots.begin());
	//	//			 }
	//	//			 else
	//	//			 {
	//	//				 this->m_bActive = true;
	//	//			 }
	//	//		 }
	//	//	 }
	//	//}
	//}
	//void CPanel::Update_Play(Time dDelta)
	//{

	//}
	//void CPanel::Update_Edit(Time dDelta)
	//{

	//}
	//void CPanel::Render_Play(Sprite::CSprite* pSprite, Text::CText* pText)
	//{
	//	for (ulong i = 0; i < kusGridSize; i++)
	//		this->m_vGrid[i].Render(pSprite, pText);
	//}
	//void CPanel::Render_Edit(Sprite::CSprite* pSprite, Text::CText* pText)
	//{
	//	this->m_tButton.Render(pSprite, pText);
	//	_COLOR<uchar> clr = CLR::DARK_GREY;
	//	if (this->m_bActive)
	//	{
	//		_RECT<slong> but_rect = this->m_tButton.GetRect();
	//		Type2<ulong> text_size = pText->GetSize("0", 1.0f);
	//		float scale = 0.75f * (float)text_size.h / (float)but_rect.h;
	//		but_rect.y -= (slong)(text_size.h * scale * 0.25f);
	//		pText->Write(std::to_string(this->m_vSlots.size()).c_str(), but_rect, clr, scale, true, Text::EFontAlignment::F_CENTER);
	//	}
	//	else 
	//	{
	//		if (this->m_tButton.GetState() == GUI::CButton::EButtonState::HOVER || this->m_tButton.GetState() == GUI::CButton::EButtonState::MDOWN)
	//			clr.a = 64;

	//		_RECT<slong> but_rect = this->m_tButton.GetRect();
	//		for (slong i = 0; i < this->m_tButton.GetOutlineSize(); i++)
	//		{
	//			pSprite->DrawRect(but_rect, CLR::DARK_GREY, false);
	//			but_rect.x++;
	//			but_rect.y++;
	//			but_rect.w -= 2;
	//			but_rect.h -= 2;
	//		}
	//		Type2<ulong> text_size = pText->GetSize("X", 1.0f);
	//		float scale = 0.75f * (float)text_size.h / (float)but_rect.h;
	//		but_rect.y -= (slong)(text_size.h * scale * 0.25f);
	//		pText->Write("X", but_rect, clr, scale, true, Text::EFontAlignment::F_CENTER);
	//	}
	//}
#pragma endregion
}
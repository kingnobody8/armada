#include "Button.h"
#include "../Audio/Audio.h"
#include "../Util/Collision.h"
using namespace Collision;

namespace GUI
{
	CButton::CButton(void)
	{
		this->m_bActive = true;
		this->m_eState = this->m_ePrevState = EButtonState::DEFAULT;
		this->m_eAlignment = Text::EFontAlignment::F_CENTER;
		this->m_fTextScale = 1.0f;
		this->m_bMouseSelection = false;
		for (schar i = 0; i < EButtonState::NUM_STATES; i++)
		{
			this->m_vClrText[i] = CLR::RED;
			this->m_vClrBG[i] = CLR::GREEN;
			this->m_vClrOutline[i] = CLR::BLUE;
			this->m_slOutlineSize[i] = 4;
		}
		for (schar i = 0; i < eDIRECTION::NUM_CARDINAL_DIRECTIONS; i++)
			this->m_vButtonGroupDir[i] = -1;
		for (schar i = 0; i < NUM_SFX; ++i)
			this->m_vSfx[i] = -1;
	}
	CButton::~CButton(void)
	{

	}

	void CButton::DefaultColor(_COLOR<uchar> clr)
	{
		for (schar i = 0; i < EButtonState::NUM_STATES; i++)
			this->m_vClrText[i] = this->m_vClrBG[i] = this->m_vClrOutline[i] = clr;
	}

	void CButton::Selection(Type2<slong> tCursor)
	{
		//Early outs
		if (!this->m_bActive)
			return;

		//Are we hovering
		this->m_eState = EButtonState::DEFAULT;
		if (Intersect_Type2_AABB<slong>(tCursor, tAABB<slong>(this->m_tRect)))
			this->m_eState = EButtonState::HOVER;

		this->m_bMouseSelection = true;
	}
	void CButton::Selection(bool bSelected)
	{
		//Early outs
		if (!this->m_bActive)
			return;

		//Are we hovering
		this->m_eState = EButtonState::DEFAULT;
		if (bSelected)
			this->m_eState = EButtonState::HOVER;

		this->m_bMouseSelection = false;
	}
	Input::_Key CButton::Confirmation(bool bDown)
	{
		//Early outs
		if (!this->m_bActive)
		{
			this->m_tKey = Input::_Key();
			return this->m_tKey;
		}

		//If we are hovering
		if (this->m_eState == EButtonState::HOVER)
		{
			//Update the key
			this->m_tKey.Next(bDown);
			this->m_tKey.Flush(0.0f);

			//Update the state
			if (this->m_tKey.Held())
				this->m_eState = EButtonState::MDOWN;
		}
		else
		{
			this->m_tKey = Input::_Key();
		}

		return this->m_tKey;
	}

	void CButton::Update(Time dDelta, Audio::CAudio* pAudio)
	{
		if (pAudio != NULL)
		{
			if (!(this->m_tKey.Push() || this->m_tKey.Pull()) &&
				(this->m_eState == HOVER || this->m_eState == ON_HOVER || this->m_eState == OFF_HOVER) &&
				(this->m_ePrevState != HOVER && this->m_ePrevState != ON_HOVER && this->m_ePrevState != OFF_HOVER))
				pAudio->Play(this->m_vSfx[SFX_HOVER], false, this->m_bMouseSelection);
			else if (this->m_tKey.Push())
				pAudio->Play(this->m_vSfx[SFX_PUSH]);
			else if (this->m_tKey.Pull())
				pAudio->Play(this->m_vSfx[SFX_PULL]);
		}
		this->m_ePrevState = this->m_eState;
		this->m_bMouseSelection = false;
	}
	void CButton::Render(Sprite::CSprite* pSprite, Text::CText* pText)
	{
		_RECT<slong> temp = this->m_tRect;
		temp.x += this->m_tOffset.x;
		temp.y += this->m_tOffset.y;

		EButtonState eState = (this->m_bActive) ? this->m_eState : EButtonState::INACTIVE;

		//Draw BG
		pSprite->DrawRect(this->m_tRect, this->m_vClrBG[eState]);
		//Draw Outline
		pSprite->DrawRect(this->m_tRect, this->m_vClrOutline[eState], false, this->m_slOutlineSize[eState]);
		//Write Text
		if (!this->m_strText.empty())
			pText->Write(this->m_strText.c_str(), temp, this->m_vClrText[eState], this->m_fTextScale, true, this->m_eAlignment);
	}
}
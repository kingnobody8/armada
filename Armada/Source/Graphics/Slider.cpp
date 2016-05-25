#include "Slider.h"
#include "../Util/Collision.h"

namespace GUI
{
	CSlider::CSlider(void)
	{
		this->m_fPercent = 1.0f;
		this->m_bUsedMouse = false;
		this->m_eOrientation = EOrientation::HORZ;
		for (schar i = 0; i < EButtonState::NUM_STATES; i++)
		{
			this->m_vClrSlider[i] = CLR::BLACK;
			this->m_vClrLine[i] = CLR::GREY;
			this->m_vClrBar[i] = CLR::WHITE;
			this->m_vSliderSize[i] = 4;
			this->m_vLineSize[i] = 4;
		}
	}
	CSlider::~CSlider(void)
	{

	}

	/*virtual*/ void CSlider::Selection(Type2<slong> tCursor)
	{
		CButton::Selection(tCursor);
		m_tCursor = tCursor;
		this->m_bUsedMouse = true;
	}
	/*virtual*/ void CSlider::Selection(bool bSelected)
	{
		CButton::Selection(bSelected);
		m_tCursor = Type2<slong>(-1, -1);
		this->m_bUsedMouse = false;
	}
	/*virtual*/ Input::_Key CSlider::Confirmation(bool bDown)
	{
		if (!this->m_bUsedMouse)
			CButton::Confirmation(bDown);
		else if (bDown && Collision::Intersect_Type2_AABB(this->m_tCursor, Collision::tAABB<slong>(this->m_tRect)))
		{
			CButton::Confirmation(bDown);
			EButtonState eState = (this->m_bActive) ? this->m_eState : EButtonState::INACTIVE;
			Type2<slong> pos[NUM_CARDINAL_DIRECTIONS];
			Type2<slong> offset = Type2<slong>(this->m_slOutlineSize[eState], this->m_slOutlineSize[eState]);
			pos[LEFT]	= Type2<slong>(this->m_tRect.x + offset.x,						slong(this->m_tRect.y + this->m_tRect.h * 0.5f));
			pos[RIGHT]	= Type2<slong>(this->m_tRect.x + this->m_tRect.w - offset.x,	slong(this->m_tRect.y + this->m_tRect.h * 0.5f));
			pos[UP]		= Type2<slong>(slong(this->m_tRect.x + this->m_tRect.w * 0.5f), this->m_tRect.y + this->m_tRect.h - offset.y);
			pos[DOWN]	= Type2<slong>(slong(this->m_tRect.x + this->m_tRect.w * 0.5f), this->m_tRect.y + offset.y);
			switch (this->m_eOrientation)
			{
			case EOrientation::VERT:
				this->m_fPercent = (this->m_tCursor.y - pos[DOWN].y) / float(pos[UP].y - pos[DOWN].y);
				break;
			case EOrientation::HORZ:
				this->m_fPercent = (this->m_tCursor.x - pos[LEFT].x) / float(pos[RIGHT].x - pos[LEFT].x);
				break;
			}
		}
		return this->m_tKey;
	}
	/*virtual*/ void CSlider::Update(Time dDelta, Audio::CAudio* pAudio)
	{
		CButton::Update(dDelta, pAudio);
		this->m_fPercent = clamp<float>(this->m_fPercent, 0.0f, 1.0f);
	}

	/*virtual*/ void CSlider::Render(Sprite::CSprite* pSprite, Text::CText* pText)
	{
		EButtonState eState = (this->m_bActive) ? this->m_eState : EButtonState::INACTIVE;
		Type2<slong> pos[NUM_CARDINAL_DIRECTIONS];
		Type2<slong> alpha, omega;
		Type2<slong> offset = Type2<slong>(this->m_slOutlineSize[eState], this->m_slOutlineSize[eState]);
		pos[LEFT]	= Type2<slong>(this->m_tRect.x + offset.x,						slong(this->m_tRect.y + this->m_tRect.h * 0.5f));
		pos[RIGHT]	= Type2<slong>(this->m_tRect.x + this->m_tRect.w - offset.x,	slong(this->m_tRect.y + this->m_tRect.h * 0.5f));
		pos[UP]		= Type2<slong>(slong(this->m_tRect.x + this->m_tRect.w * 0.5f), this->m_tRect.y + this->m_tRect.h - offset.y);
		pos[DOWN]	= Type2<slong>(slong(this->m_tRect.x + this->m_tRect.w * 0.5f), this->m_tRect.y + offset.y);
		offset = Type2<slong>(this->m_vSliderSize[eState], this->m_vSliderSize[eState]);
			//Draw BG
			pSprite->DrawRect(this->m_tRect, this->m_vClrBG[eState]);
			//Draw Slider Stuff
			switch (this->m_eOrientation)
			{
			case VERT:
			{
						 slong diff = slong((pos[UP].y - pos[DOWN].y) * this->m_fPercent);
						 alpha = Type2<slong>(pos[LEFT].x, this->m_tRect.y + offset.y + diff);
						 omega = Type2<slong>(pos[RIGHT].x, this->m_tRect.y + offset.y + diff);
						 //Line
						 pSprite->DrawLine(pos[DOWN], pos[UP], this->m_vClrLine[eState], this->m_vLineSize[eState]);
						 //Bar
						 pSprite->DrawRect(_RECT<slong>(pos[LEFT].x, pos[DOWN].y, pos[RIGHT].x - pos[LEFT].x, diff), this->m_vClrBar[eState]);
						 //Slider
						 pSprite->DrawLine(alpha, omega, this->m_vClrSlider[eState], this->m_vSliderSize[eState]);
						 break;
			}
			case HORZ:
			{
						 slong diff = slong((pos[RIGHT].x - pos[LEFT].x) * this->m_fPercent);
						 alpha = Type2<slong>(this->m_tRect.x + offset.x + diff, pos[UP].y);
						 omega = Type2<slong>(this->m_tRect.x + offset.x + diff, pos[DOWN].y);
						 //Line
						 pSprite->DrawLine(pos[LEFT], pos[RIGHT], this->m_vClrLine[eState], this->m_vLineSize[eState]);
						 //Bar
						 pSprite->DrawRect(_RECT<slong>(pos[LEFT].x, pos[DOWN].y, diff, pos[UP].y - pos[DOWN].y), this->m_vClrBar[eState]);
						 //Slider
						 pSprite->DrawLine(alpha, omega, this->m_vClrSlider[eState], this->m_vSliderSize[eState]);
						 break;
			}
			}
			//Draw Outline
			pSprite->DrawRect(this->m_tRect, this->m_vClrOutline[eState], false, this->m_slOutlineSize[eState]);
			//Write Text
			if (!this->m_strText.empty())
				pText->Write(this->m_strText.c_str(), _RECT<slong>(this->m_tRect.x + this->m_tOffset.x, this->m_tRect.y + this->m_tOffset.y, this->m_tRect.w, this->m_tRect.h), 
				this->m_vClrText[eState], this->m_fTextScale, true, this->m_eAlignment);
	}
	/*virtual*/ void CSlider::DefaultColor(_COLOR<uchar> clr)
	{
		CButton::DefaultColor(clr);
		for (ulong i = 0; i < CButton::EButtonState::NUM_STATES; i++)
			this->m_vClrSlider[i] = this->m_vClrLine[i] = this->m_vClrBar[i] = clr;
	}
}
#include "ButtonGroup.h"
using namespace Input;

namespace GUI
{
	CButtonGroup::CButtonGroup(void)
	{
		this->m_slSelectedIndex = 0;
		this->m_eMoveType = CButton::EMoveType::PREVIOUS;
		this->m_dCtrlStickTimer = -1.0f;
		this->m_fStickThreshold = 0.35f;
		this->m_pInput = NULL;
		this->m_bFindClosestButtonGroup = false;
		for (slong i = 0; i < NUM_CARDINAL_DIRECTIONS; i++)
			this->m_vButtonGroupDir[i] = -1;
	}
	CButtonGroup::~CButtonGroup(void)
	{

	}

	slong CButtonGroup::Selection(Input::eInputType eType)
	{
		if (!this->m_pInput)
			return -1;

		eDIRECTION eDir = eDIRECTION::NUM_CARDINAL_DIRECTIONS;

		switch (eType)
		{
		case Input::eInputType::MOUSE:
		{
#pragma region MOUSE
										 ulong size = this->m_vButtons.size();
										 for (ulong i = 0; i < size; i++)
											 this->m_vButtons[i]->Selection(m_pInput->Cursor_SDL());
										 return -1;
#pragma endregion
										 break;
		}
		case Input::eInputType::CTRL_KEYBOARD:
		{

#pragma region KEYBOARD
												 //Find the direction
												 if (this->m_pInput->Key_Push(VK_UP))
													 eDir = UP;
												 else if (this->m_pInput->Key_Push(VK_DOWN))
													 eDir = DOWN;
												 else if (this->m_pInput->Key_Push(VK_LEFT))
													 eDir = LEFT;
												 else if (this->m_pInput->Key_Push(VK_RIGHT))
													 eDir = RIGHT;

												 //Handle the direction
												 if (eDir != eDIRECTION::NUM_CARDINAL_DIRECTIONS)
													 break;
#pragma endregion
#pragma region CTRL
												 //D-Pad selection
												 if (this->m_pInput->Ctrl_Push(CTRL_KEYS::d_UP))
													 eDir = UP;
												 else if (this->m_pInput->Ctrl_Push(CTRL_KEYS::d_DOWN))
													 eDir = DOWN;
												 else if (this->m_pInput->Ctrl_Push(CTRL_KEYS::d_LEFT))
													 eDir = LEFT;
												 else if (this->m_pInput->Ctrl_Push(CTRL_KEYS::d_RIGHT))
													 eDir = RIGHT;

												 //Handle the direction
												 if (eDir != eDIRECTION::NUM_CARDINAL_DIRECTIONS)
													 break;

												 //Stick selection
												 VecComp lThumb = m_pInput->Ctrl_Stick(CTRL_KEYS::XB_LTHUMB);
												 if (lThumb.mag >= this->m_fStickThreshold && this->m_dCtrlStickTimer < 0.0f)
												 {
													 //Y Axis
													 if (abs(lThumb.dir.y) > abs(lThumb.dir.x))
													 {
														 if (lThumb.dir.y > 0.0f)
															 eDir = UP;
														 else if (lThumb.dir.y < 0.0f)
															 eDir = DOWN;
													 }
													 else //X Axis
													 {
														 if (lThumb.dir.x < 0.0f)
															 eDir = LEFT;
														 else if (lThumb.dir.x > 0.0f)
															 eDir = RIGHT;
													 }
													 this->m_dCtrlStickTimer = kCtrlStickRate;
												 }

												 //Handle the direction
												 if (eDir != eDIRECTION::NUM_CARDINAL_DIRECTIONS)
													 break;
#pragma endregion
												 break;
		}
		}

		//Handle the direction
		slong index = this->Move(eDir, this->m_slSelectedIndex);
		if (index == -1)
			return eDir;
		this->m_slSelectedIndex = index;

		ulong size = this->m_vButtons.size();
		for (ulong i = 0; i < size; i++)
			this->m_vButtons[i]->Selection(i == this->m_slSelectedIndex);

		return -1;
	}
	void CButtonGroup::Confirmation(bool bConfirm)
	{
		ulong size = this->m_vButtons.size();
		for (ulong i = 0; i < size; i++)
			this->m_vButtons[i]->Confirmation(bConfirm);
	}
	void CButtonGroup::Update(Time dDelta, Audio::CAudio* pAudio)
	{
		if (!this->m_pInput)
			return;

		VecComp lThumb = this->m_pInput->Ctrl_Stick(CTRL_KEYS::XB_LTHUMB);
		if (this->m_pInput->Ctrl_Push(CTRL_KEYS::XB_LTHUMB))
			this->m_dCtrlStickTimer = kCtrlStickRate;
		else if (lThumb.mag >= this->m_fStickThreshold)
			this->m_dCtrlStickTimer -= dDelta;
		else
			this->m_dCtrlStickTimer = -1.0f;

		ulong size = this->m_vButtons.size();
		for (ulong i = 0; i < size; i++)
			this->m_vButtons[i]->Update(dDelta, pAudio);
	}
	void CButtonGroup::Render(Sprite::CSprite* pSprite, Text::CText* pText)
	{
		ulong size = this->m_vButtons.size();
		for (ulong i = 0; i < size; i++)
			this->m_vButtons[i]->Render(pSprite, pText);
	}

	slong CButtonGroup::Move(eDIRECTION eDir, slong slIndex)
	{
		//Early Outs
		if (eDir == eDIRECTION::NUM_CARDINAL_DIRECTIONS)
			return slIndex;
		if ((eDir == eDIRECTION::UP || eDir == eDIRECTION::DOWN) && !this->m_bMove.y)
			return -1;
		if ((eDir == eDIRECTION::LEFT || eDir == eDIRECTION::RIGHT) && !this->m_bMove.x)
			return -1;
		if (this->m_vButtons[slIndex]->GetButtonGroupDir(eDir) == -2)
			return slIndex;
		

		float distance = -1.0f;
		slong ret = -1;
		bool active = false;

		_RECT<slong> rect = this->m_vButtons[slIndex]->GetRect();

		ulong size = this->m_vButtons.size();
		for (ulong i = 0; i < size; i++)
		{
			if (i == slIndex)
				continue;

			_RECT<slong> tempR = this->m_vButtons[i]->GetRect();
			Type2<float> between = Type2<float>(float(tempR.GetPos().x - rect.GetPos().x), float(tempR.GetPos().y - rect.GetPos().y));

			if (between.magnitude() < distance || ret == -1)
			{
				bool bConfirm = false;
				switch (eDir)
				{
				case UP:
					bConfirm = between.y > 0.0f;
					break;
				case DOWN:
					bConfirm = between.y < 0.0f;
					break;
				case LEFT:
					bConfirm = between.x < 0.0f;
					break;
				case RIGHT:
					bConfirm = between.x > 0.0f;
					break;
				}
				if (bConfirm)
				{
					distance = between.magnitude();
					ret = i;
					active = this->m_vButtons[i]->GetActive();
				}
			}
		}

		//If no new index, then test for wrapping around
		if (ret == -1)
		{
			Type2<float> dist = Type2<float>(10000.0f, 10000.0f);
			if (eDir == UP || eDir == DOWN)
				dist.y = 0.0f;
			else if (eDir == LEFT || eDir == RIGHT)
				dist.x = 0.0f;

			for (ulong i = 0; i < size; i++)
			{
				if (i == slIndex)
					continue;

				_RECT<slong> tempR = this->m_vButtons[i]->GetRect();
				Type2<float> between = Type2<float>(float(tempR.GetPos().x - rect.GetPos().x), float(tempR.GetPos().y - rect.GetPos().y));

				bool bConfirm = false;
				switch (eDir)
				{
				case UP:
					bConfirm = between.y < 0.0f && abs(between.x) <= dist.x && abs(between.y) >= dist.y && this->m_bWrap.y;
					break;
				case DOWN:
					bConfirm = between.y > 0.0f && abs(between.x) <= dist.x && abs(between.y) >= dist.y && this->m_bWrap.y;
					break;
				case LEFT:
					bConfirm = between.x > 0.0f && abs(between.y) <= dist.y && abs(between.x) >= dist.x && this->m_bWrap.x;
					break;
				case RIGHT:
					bConfirm = between.x < 0.0f && abs(between.y) <= dist.y && abs(between.x) >= dist.x && this->m_bWrap.x;
					break;
				}
				if (bConfirm)
				{
					dist.x = abs(between.x);
					dist.y = abs(between.y);
					ret = i;
					active = this->m_vButtons[i]->GetActive();
				}
			}
		}

		//If the button we landed on is not active, find that button's next button based on direction
		if (!active && ret != -1)
			ret = Move(eDir, ret);
		return ret;
	}
	void CButtonGroup::UnSelect(void)
	{
		if (!this->m_vButtons.empty())
			this->m_vButtons[this->m_slSelectedIndex]->Selection(false);
	}
	bool CButtonGroup::GetActive(void) const
	{
		ulong size = this->m_vButtons.size();
		for (ulong i = 0; i < size; i++)
		{
			if (this->m_vButtons[i]->GetActive())
				return true;
		}
		return false;
	}
	void CButtonGroup::SetActive(const bool b)
	{
		ulong size = this->m_vButtons.size();
		for (ulong i = 0; i < size; i++)
			this->m_vButtons[i]->SetActive(b);
	}
	void CButtonGroup::ClearInput(void)
	{
		ulong size = this->m_vButtons.size();
		for (ulong i = 0; i < size; i++)
		{
			this->m_vButtons[i]->Selection(false);
			this->m_vButtons[i]->Confirmation(false);
			this->m_vButtons[i]->Selection(false);
			this->m_vButtons[i]->Confirmation(false);
		}
	}

	bool CButtonGroup::GetAnyPush(void)
	{
		ulong size = this->m_vButtons.size();
		for (ulong i = 0; i < size; i++)
		{
			if (this->m_vButtons[i]->GetClick().Push())
				return true;
		}
		return false;
	}
	bool CButtonGroup::GeyAnyHeld(void)
	{
		ulong size = this->m_vButtons.size();
		for (ulong i = 0; i < size; i++)
		{
			if (this->m_vButtons[i]->GetClick().Held())
				return true;
		}
		return false;
	}
	bool CButtonGroup::GetAnyPull(void)
	{
		ulong size = this->m_vButtons.size();
		for (ulong i = 0; i < size; i++)
		{
			if (this->m_vButtons[i]->GetClick().Pull())
				return true;
		}
		return false;
	}
}
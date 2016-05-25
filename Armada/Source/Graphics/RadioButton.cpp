#include "RadioButton.h"

namespace GUI
{
	CRadioButton::CRadioButton(void)
	{
		this->m_bToggle = false;
		this->m_eRadioType = PULL;
	}
	CRadioButton::~CRadioButton(void)
	{

	}

	CButton::EButtonState CRadioButton::ParseStateToRadio(CButton::EButtonState eState) const
	{
		switch (eState)
		{
		case DEFAULT:
			eState = (this->m_bToggle) ? ON : OFF;
			break;
		case HOVER:
			eState = (this->m_bToggle) ? ON_HOVER : OFF_HOVER;
			break;
		case MDOWN:
			eState = (this->m_bToggle) ? ON_MDOWN : OFF_MDOWN;
			break;
		}
		return eState;
	}
	CButton::EButtonState CRadioButton::ParseRadioToState(CButton::EButtonState eState) const
	{
		switch (eState)
		{
		case ON:
		case OFF:
			eState = DEFAULT;
			break;
		case ON_HOVER:
		case OFF_HOVER:
			eState = HOVER;
			break;
		case ON_MDOWN:
		case OFF_MDOWN:
			eState = MDOWN;
			break;
		}
		return eState;
	}

	/*virtual*/ void CRadioButton::Selection(Type2<slong> tCursor)
	{
		this->m_eState = ParseRadioToState(this->m_eState);
		CButton::Selection(tCursor);
		this->m_eState = ParseStateToRadio(this->m_eState);
	}
	/*virtual*/ void CRadioButton::Selection(bool bSelected)
	{
		this->m_eState = ParseRadioToState(this->m_eState);
		CButton::Selection(bSelected);
		this->m_eState = ParseStateToRadio(this->m_eState);
	}
	/*virtual*/ Input::_Key CRadioButton::Confirmation(bool bDown)
	{
		this->m_eState = ParseRadioToState(this->m_eState);
		CButton::Confirmation(bDown);
		this->m_eState = ParseStateToRadio(this->m_eState);
		if ((this->m_eRadioType == PULL && this->m_tKey.Pull()) || (this->m_eRadioType == PUSH && this->m_tKey.Push()))
			this->m_bToggle = !this->m_bToggle;
		return this->m_tKey;
	}
}
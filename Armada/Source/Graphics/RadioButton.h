#pragma once
#include <string>
#include "Button.h"

namespace GUI
{
	class CRadioButton : public CButton
	{
	public:
		enum eRadioType { PUSH, PULL, NUM_RADIO_TYPES };
	private:	//Data
	protected:	//Data
		bool		m_bToggle;
		eRadioType	m_eRadioType;

	private:	//Methods
		EButtonState	ParseStateToRadio(EButtonState eState) const;
		EButtonState	ParseRadioToState(EButtonState eState) const;
	protected:	//Methods
	public:		//Methods
		CRadioButton(void);
		~CRadioButton(void);


		//Determines if the button is being 'hovered' over using a mouse
		virtual void Selection(Type2<slong> tCursor);
		//Determine if the button is being 'hovered' over using a keyboard or controller
		virtual void Selection(bool bSelected);
		//Determines if the button is being pressed
		virtual Input::_Key Confirmation(bool bDown);

		//Gets
		inline bool GetToggle(void) const { return this->m_bToggle; }
		inline eRadioType GetRadioType(void) const { return this->m_eRadioType; }
		//Sets
		inline void SetToggle(const bool bToggle) { this->m_bToggle = bToggle; }
		inline void SetRadioType(const eRadioType eType) { this->m_eRadioType = eType; }
	};
}
#pragma once
#include "Button.h"
#include <vector>
#include "../Input/InputCMD.h"

namespace GUI
{
	class CButtonGroup
	{
	private:	//Data
		std::vector<CButton*>	m_vButtons;
		slong					m_slSelectedIndex;
		Type2<bool>				m_bMove;				//If we can move in these directions
		Type2<bool>				m_bWrap;				//If no buttons are available in the selected direction, does it wrap around
		CButton::EMoveType		m_eMoveType;			//If coming here from another group, which button do we go to
		Time					m_dCtrlStickTimer;
		Input::CInput*			m_pInput;
		slong					m_vButtonGroupDir[Type::eDIRECTION::NUM_CARDINAL_DIRECTIONS];	//Which index of ButtonManager should it go to
		bool					m_bFindClosestButtonGroup;
		float					m_fStickThreshold;


	private:	//Methods
		slong	Move(eDIRECTION eDir, slong slIndex);
	public:		//Methods
		CButtonGroup(void);
		~CButtonGroup(void);

		//Returns -1 on successful move, otherwise returns EDIRECTION enum to specify what button group to move to next
		slong Selection(Input::eInputType eType);
		void Confirmation(bool bConfirm);
		void Update(Time dDelta, Audio::CAudio* pAudio);
		void Render(Sprite::CSprite* pSprite, Text::CText* pText);

		void ClearInput(void);
		void UnSelect(void);
		bool GetActive(void) const;
		void SetActive(const bool b);
		bool GetAnyPush(void);
		bool GeyAnyHeld(void);
		bool GetAnyPull(void);

		//Gets
		inline std::vector<CButton*>	GetButtonList				( void )									const	{ return this->m_vButtons; }
		inline slong					GetSelectedIndex			( void )									const	{ return this->m_slSelectedIndex; }
		inline Type2<bool>				GetMove						( void )									const	{ return this->m_bMove; }
		inline Type2<bool>				GetWrap						( void )									const	{ return this->m_bWrap; }
		inline CButton::EMoveType		GetMoveType					( void )									const	{ return this->m_eMoveType; }
		inline slong					GetButtonGroupDir			( const Type::eDIRECTION eDir)				const	{ return this->m_vButtonGroupDir[eDir]; }
		inline bool						GetFindClosestButtonGroup	( void )									const	{ return this->m_bFindClosestButtonGroup; }
		inline Time						GetCtrlStickTimer			( void )									const	{ return this->m_dCtrlStickTimer; }
		inline float					GetStickThreshold			( void )									const	{ return this->m_fStickThreshold; }
		
		//Sets
		inline void						SetButtonList				( const std::vector<CButton*> vButtons )			{ this->m_vButtons = vButtons; }
		inline void						SetSelectedIndex			( const slong slSelectedIndex )						{ this->m_slSelectedIndex = slSelectedIndex; }
		inline void						SetMove						( const Type2<bool> bMove )							{ this->m_bMove = bMove; }
		inline void						SetWrap						( const Type2<bool> bWrap )							{ this->m_bWrap = bWrap; }
		inline void						SetMoveType					( const CButton::EMoveType eMoveType )				{ this->m_eMoveType = eMoveType; }
		inline void						SetInputCMD					( Input::CInput* const pInput )						{ this->m_pInput = pInput; }
		inline void						SetButtonGroupDir			( const Type::eDIRECTION eDir, const slong slVal )	{ this->m_vButtonGroupDir[eDir] = slVal; }
		inline void						SetFindClosestButtonGroup	( const bool bFindClosestButtonGroup )				{ this->m_bFindClosestButtonGroup = bFindClosestButtonGroup; }
		inline void						SetCtrlStickTimer			( const Time dCtrlStickTimer )						{ this->m_dCtrlStickTimer = dCtrlStickTimer; }
		inline void						SetStickThreshold			( const float fStickThreshold )						{ this->m_fStickThreshold = fStickThreshold; }
	};
}
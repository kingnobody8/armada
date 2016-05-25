/*
Author:			Daniel Habig
Date:			11-15-13
File:			IntroState.h
Purpose:		gamestate that allows selection of sub-states
*/
#pragma once
#include "igamestate.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/RadioButton.h"
#include "../Board/Board.h"

class CForgeState : public IGameState
{
private:
	enum EForgeButtons	{ BACK, NUM_BUTTONS };
	enum ESlotButtons	{ S0, S1, S2, S3, S4, OFF, NUM_SLOT_BUTTONS, NO_SBUT };
	enum EPlayerButtons { P1, P2, P3, P4, NUM_PLAYER_BUTTONS, NO_PBUT };
	enum EButtonSects { PRIMARY, SLOT, PLAYER, PANEL, NUM_BUTTON_SECTS };
	enum EButtonAmount { INC, DEC, NUM_BUTTON_AMOUNT };

private:		//Data
	CStarField*				m_pStarField;
	GUI::CButton			m_vButtons[NUM_BUTTONS];
	GUI::COutlineButton		m_vSlotButtons[NUM_SLOT_BUTTONS];
	GUI::COutlineButton		m_vPlayerButtons[NUM_PLAYER_BUTTONS];
	GUI::COutlineButton		m_vAmountButtons[NUM_BUTTON_AMOUNT];
	GUI::CButton			m_tMapName;//Not a button, just used for words
	GUI::CButton			m_tPanels;//Not a button, just used for words
	GUI::CButton			m_tSpawns;//Not a button, just used for words
	GUI::CButton			m_tAmount;//Not a button, just used for words
	Board::CBoard			m_cBoard;

	//Type2<slong>		m_tMouseOffset;
	ESlotButtons		m_eSelectedSlot;
	EPlayerButtons		m_eSelectedPlayer;
	EButtonSects		m_eCurrButtonSect;

protected:		//Data
public:			//Data

private:		//Methods
protected:		//Methods
public:			//Methods
							CForgeState	( void );						// ctor
	virtual					~CForgeState	( void );						// dtor

	// IGameState Interface					  
	/*virtual*/ void		Enter		( Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText );	// load resources
	/*virtual*/ void		Exit		( void );																	// unload resources

	// Primary Funcs						  
	/*virtual*/ void		Input		( void );							// handle user input
	/*virtual*/ void		Update		( Time delta, Time gameTime );		// update game entities
	/*virtual*/ void		Render		( void );							// render game entities

	inline void SetStarField(CStarField* pStarField) { this->m_pStarField = pStarField; }
};



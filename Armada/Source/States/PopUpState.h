/*
Author:			Daniel Habig
Date:			11-15-13
File:			IntroState.h
Purpose:		gamestate that allows selection of sub-states
*/
#pragma once
#include "igamestate.h"

class IPopUpState : public IGameState
{
public:
	enum EPopUpType { pu_DEFAULT, pu_TIMED, pu_OK, pu_YESNO, pu_ERROR, NUM_POPUP_TYPES };

private:		//Data
protected:		//Data
	EPopUpType		m_ePopUpType;
	EGameStateType	m_eGotoType;
public:			//Data

private:		//Methods
protected:		//Methods
public:			//Methods
							IPopUpState(void);						// ctor
	virtual					~IPopUpState(void);						// dtor

	//Gets
	inline EGameStateType	GetGotoType		( void )				const	{ return this->m_eGotoType; }
	inline EPopUpType		GetPopUpType	( void )				const	{ return this->m_ePopUpType; }
	//Sets
	inline void				SetGotoType		( const EGameStateType eType )	{ this->m_eGotoType = eType; }
};



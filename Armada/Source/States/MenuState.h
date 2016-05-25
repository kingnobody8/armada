/*
Author:			Daniel Habig
Date:			11-15-13
File:			IntroState.h
Purpose:		gamestate that allows selection of sub-states
*/
#pragma once
#include "igamestate.h"
#include "../Graphics/Graphics.h"
#include "OptionState.h"
#include "PreLobbyState.h"
#include "ForgeState.h"

class CMenuState : public IGameState
{
private:
	enum EMenuButtons { PLAY, FORGE, OPTIONS, EXIT, NUM_BUTTONS };

private:		//Data
	CStarField*			m_pStarField;
	GUI::CButton		m_vButtons[NUM_BUTTONS];

	//States
	COptionState	m_cOptionState;
	CPreLobbyState	m_cPreLobbyState;
	CForgeState		m_cForgeState;
	Time			m_dCtrlStickTimer;

protected:		//Data
public:			//Data

private:		//Methods
protected:		//Methods
public:			//Methods
							CMenuState	( void );							// ctor
virtual						~CMenuState	( void );							// dtor

	// IGameState Interface					  
	/*virtual*/ void		Enter(Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText, Audio::CAudio* pAudio);		// load resources
	/*virtual*/ void		Exit		( void );															// unload resources

	// Primary Funcs						  
	/*virtual*/ void		Input		( void );							// handle user input
	/*virtual*/ void		Update		( Time delta, Time gameTime );		// update game entities
	/*virtual*/ void		Render		( void );							// render game entities

	inline void SetStarField(CStarField* pStarField) { this->m_pStarField = pStarField; }
};



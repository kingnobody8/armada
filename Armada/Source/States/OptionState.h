/*
Author:			Daniel Habig
Date:			11-15-13
File:			IntroState.h
Purpose:		gamestate that allows selection of sub-states
*/
#pragma once
#include "igamestate.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Button.h"
#include "OptionAVState.h"

class COptionState : public IGameState
{
private:
	enum EOptionButtons { AV, INPUT, RULES, CREDITS, BACK, NUM_BUTTONS };
	enum EButtonGroups	{ PRIMARY, NUM_BUTTON_GROUPS };

private:		//Data
	CStarField*					m_pStarField;
	GUI::CButton				m_vButtons[NUM_BUTTONS];
	GUI::CButtonGroup			m_vButtonGroup[NUM_BUTTON_GROUPS];

	COptionAVState				m_cOptionAVState;
protected:		//Data
public:			//Data

private:		//Methods
protected:		//Methods
public:			//Methods
							COptionState	( void );						// ctor
	virtual					~COptionState	( void );						// dtor

	// IGameState Interface					  
	/*virtual*/ void		Enter		( Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText, Audio::CAudio* pAudio  );	// load resources
	/*virtual*/ void		Exit		( void );																	// unload resources

	// Primary Funcs						  
	/*virtual*/ void		Input		( void );							// handle user input
	/*virtual*/ void		Update		( Time delta, Time gameTime );		// update game entities
	/*virtual*/ void		Render		( void );							// render game entities

	inline void SetStarField(CStarField* pStarField) { this->m_pStarField = pStarField; }
};



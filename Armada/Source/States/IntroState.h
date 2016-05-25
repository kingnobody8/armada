/*
Author:			Daniel Habig
Date:			11-15-13
File:			IntroState.h
Purpose:		gamestate that begins the game, shows logos, transitions to StartState
*/
#pragma once
#include "igamestate.h"
#include "../Graphics/Graphics.h"
#include "MenuState.h"


class CIntroState :	public IGameState
{
private:		//Data
	slong			m_vLogo[5];
	Time			time;
	Type2<float>	center_offset;
	CMenuState		m_cMenuState;
	CStarField		m_cStarField;

protected:		//Data
public:			//Data

private:		//Methods
protected:		//Methods
public:			//Methods
							CIntroState		( void );							// ctor
	virtual					~CIntroState	( void );							// dtor
											  
	// IGameState Interface					  
	/*virtual*/ void		Enter			( Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText, Audio::CAudio* pAudio  );		// load resources
	/*virtual*/ void		Exit			( void );							// unload resources
											  
	// Primary Funcs						  
	/*virtual*/ void		Input			( void );							// handle user input
	/*virtual*/ void		Update			( Time delta, Time gameTime );		// update game entities
	/*virtual*/ void		Render			( void );							// render game entities

	
};


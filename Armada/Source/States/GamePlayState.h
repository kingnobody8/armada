/*
Author:			Daniel Habig
Date:			11-15-13
File:			IntroState.h
Purpose:		gamestate that runs the main game loop
*/
#pragma once
#include "igamestate.h"

class CGamePlayState : public IGameState
{
private:		//Data
protected:		//Data
public:			//Data

private:		//Methods
protected:		//Methods
public:			//Methods
							CGamePlayState		( void );							// ctor
	virtual					~CGamePlayState		( void );							// dtor

	// IGameState Interface					  
	/*virtual*/ void		Enter				( Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText, Audio::CAudio* pAudio  );		// load resources
	/*virtual*/ void		Exit				( void );							// unload resources

	// Primary Funcs						  
	/*virtual*/ void		Input				( void );							// handle user input
	/*virtual*/ void		Update				( Time delta, Time gameTime );		// update game entities
	/*virtual*/ void		Render				( void );							// render game entities
};



/*
Author:			Daniel Habig
Date:			11-9-13
File:			IGameState.cpp
Purpose:		Abstract Base Class for Game States to inherit from.
*/
#include "IGameState.h"
#include "../State_Machine/Machine.h"

IGameState::IGameState(void)
{
	this->m_eType = gs_DEFAULT;
	this->m_pPrevState = NULL;
}
IGameState::~IGameState(void)
{
}

// load resources
void IGameState::Enter(Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText, Audio::CAudio* pAudio)
{
	this->m_pSprite = pSprite;
	this->m_pInput = pInput;
	this->m_pText = pText;
	this->m_pAudio = pAudio;
	this->ResetResult();
}
// unload resources
void IGameState::Exit ( void )	
{
}
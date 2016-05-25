/*
Author:			Daniel Habig
Date:			11-15-13
File:			IntroState.cpp
Purpose:		gamestate that allows selection of sub-states
*/
#include "PopUpState.h"

IPopUpState::IPopUpState(void)
{
	this->m_eType		= EGameStateType::gs_POPUP;
	this->m_ePopUpType	= EPopUpType::pu_DEFAULT;
	this->m_eGotoType	= EGameStateType::gs_DEFAULT;
}
IPopUpState::~IPopUpState(void)
{
}

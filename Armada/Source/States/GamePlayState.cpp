/*
Author:			Daniel Habig
Date:			11-15-13
File:			IntroState.cpp
Purpose:		gamestate that runs the main game loop
*/
#include "GamePlayState.h"

CGamePlayState::CGamePlayState(void)
{
	this->m_eType = EGameStateType::gs_GAMEPLAY;
}
CGamePlayState::~CGamePlayState(void)
{
}

void CGamePlayState::Enter(Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText, Audio::CAudio* pAudio )
{
	IGameState::Enter(pSprite, pInput, pText, pAudio);
}
void CGamePlayState::Exit(void)
{
	IGameState::Exit();
}

void CGamePlayState::Input(void)
{
}
void CGamePlayState::Update(Time delta, Time gameTime)
{
}
void CGamePlayState::Render(void)
{
}
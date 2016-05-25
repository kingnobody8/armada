/*
Author:			Daniel Habig
Date:			11-15-13
File:			IntroState.cpp
Purpose:		gamestate that allows selection of sub-states
*/
#include "PUTimedState.h"
#include "../SDL/Sprite.h"
#include "../Text/Text.h"
#include "../Input/InputCMD.h"
using namespace Input;

CPUTimedState::CPUTimedState(void)
{
	this->m_ePopUpType = EPopUpType::pu_TIMED;
	this->m_dMaxTime = this->m_dTimer = -1.0f;
	this->m_bBar = true;
	this->m_tBarClr = CLR::WHITE;
	this->m_slBarSize = 8;
}
CPUTimedState::~CPUTimedState(void)
{
}

/*virtual*/ void CPUTimedState::Enter(Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText, Audio::CAudio* pAudio)
{
	IGameState::Enter(pSprite, pInput, pText, pAudio);

	m_slFadeImg = this->m_pSprite->LoadTex("black.sdl");
}
/*virtual*/ void CPUTimedState::Exit(void)
{
	IGameState::Exit();
}

/*virtual*/ void CPUTimedState::Input(void)
{
	/* DO NOTHING */
}
/*virtual*/ void CPUTimedState::Update(Time delta, Time gameTime)
{
	//Update prev state
	this->m_pPrevState->Update(delta, gameTime);
	//Update our state
	this->m_cHeader.Update(delta, this->m_pAudio);
	if (this->m_dTimer > 0.0f)
		this->m_dTimer -= delta;
	//Goto Specified State
	if (this->m_dTimer < 0.0f)
		this->m_tResult.Prev(this->m_eGotoType);
}
/*virtual*/ void CPUTimedState::Render(void)
{
	//Calculate percent till GOTO state
	double percent = 1.0l - (this->m_dTimer / this->m_dMaxTime);

	//Render prev state
	this->m_pPrevState->Render();
	//Fade the prev state
	this->m_pSprite->DrawTex(this->m_slFadeImg, Type2<slong>(), Type2<float>(float(Sprite::SCREEN_WIDTH), float(Sprite::SCREEN_HEIGHT)),
		NULL, NULL, SDL_BLENDMODE_BLEND, _COLOR<uchar>(0, 0, 0, uchar(percent * 255)) );
	//Render our state
	this->m_cHeader.Render(this->m_pSprite, this->m_pText);

	if (this->m_dMaxTime != 0.0l)
	{
		if (this->m_bBar)
		{
			_RECT<slong> rect = this->m_cHeader.GetRect();
			rect.w = slong(rect.w * percent);
			rect.h = this->m_slBarSize;
			rect.y -= this->m_slBarSize;
			this->m_pSprite->DrawRect(rect, this->m_tBarClr);
		}
	}
}

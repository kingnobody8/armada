/*
Author:			Daniel Habig
Date:			11-15-13
File:			IntroState.cpp
Purpose:		gamestate that begins the game, shows logos, transitions to StartState
*/
#include "IntroState.h"
#include "../SDL/Sprite.h"
#include "../Paker/Paker.h"
#include "../Input/InputCMD.h"
#include "../Text/Text.h"
#include "OptionAVState.h"
using namespace Input;

#define INTRO 1
#define FULL_GAME 0

CIntroState::CIntroState(void)
{
	this->m_eType = EGameStateType::gs_INTRO;
}
CIntroState::~CIntroState(void)
{
}

void CIntroState::Enter(Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText, Audio::CAudio* pAudio)
{
	IGameState::Enter(pSprite, pInput, pText, pAudio);

	//Build StarField
	m_cStarField.Enter(pSprite);

	//Init Render Data
	time = 0.0f;
	center_offset = Type2<float>(0, Sprite::SCREEN_HEIGHT * 0.5f);
	for (uchar i = 0; i < 5; i++)
	{
		if (i == 2)
			continue;

		std::string file = "L";
		file += '0' + i;
		file += ".png";
		m_vLogo[i] = this->m_pSprite->LoadTex(file.c_str(), true);
	}

	//Setup the screen
	this->m_pSprite->SetBGColor(CLR::BLACK);
	this->m_pSprite->SetFullScreen(FULL_GAME);
	this->m_pInput->Cursor_Lock(FULL_GAME);
	//Load Settings - AV state loads on enter
	COptionAVState avState;
	avState.Enter(pSprite, pInput, pText, pAudio);
}
void CIntroState::Exit(void)
{
	IGameState::Exit();
}

void CIntroState::Input(void)
{
	Input::eInputType eType = this->m_pInput->GetFinalInputType();
	this->m_pInput->Cursor_Lock(eType != MOUSE && FULL_GAME);

	if (this->m_pInput->Key_Push('R'))
	{
		this->time = 0.0f;
		this->m_pInput->Cursor_Lock(FULL_GAME);
	}
	if (this->time > 7.5f)
		return;

#pragma region KEYBOARD
	if (this->m_pInput->Key_Push(VK_ANYKEY) && !this->m_pInput->Key_Held('R'))
	{
		if (this->time < 5.0f)
			this->time = 5.0f;
		else
			this->time = 7.5f;
	}
#pragma endregion
#pragma region CTRL
	if (!this->m_pInput->Ctrl_Held(PLUGIN))
		return;
	if (
		this->m_pInput->Ctrl_Push(CTRL_KEYS::XB_A) || this->m_pInput->Ctrl_Push(CTRL_KEYS::XB_B) ||
		this->m_pInput->Ctrl_Push(CTRL_KEYS::XB_X) || this->m_pInput->Ctrl_Push(CTRL_KEYS::XB_Y) ||
		this->m_pInput->Ctrl_Push(CTRL_KEYS::START) || this->m_pInput->Ctrl_Push(CTRL_KEYS::XB_BACK)
		)
	{
		if (this->time < 5.0f)
			this->time = 5.0f;
		else
			this->time = 8.0f;
	}
#pragma endregion
}
void CIntroState::Update(Time delta, Time gameTime)
{
	this->m_cStarField.Update(delta);

	time += delta;
	center_offset.y = clamp<float>((float)((Sprite::SCREEN_HEIGHT * 0.5f) - (time * 0.25f * Sprite::SCREEN_HEIGHT)), 0, Sprite::SCREEN_HEIGHT * 0.5f);

	if (time > 9.0f)
	{
		this->m_tResult.Push(&this->m_cMenuState);
		this->m_cMenuState.SetStarField(&this->m_cStarField);
	}
}
void CIntroState::Render(void)
{
	this->m_cStarField.Render();

	_COLOR<uchar> clr_red = CLR::WHITE;
	_COLOR<uchar> clr_blue = CLR::WHITE;
	_COLOR<uchar> clr_word = CLR::LIGHT_GREY;
	clr_blue.a = clamp<int>((int)((this->time - 4.0f) * 255), 0, 255);
	clr_word.a = clamp<int>((int)((this->time - 4.0f) * 255), 0, 255);

	if (time > 6.0f)
	{
		clr_red.a = clr_word.a = clr_blue.a = clamp<int>((int)((1.0f - (this->time - 7.5f)) * 255), 0, 255);
	}

	//Wheel in wheel
#if (INTRO == 1)
	{
		float s = 8.0f;
		Type2<float> scale = Type2<float>(s, s);
		Type2<slong> center = Type2<slong>(960, (slong)(540 - center_offset.y));
		Type2<slong> pos = center;
		slong offset = 0;
	
		//Layer 0
		offset = (slong)(24 * s);
		pos = center - Type2<slong>(offset, offset);
		if (time > 2.0f)
			this->m_pSprite->DrawTex(m_vLogo[0], pos, scale, NULL, NULL, SDL_BLENDMODE_BLEND, clr_blue);
	
		//Layer 1
		offset = (slong)(24 * s);
		pos = center - Type2<slong>(offset, offset);
		Sprite::TRotation rot;
		//rot.m_fAngle = 135.0f + 225 * clamp<float>((slong)((time - 2.0f) * 0.5f * 7) / 7.0f, 0, 1.0f); 
		rot.m_fAngle = 135.0f + 225 * clamp<float>((float)((time - 2.0f) * 0.5f), 0.0f, 1.0f);
		rot.m_bInPlace = true;
		this->m_pSprite->DrawTex(m_vLogo[1], pos, scale, NULL, &rot, SDL_BLENDMODE_BLEND, clr_red);
	
		//Layer 3
		offset = (slong)(8 * s);
		pos = center + Type2<slong>(offset, offset);
		if (time > 2.0f)
			this->m_pSprite->DrawTex(m_vLogo[3], pos, scale, NULL, NULL, SDL_BLENDMODE_BLEND, clr_blue);
	
		//Layer 4
		offset = (slong)(16 * s);
		pos = center + Type2<slong>(offset, offset);
		rot = Sprite::TRotation();
		//rot.m_fAngle = 135.0f - 135 * clamp<float>((slong)((time - 2.0f) * 0.5f * 13) / 13.0f, 0.0f, 1.0f);
		rot.m_fAngle = 135.0f - 135 * clamp<float>((float)((time - 2.0f) * 0.5f), 0.0f, 1.0f);
		rot.m_tCenterPoint = center;
		this->m_pSprite->DrawTex(m_vLogo[4], pos, scale, NULL, &rot, SDL_BLENDMODE_BLEND, clr_red);
	}

	this->m_pText->Write("STUDIO HABIG", _RECT<slong>(0, 200, Sprite::SCREEN_WIDTH, 200), clr_word, 1.0f, true, Text::EFontAlignment::F_CENTER);
#endif
	//Original
#if (INTRO == 0)
	{
		_COLOR<uchar> clr = CLR::WHITE;
		clr = clr_red;//clamp<int>((1.0f - m_dDelay) * (1.0f - m_dDelay) * 255, 0, 255);

		float s = 8.0f;
		Type2<float> scale = Type2<float>(s, s);
		Type2<slong> center = Type2<slong>(960, 540);
		Type2<slong> pos = center;
		slong offset = 0;

		char alpha = 255 * clamp<float>((time - 2.0f) * 10.0f, 0, 1.0f) * clamp<float>(time - 2.0f, 0, 1.0f);


		//Layer 0
		offset = 24 * s;
		pos = center - Type2<slong>(offset, offset);
		if (time > 2.0f)
			this->m_pSprite->DrawTex(m_vLogo[0], pos, scale, NULL, NULL, SDL_BLENDMODE_BLEND, _COLOR<uchar>(255, 255, 255, alpha));

		//Layer 1
		offset = 24 * s;
		pos = center - Type2<slong>(offset, offset);
		this->m_pSprite->DrawTex(m_vLogo[1], pos, scale, NULL, NULL, SDL_BLENDMODE_BLEND, clr);

		//Layer 3
		offset = 8 * s;
		pos = center + Type2<slong>(offset, offset);
		if (time > 2.0f)
			this->m_pSprite->DrawTex(m_vLogo[3], pos, scale, NULL, NULL, SDL_BLENDMODE_BLEND, _COLOR<uchar>(255, 255, 255, alpha));


		//Layer 4
		offset = 16 * s;
		pos = center + Type2<slong>(offset, offset);
		Sprite::TRotation rot;
		rot.m_fAngle = clamp<float>((slong)(time * 6) * 360.0f / 12.0f, 0.0f, 360.0f);

		rot.m_tCenterPoint = Type2<slong>(center.x, center.y);
		this->m_pSprite->DrawTex(m_vLogo[4], pos, scale, NULL, &rot, SDL_BLENDMODE_BLEND, clr);
	}
#endif

}

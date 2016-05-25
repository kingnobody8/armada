/*
Author:			Daniel Habig
Date:			11-15-13
File:			IntroState.h
Purpose:		gamestate that allows selection of sub-states
*/
#pragma once
#include "igamestate.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Slider.h"

class COptionAVState : public IGameState
{
private:
	enum EOptionButtons { BACK, NUM_BUTTONS };
	enum EButtonGroups	{ PRIMARY, AUDIO, VIDEO, NUM_BUTTON_GROUPS };
	enum EAudioButtons	{ SFX, MUS, NUM_AUDIO_BUTTONS };

private:		//Data
	CStarField*					m_pStarField;
	GUI::CButton				m_vButtons[NUM_BUTTONS];
	GUI::CSlider				m_vAudioButtons[NUM_AUDIO_BUTTONS];
	GUI::CSlider				m_cGammaButton;
	GUI::CRadioButton			m_cFullscreenButton;
	GUI::CButtonGroup			m_vButtonGroup[NUM_BUTTON_GROUPS];
	Time						m_dPercentTimer;
	slong						m_sfxTick;


protected:		//Data
public:			//Data

private:		//Methods
	void		InputPrimary(void);
	void		InputAudio(void);
	void		InputVideo(void);
	void		RenderButtons(void);
	void		RenderText(void);

	void		SaveAudio(void);
	void		LoadAudio(void);
protected:		//Methods
public:			//Methods
				COptionAVState(void);						// ctor
	virtual		~COptionAVState(void);						// dtor

	// IGameState Interface					  
	/*virtual*/ void		Enter(Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText, Audio::CAudio* pAudio );	// load resources
	/*virtual*/ void		Exit(void);																	// unload resources

	// Primary Funcs						  
	/*virtual*/ void		Input(void);							// handle user input
	/*virtual*/ void		Update(Time delta, Time gameTime);		// update game entities
	/*virtual*/ void		Render(void);							// render game entities

	inline void SetStarField(CStarField* pStarField) { this->m_pStarField = pStarField; }
};

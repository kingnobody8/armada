/*
Author:			Daniel Habig
Date:			11-15-13
File:			IntroState.h
Purpose:		gamestate that allows selection of sub-states
*/
#pragma once
#include "PopUpState.h"

class CPUTimedState : public IPopUpState
{
private:		//Data
	Time			m_dTimer;
	Time			m_dMaxTime;
	GUI::CButton	m_cHeader;
	bool			m_bBar;
	_COLOR<uchar>	m_tBarClr;
	slong			m_slBarSize;
	slong			m_slFadeImg;
protected:		//Data
public:			//Data

private:		//Methods
protected:		//Methods
public:			//Methods
							CPUTimedState(void);						// ctor
	virtual					~CPUTimedState(void);						// dtor

	// IGameState Interface
	virtual void			Enter(Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText, Audio::CAudio* pAudio);		// load resources
	virtual void			Exit(void);																								// unload resources

	// Primary Funcs
	virtual void			Input(void);						// handle user input
	virtual void			Update(Time delta, Time gameTime);	// update game entities
	virtual void			Render(void);						// render game entities

	//Gets
	inline Time			GetMaxTime	( void )			const	{ return this->m_dMaxTime; }
	inline GUI::CButton	GetHeader	( void )			const	{ return this->m_cHeader; }
	inline bool			GetBar		( void )			const	{ return this->m_bBar; }
	inline _COLOR<uchar> GetBarClr(void) const { return this->m_tBarClr; }

	//Sets
	inline void		SetTimer	( const Time dTime)				{ this->m_dTimer = this->m_dMaxTime = dTime; }
	inline void		SetHeader	( const GUI::CButton cHead )	{ this->m_cHeader = cHead; }
	inline void		SetBar		( const bool bBar )				{ this->m_bBar = bBar; }
	inline void SetBarClr(const _COLOR<uchar> tClr) { this->m_tBarClr = tClr; }
};



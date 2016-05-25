/*
Author:			Daniel Habig
Date:			11-9-13
File:			Machine.h
Purpose:		State Machine for creation, looping, and destruction of the game
*/
#pragma once
#include "../Util/XTime.h"
#include "../Util/Types.h"
#include "../SDL/Sprite.h"
#include "../Text/Text.h"
#include "../Input/InputCMD.h"
#include "../Audio/Audio.h"
#include "../States/IGameState.h"
#include "../Console Debug/ConsoleDebug.h"
#include <stack>
#include <vector>


class CMachine
{
private:	//Data
	bool						m_bRunApp;
	//State
	IGameState*					m_pCurrState;
	std::stack<IGameState*>		m_vStateStack;
	//Timer
	XTime						m_tXTime;
	//HWND
	HWND						m_tHWND;

	//CMD Classes
	Sprite::CSprite				m_cSprite;
	Input::CInput				m_cInput;
	Text::CText					m_cText;
	Audio::CAudio				m_cAudio;
	CConsoleDebug				m_cDebug;

private:	//Methods
	void	PushState			( IGameState* pNextState );
	void	PrevState			( IGameState::EGameStateType eType );
	void	GrabState			( void );
	void	PopState			( void );
	void	KeyCombo			( void );
public:		//Methods
			CMachine			( void );
			~CMachine			( void );

	//WinMain Funcs
	void	Start				( const char* win_title, const LPCWSTR win_name );
	bool	Loop				( void );
	void	End					( void );
};
/*
Author:			Daniel Habig
Date:			11-9-13
File:			IGameState.h
Purpose:		Abstract Base Class for Game States to inherit from.
*/
#pragma once
#include "../Graphics/ButtonManager.h"
#include "../Util/Types.h"
#include <vector>
using namespace Type;

//Forward Declaration
namespace Sprite { class CSprite; }
namespace Input { class CInput; }
namespace Text { class CText; }
namespace Audio { class CAudio; }

//Abstract Base Class
class IGameState
{
public:	//IGameState enums and structs
	enum EGameStateType {	gs_DEFAULT, gs_INTRO, gs_START, gs_MAIN_MENU, gs_GAMEPLAY, gs_CAMPAIGN, gs_MULTIPLAYER, gs_OPTIONS,
							gs_OPTIONS_AV, gs_CREDITS, gs_FORGE, gs_PRELOBBY, gs_LOBBY, gs_LOBBY_SERVER, gs_SETUP, gs_POPUP,
							gs_NUM_TYPES };
	enum EStateResult	{ sr_PUSH, sr_PREV, sr_END_APP, sr_DEFAULT };

	struct TStateResult
	{
	private:
		IGameState*		m_pState;
		EGameStateType	m_eType;
		EStateResult	m_eResult;
		
	public:
		TStateResult(void)
		{
			m_pState = NULL;
			m_eType = EGameStateType::gs_DEFAULT;
			m_eResult = EStateResult::sr_DEFAULT;
		}
		inline void Push(IGameState* const pState)
		{
			m_pState = pState;
			m_eResult = EStateResult::sr_PUSH;
		}
		inline void Prev(const EGameStateType eType)
		{
			m_eType = eType;
			m_eResult = EStateResult::sr_PREV;
		}
		inline void Kill(void)
		{
			m_eResult = EStateResult::sr_END_APP;
		}

		inline IGameState*		GetState	(void) const { return this->m_pState; }
		inline EGameStateType	GetType		(void) const { return this->m_eType; }
		inline EStateResult		GetResult	(void) const { return this->m_eResult; }
	};

private:	//Data
protected:	//Data
	EGameStateType				m_eType;
	TStateResult				m_tResult;
	IGameState*					m_pPrevState;
	GUI::CButtonManager			m_cButMan;		//Button manager

	Sprite::CSprite*			m_pSprite;		//Rendering System
	Input::CInput*				m_pInput;		//Input System
	Text::CText*				m_pText;		//Text System
	Audio::CAudio*				m_pAudio;		//Audio System
public:		//Data

private:	//Methods
protected:	//Methods
public:		//Methods
							IGameState		( void );							// ctor
	virtual					~IGameState		( void );							// dtor

	// IGameState Interface
	virtual void			Enter			( Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText, Audio::CAudio* pAudio );		// load resources
	virtual void			Exit			( void );							// unload resources

	// Primary Funcs
	virtual void			Input			( void ) = 0;							// handle user input
	virtual void			Update			( Time dDelta, Time dGameTime ) = 0;	// update game entities
	virtual void			Render			( void ) = 0;							// render game entities

	// Shared Funcs
	inline	EGameStateType	GetType			( void )	const	{ return this->m_eType; }	// the type of state
	inline	TStateResult	GetResult		( void )	const	{ return this->m_tResult; }	// how should the state machine respond
	inline	void			ResetResult		( void )			{ this->m_tResult = TStateResult(); }
	inline	IGameState*		GetPrevState	( void )	const	{ return this->m_pPrevState; }
	inline	void			SetPrevState	( IGameState* const pPrev )	{ this->m_pPrevState = pPrev; }

																							

};


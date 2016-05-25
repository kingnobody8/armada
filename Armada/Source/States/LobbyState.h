/*
Author:			Daniel Habig
Date:			11-15-13
File:			IntroState.h
Purpose:		gamestate that begins the game, shows logos, transitions to StartState
*/
#pragma once
#include "igamestate.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/RadioButton.h"
#include "../Graphics/Slider.h"
#include "../Network/NetMsg.h"
#include "SetupState.h"
#include "PUTimedState.h"

//Forward Declare
namespace Network { class CClient; }

class CLobbyState :	public IGameState
{
private:
	enum ELobbyButtons	{ BACK, NUM_BUTTONS };
	enum EListButtons	{ FIRST, SECOND, THIRD, FOURTH, FIFTH, SIXTH, SEVENTH, EIGTH, NUM_LIST_SLOTS, INC = 0, DEC, NUM_LIST_BUTS };
	enum EButtonGroups	{ PRIMARY, SERVER_SLOTS, SERVER_BUTS, JOINING, NUM_BUTTON_GROUPS };

private:		//Data
	CStarField*					m_pStarField;

	CSetupState					m_cSetupState;

	//Buttons
	GUI::CButton				m_vButtons[NUM_BUTTONS];
	GUI::CButton				m_vServerListButtons[NUM_LIST_BUTS];
	GUI::CRadioButton			m_vServerSlotButtons[NUM_LIST_SLOTS];
	GUI::CButton				m_cJoinButton;
	GUI::CButtonGroup			m_vButtonGroup[NUM_BUTTON_GROUPS];
	//Buttons Only Used for Text
	GUI::CButton				m_cLobbyName;
	GUI::CButton				m_vBroadcastNames[Game::kulMaxPlayers];

	//Broadcast Data
	Network::CClient*			m_pClient;
	Time						m_dBroadCastTimer;
	Game::TClientData			m_vClientData[Game::kulMaxPlayers];

	//Server List
	std::vector<Network::CNetMsg>	m_vServerList;
	slong							m_slServerOffset;
	slong							m_slServerIndex;

	//Popup
	CPUTimedState		m_cPUTimedState;

protected:		//Data
public:			//Data

private:		//Methods
	void		InputPrimary(void);
	void		InputServerSlots(void);
	void		InputServerButs(void);
	void		InputJoin(void);
	void		UpdateBroadcast(Time dDelta);
	void		UpdateRecvMsg(Time dDelta);
	void		RenderButtons(void);
	void		RenderText(void);
	void		DetermineActiveButtons(void);
	void		RecvNoSlots(void);
protected:		//Methods
public:			//Methods
							CLobbyState		( void );							// ctor
	virtual					~CLobbyState	( void );							// dtor
											  
	// IGameState Interface					  
	/*virtual*/ void		Enter			( Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText, Audio::CAudio* pAudio  );		// load resources
	/*virtual*/ void		Exit			( void );							// unload resources
											  
	// Primary Funcs						  
	/*virtual*/ void		Input			( void );							// handle user input
	/*virtual*/ void		Update			( Time delta, Time gameTime );		// update game entities
	/*virtual*/ void		Render			( void );							// render game entities

	//Sets
	inline void SetStarField(CStarField* pStarField) { this->m_pStarField = pStarField; }
	inline void SetClient(Network::CClient* pClient) { this->m_pClient = pClient; }
};

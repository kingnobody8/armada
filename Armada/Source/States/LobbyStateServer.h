/*
Author:			Daniel Habig
Date:			11-15-13
File:			IntroState.h
Purpose:		gamestate that begins the game, shows logos, transitions to StartState
*/
#pragma once
#include "igamestate.h"
#include "../Network/Server.h"
#include "../Game/MatchData.h"
#include "../Board/Board.h"

class CLobbyStateServer :	public IGameState
{
private:		//Data
	Network::CServer		m_cServer;
	Game::TClientData		m_vClientData[Game::kulMaxPlayers];
	std::vector<std::pair<schar, std::string>>	m_vTxtMsgs;
	Board::CBoard			m_cBoard;

protected:		//Data
public:			//Data

private:		//Methods
	void		HandleDisconnect(Network::CNetMsg &msg);
	void		RecvLobbyEnter(Network::CNetMsg &msg);
	void		RecvClientData(Network::CNetMsg &msg);
	void		RecvClientType(Network::CNetMsg &msg);
	void		RecvTxtMsg(Network::CNetMsg &msg);
	void		RecvSlotRequest(Network::CNetMsg &msg);
	void		RecvBoard(Network::CNetMsg &msg);
	void		SendLobbyData(void);
	void		SendTextMsg(bool bSendAll);
	void		SendBoard(void);
	void		DetermineClientLock(void);
protected:		//Methods
public:			//Methods
							CLobbyStateServer	( void );							// ctor
	virtual					~CLobbyStateServer	( void );							// dtor
											  
	// IGameState Interface					  
	/*virtual*/ void		Enter			( Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText, Audio::CAudio* pAudio  );		// load resources
	/*virtual*/ void		Exit			( void );							// unload resources
											  
	// Primary Funcs						  
	/*virtual*/ void		Input			( void );							// handle user input
	/*virtual*/ void		Update			( Time delta, Time gameTime );		// update game entities
	/*virtual*/ void		Render			( void );							// render game entities
};

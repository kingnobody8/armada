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
#include "../Board/Board.h"
#include "../Game/MatchData.h"
#include "../Network/NetMsg.h"
#include "PUTimedState.h"

//Forward Declare
namespace Network { class CClient; }
class CServerMachine;

class CSetupState :	public IGameState
{
private:
	enum ESetupButtons	{ BACK, NUM_BUTTONS };
	enum EListButtons	{ FIRST, SECOND, THIRD, FOURTH, NUM_LIST_SLOTS, INC = 0, DEC, NUM_LIST_BUTS };
	enum EButtonGroups	{ PRIMARY, NAMES, CLIENT_TYPES, TXTMSG, MSG_BUTS, GRID, DEFAULT_SLOTS, DEFAULT_BUTS, CUSTOM_SLOTS, CUSTOM_BUTS, NUM_BUTTON_GROUPS };

private:		//Data
	CStarField*			m_pStarField;

	CServerMachine*		m_pSMach;
	Network::CClient*	m_pClient;

	CPUTimedState		m_cPUTimedState;

	//Game
	Game::TClientData			m_vClientData[Game::kulMaxPlayers];

	//Buttons
	GUI::CButton				m_vButtons[NUM_BUTTONS];
	GUI::CButton				m_vGrid[Board::kslBoardSize][Board::kslBoardSize];
	GUI::CRadioButton			m_vNameButtons[Game::kulMaxPlayers];
	GUI::CButton				m_vClientTypeButtons[Game::kulMaxPlayers];
	GUI::CButton				m_vDefaultListButtons[NUM_LIST_BUTS];
	GUI::CRadioButton			m_vDefaultSlotButtons[NUM_LIST_SLOTS];
	GUI::CButton				m_vCustomListButtons[NUM_LIST_BUTS];
	GUI::CRadioButton			m_vCustomSlotButtons[NUM_LIST_SLOTS];
	GUI::CRadioButton			m_cTextMsgButton;
	GUI::CButton				m_cTxtMsgGui;							//GUI
	GUI::CButton				m_vMsgSlotTxtGui[NUM_LIST_SLOTS - 1];	//GUI
	GUI::CButton				m_vMsgSlotNameGui[NUM_LIST_SLOTS - 1];	//GUI
	GUI::CButton				m_vMsgListButtons[NUM_LIST_BUTS];
	GUI::CButtonGroup			m_vButtonGroup[NUM_BUTTON_GROUPS];

	//Board
	Board::CBoard				m_cBoard;
	bool						m_bDefault;
	//Default List
	std::vector<Board::CBoard>	m_vDefaultList;
	slong						m_slDefaultOffset;
	slong						m_slDefaultIndex;
	//Custom List
	std::vector<Board::CBoard>	m_vCustomList;
	slong						m_slCustomOffset;
	slong						m_slCustomIndex;
	// Name '_' Timer
	Time						m_dMarkerTimer;
	bool						m_bMarkerVisible;
	//Txt Msgs
	std::vector<std::pair<schar, std::string>>	m_vTxtMsgs;
	slong						m_slMsgOffset;
	slong						m_slSfxMsg;

protected:		//Data
public:			//Data

private:		//Methods
	/*Enter*/
	void		EnterPrimary(void);
	void		EnterGrid(void);
	void		EnterNames(void);
	void		EnterClientType(void);
	void		EnterTextMsg(void);
	void		EnterTextSlots(void);
	void		EnterTextButs(void);
	void		EnterDefaultSlots(void);
	void		EnterDefaultButs(void);
	void		EnterCustomSlots(void);
	void		EnterCustomButs(void);
	/*Input*/
	void		InputPrimary(void);
	void		InputGrid(void);
	void		InputNames(void);
	void		InputClientTypes(void);
	void		InputTxtMsg(void);
	void		InputTxtButs(void);
	void		InputDefaultSlots(void);
	void		InputDefaultButs(void);
	void		InputCustomSlots(void);
	void		InputCustomButs(void);
	/*Update*/
	void		UpdateNames(Time dDelta);
	void		UpdateTxtMsg(Time dDelta);
	void		UpdateClient(Time dDelta);
	/*Render*/
	void		RenderButtons(void);
	void		RenderText(void);
	void		RenderClientType(void);
	/*Loading*/
	void		LoadDefault(void);
	void		LoadCustom(void);
	void		SortBoards(void);
	/*Other*/
	void		DetermineActiveButtons(void);
	void		SetBoard(void);
	/*Network*/
	void		RecvDisconnect(Network::CNetMsg &msg);
	void		RecvLobbyData(Network::CNetMsg &msg);
	void		RecvTxtMsg(Network::CNetMsg &msg);
	void		RecvBoard(Network::CNetMsg &msg);
	void		SendClientData(void);
	void		SetLobbyData(void);
	void		SendTxtMsg(const char* strTxt);
	void		SendSlotRequest(const schar scSlot);
	void		SendBoard(void);
	inline bool Host(void) { return this->m_pSMach != NULL; }
protected:		//Methods
public:			//Methods
							CSetupState		( void );							// ctor
	virtual					~CSetupState	( void );							// dtor
											  
	// IGameState Interface					  
	/*virtual*/ void		Enter			( Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText, Audio::CAudio* pAudio  );		// load resources
	/*virtual*/ void		Exit			( void );							// unload resources
											  
	// Primary Funcs						  
	/*virtual*/ void		Input			( void );							// handle user input
	/*virtual*/ void		Update			( Time delta, Time gameTime );		// update game entities
	/*virtual*/ void		Render			( void );							// render game entities

	//Gets
	//Sets
	inline void SetStarField(CStarField* pStarField) { this->m_pStarField = pStarField; }
	inline void SetClient(Network::CClient* pClient) { this->m_pClient = pClient; }
	inline void SetServerMachine(CServerMachine* pSMach) { this->m_pSMach = pSMach; }
};

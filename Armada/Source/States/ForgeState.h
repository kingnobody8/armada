/*
Author:			Daniel Habig
Date:			11-15-13
File:			IntroState.h
Purpose:		gamestate that allows selection of sub-states
*/
#pragma once
#include "igamestate.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/RadioButton.h"
#include "../Board/Board.h"

class CForgeState : public IGameState
{
private:
	enum EForgeButtons	{ BACK, NUM_BUTTONS };
	enum ESlotButtons	{ S0, S1, S2, S3, S4, OFF, NUM_SLOT_BUTTONS, NO_SBUT };
	enum EPlayerButtons { P1, P2, P3, P4, NUM_PLAYER_BUTTONS, NO_PBUT };
	enum EMapButtons	{ NEW, COPY, DEL, NUM_MAP_BUTTONS  };
	enum EListButtons	{ FIRST, SECOND, THIRD, FOURTH, NUM_LIST_SLOTS, INC = 0, DEC, NUM_LIST_BUTS };
	enum EButtonGroups	{ PRIMARY, SLOTS, SPAWNS, GRID, MAPNAME, MAPBUTS, DEFAULT_SLOTS, DEFAULT_BUTS, CUSTOM_SLOTS, CUSTOM_BUTS, NUM_BUTTON_GROUPS };

private:		//Data
	CStarField*					m_pStarField;

	//Buttons
	GUI::CButton				m_vButtons[NUM_BUTTONS];
	GUI::CButton				m_vGrid[Board::kslBoardSize][Board::kslBoardSize];
	GUI::CRadioButton			m_vSlotButtons[NUM_SLOT_BUTTONS];
	GUI::CRadioButton			m_vPlayerButtons[NUM_PLAYER_BUTTONS];
	GUI::CRadioButton			m_cMapNameButton;
	GUI::CButton				m_vMapButtons[NUM_MAP_BUTTONS];
	GUI::CButton				m_vDefaultListButtons[NUM_LIST_BUTS];
	GUI::CRadioButton			m_vDefaultSlotButtons[NUM_LIST_SLOTS];
	GUI::CButton				m_vCustomListButtons[NUM_LIST_BUTS];
	GUI::CRadioButton			m_vCustomSlotButtons[NUM_LIST_SLOTS];
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

	//Board Editing
	ESlotButtons			m_eSelectedSlot;
	EPlayerButtons			m_eSelectedPlayer;

	//Board Name '_' Timer
	Time					m_dMarkerTimer;
	bool					m_bMarkerVisible;

protected:		//Data
public:			//Data

private:		//Methods
	/*Enter*/
	void		EnterPrimary(void);
	void		EnterSlots(void);
	void		EnterSpawns(void);
	void		EnterGrid(void);
	void		EnterMapName(void);
	void		EnterMapButs(void);
	void		EnterDefaultSlots(void);
	void		EnterDefaultButs(void);
	void		EnterCustomSlots(void);
	void		EnterCustomButs(void);
	/*Input*/
	void		InputPrimary(void);
	void		InputSlots(void);
	void		InputSpawns(void);
	void		InputGrid(void);
	void		InputMapName(void);
	void		InputMapButs(void);
	void		InputDefaultSlots(void);
	void		InputDefaultButs(void);
	void		InputCustomSlots(void);
	void		InputCustomButs(void);
	void		InputSpecialCase_Grid(void);
	/*Render*/
	void		RenderButtons(void);
	void		RenderText(void);
	void		RenderGrid(void);
	/*Saving & Loading*/
	void		LoadDefault(void);
	void		SaveDefault(void);
	void		LoadCustom(void);
	void		SaveCutsom(void);
	//Used when unclicking a button (going back to a safe state)
	void		DetermineActiveButtons(void);
	void		SetBoard(void);
	void		GetBoard(void);
	void		SortBoards(void);
protected:		//Methods
public:			//Methods
							CForgeState		( void );					// ctor
	virtual					~CForgeState	( void );					// dtor

	// IGameState Interface					  
	/*virtual*/ void		Enter(Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText, Audio::CAudio* pAudio );	// load resources
	/*virtual*/ void		Exit		( void );																	// unload resources

	// Primary Funcs						  
	/*virtual*/ void		Input		( void );							// handle user input
	/*virtual*/ void		Update		( Time delta, Time gameTime );		// update game entities
	/*virtual*/ void		Render		( void );							// render game entities

	inline void SetStarField(CStarField* pStarField) { this->m_pStarField = pStarField; }
};



/*
Author:			Daniel Habig
Date:			11-9-13
File:			Server_Machine.h
Purpose:		State Machine for a server
*/
#pragma once
#include "../States/IGameState.h"
#include "../Util/Types.h"
#include <stack>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
using namespace Type;

class CServerMachine
{
private:	//Data
	std::atomic_bool			m_bRunApp;
	//State
	IGameState*					m_pCurrState;
	std::stack<IGameState*>		m_vStateStack;
	//Threading
	std::thread*				loopThread;
	std::mutex					boolMutex;
	//Cleanup Sync
	std::condition_variable		con_var;
	std::mutex					cv_mutex;

private:	//Methods
	void	PushState			( IGameState* pNextState );
	void	PrevState			( IGameState::EGameStateType eType );
	void	GrabState			( void );
	void	PopState			( void );

	//Loop Funcs
	void	Loop				( void );
	void	L_ExitSync			( void );
	void	L_Update			( Time dDelta, Time dGameTime );

public:		//Methods
			CServerMachine		( void );
			~CServerMachine		( void );

	bool	Start				( IGameState* pState );
	void	End					( void );

	inline bool GetActive(void) const { return this->m_bRunApp; }

};
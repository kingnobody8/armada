#include "Server_Machine.h"
#include "../States/IGameState.h"
#include "../Thread_Pool/ThreadPool.h"
#include "../Util/XTime.h"

CServerMachine::CServerMachine(void)
{
	this->m_bRunApp = true;
	this->m_pCurrState = NULL;
	this->loopThread = NULL;
}
CServerMachine::~CServerMachine(void)
{
	this->End();
}

//Main Funcs
bool CServerMachine::Start(IGameState* pState)
{
	//Early out
	if (this->loopThread != NULL || pState == NULL)
		return false;

	//Start Intro State
	this->PushState(pState);

	//Spawn the thread
	this->m_bRunApp = true;
	loopThread = new std::thread(&CServerMachine::Loop, this);
	ThreadPool::SetThreadName(loopThread, "Server");

	return true;
}
void CServerMachine::Loop(void)
{
	//Timer
	XTime		xTime;
	xTime.Restart();

	//Loop
	while (this->m_bRunApp)
	{
		//Update Timers
		xTime.Signal();
		this->L_Update(xTime.Delta(), xTime.TotalTime());
	}

	//Sync to leave the func
	this->L_ExitSync();
}
void CServerMachine::End(void)
{
	//If we aren't running a thread, then return
	if (this->loopThread == NULL)
		return;

	//Mark for exit
	this->m_bRunApp = false;

	//Wait for the Loop Thread to end
	/*
	std::unique_lock<std::mutex> ulock(cv_mutex);
	this->con_var.wait(ulock, [&](){return true; });
	ulock.unlock();
	*/
	this->loopThread->join();

	//Cleanup Thread Memory
	if (loopThread != NULL)
		delete loopThread;
	loopThread = NULL;

	//Pop all the states
	while (!this->m_vStateStack.empty())
		this->PopState();
	this->m_pCurrState = NULL;
}

//Loop Funcs
void CServerMachine::L_Update(Time dDelta, Time dGameTime)
{
	// Grab current state
	this->GrabState();

	//State Loop
	if (this->m_pCurrState != NULL)
		this->m_pCurrState->Update(dDelta, dGameTime);
}
void CServerMachine::L_ExitSync(void)
{
	this->cv_mutex.lock();
	this->con_var.notify_all();
	this->cv_mutex.unlock();
}

//State Change
void CServerMachine::PushState(IGameState* pNextState)
{
	if (pNextState == nullptr || pNextState == this->m_pCurrState)
		return;

	this->m_vStateStack.push(pNextState);
	pNextState->Enter(NULL, NULL, NULL, NULL);

	//If this is the first state
	if (this->m_pCurrState == NULL)
		this->m_pCurrState = pNextState;
}
void CServerMachine::PrevState(IGameState::EGameStateType eType)
{
	while (true)
	{
		this->PopState();
		if (this->m_vStateStack.empty())
		{
			this->m_bRunApp = false;
			this->m_pCurrState = NULL;
			break;
		}
		if (this->m_vStateStack.top()->GetType() == eType)
		{
			this->m_pCurrState = this->m_vStateStack.top();
			break;
		}
	}
}
void CServerMachine::PopState(void)
{
	IGameState* pState = this->m_vStateStack.top();
	pState->Exit();
	this->m_vStateStack.pop();

	//If last state, then delete
	if (this->m_vStateStack.empty())
		delete pState;
}
void CServerMachine::GrabState(void)
{
	IGameState::TStateResult tResult = this->m_pCurrState->GetResult();
	this->m_pCurrState->ResetResult();

	switch (tResult.GetResult())
	{
	case IGameState::EStateResult::sr_DEFAULT:
		this->m_pCurrState = this->m_vStateStack.top();
		break;
	case IGameState::EStateResult::sr_END_APP:
		while (!this->m_vStateStack.empty())
			this->PopState();
		this->m_pCurrState = NULL;
		this->m_bRunApp = false;
		break;
	case IGameState::EStateResult::sr_PREV:
		this->PrevState(tResult.GetType());
		break;
	case IGameState::EStateResult::sr_PUSH:
		this->PushState(tResult.GetState());
		this->m_pCurrState = this->m_vStateStack.top();
		break;
	}
}

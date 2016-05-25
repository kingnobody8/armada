/*
Author:			Daniel Habig
Date:			11-9-13
File:			Machine.cpp
Purpose:		State Machine for creation, looping, and destruction of the game
*/
#include "Machine.h"
#include "../States/IGameState.h"
#include "../States/IntroState.h"
#include "../Paker/Paker.h"

#define SCREEN_WIDTH			1280
#define SCREEN_HEIGHT			720

CMachine::CMachine(void)
{
	this->m_bRunApp = true;
	this->m_pCurrState = NULL;
}
CMachine::~CMachine(void)
{
	this->End();
}

//WinMain Funcs
void CMachine::Start(const char* win_title, const LPCWSTR win_name)
{
	//Initialize CMD Classes
	if (!this->m_cSprite.Init(win_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT))
		this->m_bRunApp = false;
	this->m_tHWND = FindWindow(NULL, win_name);
	this->m_cInput.Start(&this->m_tHWND, &this->m_cSprite);
	this->m_cText.Start(&m_cSprite);
	this->m_cAudio.Start();
	Pak::Load("files.pak");
	this->m_cText.LoadBinData("default.font", "default.png");
	this->m_tXTime.Restart();

	//Start Intro State
	this->PushState(new CIntroState());

	//Build CMD classes
	//this->sprite = pSprite;
	//this->thread = ThreadPoolCMD::GetInstance();
	//this->input = InputCMD::GetInstance();
	//this->input->Start(pHWND);
	//Paker::GetInstance();
	//if (!Paker::GetInstance()->LoadPak("files.pak"))
	//	this->KillApp();
}
bool CMachine::Loop(void)
{
	// Update Timer
	this->m_tXTime.Signal();
	double dDelta = clamp<double>(this->m_tXTime.Delta(), 0.0f, 0.125f);
	double dGameTime = this->m_tXTime.TotalTime();

	// Update CMD Classes
	if (!this->m_cSprite.Poll())
		this->m_bRunApp = false;
	this->m_cInput.Update(dDelta);
	this->m_cAudio.Update(dDelta);

	//Key Combos
	this->KeyCombo();

	// Grab current state
	this->GrabState();

	//State Loop
	if (this->m_pCurrState != NULL)
	{
		// Input
		this->m_pCurrState->Input();
		// Update
		this->m_pCurrState->Update(dDelta, dGameTime);
		// Render
		this->m_cSprite.Primer();
		this->m_pCurrState->Render();
		this->m_cSprite.Present();
	}

	return this->m_bRunApp;
}
void CMachine::End(void)
{
	//Perform un-init in reverse order
	this->m_cSprite.UnloadAll();
	this->m_cAudio.Release();

	//Pop all the states
	while (!this->m_vStateStack.empty())
		this->PopState();
	this->m_pCurrState = NULL;
	this->m_bRunApp = false;
}

//State Change
void CMachine::PushState(IGameState* pNextState)
{
	if (pNextState == nullptr || pNextState == this->m_pCurrState)
		return;

	this->m_vStateStack.push(pNextState);
	pNextState->SetPrevState(this->m_pCurrState);
	pNextState->Enter(&this->m_cSprite, &this->m_cInput, &this->m_cText, &this->m_cAudio);

	//If this is the first state
	if (this->m_pCurrState == NULL)
		this->m_pCurrState = pNextState;
}
void CMachine::PrevState(IGameState::EGameStateType eType)
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
void CMachine::PopState(void)
{
	IGameState* pState = this->m_vStateStack.top();
	pState->Exit();
	this->m_vStateStack.pop();

	//If last state, then delete
	if (this->m_vStateStack.empty())
		delete pState;
}
void CMachine::GrabState(void)
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
		this->m_cInput.Reset(true);
		break;
	case IGameState::EStateResult::sr_PUSH:
		this->PushState(tResult.GetState());
		this->m_pCurrState = this->m_vStateStack.top();
		this->m_cInput.Reset(true);
		break;
	}
}

void CMachine::KeyCombo(void)
{
	//Alt Button
	if (this->m_cInput.Key_Held(VK_MENU))
	{
		//Kill App
		if (this->m_cInput.Key_Push(VK_F4))
			this->m_bRunApp = false;
		//Full Screen
		if (this->m_cInput.Key_Push(VK_RETURN))
			this->m_cSprite.FullScreenToggle();
		//Give Mouse Back
		if (this->m_cInput.Key_Push(VK_F5))
			this->m_cInput.Cursor_Set_Override(!this->m_cInput.Cursor_Get_Override());
#ifdef _DEBUG
		//Console Debug
		if (this->m_cInput.Key_Push(VK_F2))
		{
			this->m_cDebug.ToggleActive();
			SetForegroundWindow(this->m_tHWND);
		}
		//Clear Console Debug
		if (this->m_cInput.Key_Push(VK_F3))
		{
			this->m_cDebug.ClearConsole();
		}
		//Show Render Rects
		if (this->m_cInput.Key_Push(VK_F6))
		{
			this->m_cSprite.SetShowRect(!this->m_cSprite.GetShowRect());
		}
		
#endif
	}
}
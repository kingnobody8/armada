/*
Author:			Daniel Habig
Date:			11-9-13
File:			wt_UpdateInputCMD.cpp
Purpose:		Worker Thread, used to update the InputCMD singleton
*/
#pragma once
#include "../Input/InputCMD.h"
#include "workerthread.h"

class wt_UpdateInputCMD : public ThreadPool::IWorkerThread
{
public:
	double delta;
public:
	wt_UpdateInputCMD(void)
	{
		delta = 0.0f;
	}
	wt_UpdateInputCMD(double nDelta )
	{
		this->delta = nDelta;
	}

	void Work()
	{
		InputCMD::GetInstance()->Update(delta);
	}

};
/*
Author:			Daniel Habig
Date:			11-9-13
File:			Seamaphore_Work_Ready.cpp
Purpose:		Semaphore class used specifically for notifying ( and unlocking ) when ALL work is done.
*/
#include "Semaphore_Work_Ready.h"

namespace ThreadPool
{
	Semaphore_Work_Ready::Semaphore_Work_Ready(void)
	{
		this->count = 0;
	}
	void Semaphore_Work_Ready::Wait(void)
	{
		unique_lock<mutex> unique_lock(this->count_mutex);
		this->cond_var.wait(unique_lock, [&](){return this->count != 0; });
		this->count--;
	}
	void Semaphore_Work_Ready::Post(void)
	{
		std::lock_guard<std::mutex> guardLock(this->count_mutex);
		count++;
		this->cond_var.notify_one();
	}
}
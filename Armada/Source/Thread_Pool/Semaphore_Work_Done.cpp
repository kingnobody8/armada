/*
Author:			Daniel Habig
Date:			11-9-13
File:			Seamaphore_Work_Done.cpp
Purpose:		Semaphore class used specifically for notifying ( and unlocking ) when ALL work is done.
*/
#include "Semaphore_Work_Done.h"

namespace ThreadPool
{
	Semaphore_Work_Done::Semaphore_Work_Done(void)
	{
		this->count = 0;
	}
	bool Semaphore_Work_Done::Wait(bool bBlocking)
	{
		if (bBlocking)
		{
			unique_lock<mutex> unique_lock(this->count_mutex);
			this->cond_var.wait(unique_lock, [&](){return this->count == 0; });
			return true;
		}
		else
		{
			bool locked = this->count_mutex.try_lock();
			if (locked)
			{
				this->count_mutex.unlock();
				if (this->count == 0)
					return true;
			}
			return false;
		}
	}
	void Semaphore_Work_Done::Inc(void)
	{
		std::lock_guard<std::mutex> guardLock(this->count_mutex);
		count++;
	}
	void Semaphore_Work_Done::Dec(void)
	{
		std::lock_guard<std::mutex> guardLock(this->count_mutex);
		count--;
		this->cond_var.notify_one();
	}
}
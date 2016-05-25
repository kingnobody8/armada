/*
Author:			Daniel Habig
Date:			11-9-13
File:			Seamaphore_Work_Done.h
Purpose:		Semaphore class used specifically for notifying ( and unlocking ) when ALL work is done.
*/
#pragma once
#include <mutex>
#include <condition_variable>
using namespace std;

namespace ThreadPool
{
	class Semaphore_Work_Done
	{
	private:
		condition_variable	cond_var;
		mutex				count_mutex;
		unsigned int		count;

	public:
		Semaphore_Work_Done(void);
		bool	Wait(bool bBlocking = true);// wait will pause until the count is 0, returns true on finished, false on unfinished (non-blocking only)
		void	Inc(void);// increases the count on the semaphore object 
		void	Dec(void);// decreases the count on the semaphore object

	};
}
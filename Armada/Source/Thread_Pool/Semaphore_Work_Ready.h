/*
Author:			Daniel Habig
Date:			11-9-13
File:			Seamaphore_Work_Ready.h
Purpose:		Semaphore class used specifically for notifying ( and unlocking ) when work to do is available.
*/
#pragma once
#include <mutex>
#include <condition_variable>
using namespace std;

namespace ThreadPool
{
	class Semaphore_Work_Ready
	{
	private:
		condition_variable	cond_var;
		mutex				count_mutex;
		unsigned int		count;

	public:
		Semaphore_Work_Ready(void);
		void	Wait(void);// wait should decrease the count on the semaphore object
		void	Post(void);// post should increase the count on the semaphore object 

	};
}

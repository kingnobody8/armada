/*
Author:			Daniel Habig
Date:			11-9-13
File:			WorkerThread.h
Purpose:		Abstract Base Class, used for WorkerThread classes to inherit from
*/
#pragma once

namespace ThreadPool
{
	class IWorkerThread
	{
	public:
						IWorkerThread	( void ) {}
		virtual void	Work			( void ) = 0;

	};
}
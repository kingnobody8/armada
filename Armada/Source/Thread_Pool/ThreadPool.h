/*
Author:			Daniel Habig
Date:			11-9-13
File:			ThreadPoolCMD.h
Purpose:		used for creation, execution, deletion, and syncronization of worker threads
*/
#pragma once
#include <queue>
#include <list>
#include <thread>
#include <atomic>
#include <mutex>
#include "WorkerThread.h"
#include "Semaphore_Work_Ready.h"
#include "Semaphore_Work_Done.h"
#include "../Util/Types.h"
using namespace Type;

namespace ThreadPool
{
	const bool	kbSingleThreaded = false;	//Setting to true will remove all benefits of the thread pool and perform the work immediately when posted
	const uchar kucThreadsPerCore = 1;		//How many threads will spawn for each core on the machine

	enum eShutdownType { DO_ALL_WORK, DO_CURRENT_WORK };

	void SetThreadName(std::thread* pThread, const char* threadName);

	class CThreadPool
	{
	private:
		//Data
		unsigned int			num_threads;	// number of spawned threads
		mutex					queue_Mutex;	// a lock to protect the work queue
		Semaphore_Work_Ready	wrk_ready_Sem;	// a sem to keep track of work in the queue
		Semaphore_Work_Done		wrk_done_Sem;	// a sem to keep track of when how much work has been completed
		queue<IWorkerThread*>	workers;		// the queue of worker threads
		vector<std::thread*>	threads;		// storage for threads
		atomic_bool				run;			// keep track if the threadpool needs to run
		eShutdownType			shutdown_type;	// if the pool should finish all tasks on exit

		//Methods
		void					Init(unsigned int _num_threads);		// initializes internal members and creates threads
		void					Shutdown(void);							// cleans up allocated resources
		void					Execute(void);							// the work horse function for all threads to execute
		void					GetWork(IWorkerThread** _worker_ptr);	// waits for work to be posted, grabs a work item from the queue

	public:
		CThreadPool(void);
		~CThreadPool(void);
		//Instance
		void					PostWork(IWorkerThread* _worker_ptr);		// increases the work count, adds work to the queue and posts the work
		inline void				SetShutdownMethod(eShutdownType _shutdown_type) { this->shutdown_type = _shutdown_type; }
		bool					Sync(bool bBlock = true);				// returns true on finished, false on unfinished (non-blocking only)
	};
}

namespace Pool
{
	void Post(ThreadPool::IWorkerThread* pWorker);
	void SetShutdownMethod(ThreadPool::eShutdownType eShutdown);
	bool Sync(bool bBlocking = true);
}

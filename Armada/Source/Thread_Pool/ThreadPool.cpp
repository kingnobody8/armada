/*
Author:			Daniel Habig
Date:			11-9-13
File:			ThreadPoolCMD.cpp
Purpose:		Used for creation, execution, deletion, and syncronization of worker threads
*/
#include "ThreadPool.h"
#include <windows.h>
#include <string>

namespace ThreadPool
{
#pragma region ThreadName
	const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)
	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType; // Must be 0x1000.
		LPCSTR szName; // Pointer to name (in user addr space).
		DWORD dwThreadID; // Thread ID (-1=caller thread).
		DWORD dwFlags; // Reserved for future use, must be zero.
	} THREADNAME_INFO;
#pragma pack(pop)
	void SetThreadName(std::thread* pThread, const char* threadName)
	{
		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = threadName;
		info.dwThreadID = ::GetThreadId(static_cast<HANDLE>(pThread->native_handle()));
		info.dwFlags = 0;

		__try
		{
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
#pragma endregion

	CThreadPool::CThreadPool(void)
	{
		unsigned int cores = std::thread::hardware_concurrency();
		this->Init(cores * kucThreadsPerCore);
	}
	CThreadPool::~CThreadPool(void)
	{
		this->Shutdown();
	}

	void CThreadPool::Init(unsigned int _num_threads)
	{
		if (!kbSingleThreaded)
		{
			//Initialize the data members
			this->num_threads = _num_threads;
			this->run = true;
			this->shutdown_type = DO_ALL_WORK;
			//Start the threads
			for (unsigned int i = 0; i < this->num_threads; i++)
			{
				std::thread* thread = new std::thread(&CThreadPool::Execute, this);
				this->threads.push_back(thread);
				SetThreadName(thread, (std::string("Pool #") + std::to_string(i)).c_str());
			}
		}
	}
	void CThreadPool::Shutdown(void)
	{
		if (!kbSingleThreaded)
		{
			switch (this->shutdown_type)
			{
			case DO_ALL_WORK:
				break;
			case DO_CURRENT_WORK:
			{
									//Pop all work
									this->queue_Mutex.lock();
									while (!this->workers.empty())
									{
										IWorkerThread* wrk = this->workers.front();
										this->workers.pop();
										delete wrk;
										//Decrement the Sync Semaphore
										wrk_done_Sem.Dec();
									}
									this->queue_Mutex.unlock();
			}
				break;
			}
			//Tell threads to end
			this->run = false;
			//Wakeup threads
			for (unsigned int i = 0; i < this->threads.size(); i++)
				this->wrk_ready_Sem.Post();
			//Join threads
			for (unsigned int i = 0; i < this->threads.size(); i++)
			{
				this->threads[i]->join();
				delete this->threads[i];
				this->threads[i] = nullptr;
				this->num_threads--;
			}
			this->threads.clear();
		}
	}
	void CThreadPool::Execute(void)
	{
		IWorkerThread* wrk;
		while (this->run)
		{
			//Get work to do
			this->GetWork(&wrk);

			//If work is invalid don't do it
			if (wrk == nullptr)
				continue;

			//Do the work
			wrk->Work();

			//Delete the work
			delete wrk;

			//Decrement the Sync Semaphore
			wrk_done_Sem.Dec();
		}

		//Shutdown!!!---------------------
		//Do all remaining tasks (this will only happen if there is work in the queue
		//Sync queue
		int wsize = 0;
		do
		{
			this->queue_Mutex.lock();
			wsize = this->workers.size();
			this->queue_Mutex.unlock();

			if (wsize != 0)
			{
				//Get work to do
				this->GetWork(&wrk);

				//If work is invalid don't do it
				if (wrk == nullptr)
					continue;

				//Do the work
				wrk->Work();

				//Delete the work
				delete wrk;

				//Decrement the Sync Semaphore
				wrk_done_Sem.Dec();
			}
		} while (wsize != 0);
	}
	void CThreadPool::PostWork(IWorkerThread* _worker_ptr)
	{
		//Don't do anything if null
		if (_worker_ptr == nullptr)
			return;

		if (!kbSingleThreaded)
		{
			//Sync the queue
			this->queue_Mutex.lock();

			//Add Worker Thread to the queue
			this->workers.push(_worker_ptr);

			//Increment the Sync Semaphore
			wrk_done_Sem.Inc();

			//Sync the queue
			this->queue_Mutex.unlock();

			//Post the work
			this->wrk_ready_Sem.Post();
		}
		else
		{
			//If in single threaded environment, then do the work immediately and don't return until finished
			_worker_ptr->Work();
			//Delete the work
			delete _worker_ptr;
		}
	}
	void CThreadPool::GetWork(IWorkerThread** _worker_ptr)
	{
		(*_worker_ptr) = nullptr;

		//Put threads to sleep if there isn't any work
		this->wrk_ready_Sem.Wait();

		//Sync the queue
		this->queue_Mutex.lock();

		//Check to be sure there is work to do
		if (this->workers.size() == 0)
		{
			this->queue_Mutex.unlock();
			return;
		}

		//Hand off work to a free thread
		(*_worker_ptr) = this->workers.front();
		this->workers.pop();

		//Sync the queue
		this->queue_Mutex.unlock();
	}
	bool CThreadPool::Sync(bool bBlock)
	{
		if (!kbSingleThreaded)	//Waits until all work is completed
			return this->wrk_done_Sem.Wait();
		else
			return true;
	}
}


//static ThreadPool::CThreadPool pool;

namespace Pool
{
	void Post(ThreadPool::IWorkerThread* pWorker)
	{
		//pool.PostWork(pWorker);
	}
	void SetShutdownMethod(ThreadPool::eShutdownType eShutdown)
	{
		//pool.SetShutdownMethod(eShutdown);
	}
	bool Sync(bool bBlocking)
	{
		//return pool.Sync(bBlocking);
		return false;
	}
}
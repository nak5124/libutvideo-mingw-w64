/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: Thread.cpp 1122 2013-12-21 08:13:20Z umezawa $ */

#include "stdafx.h"
#include "Thread.h"

#ifdef _WIN32

int CThreadManager::GetNumProcessors(void)
{
	DWORD_PTR dwpProcessAffinityMask;
	DWORD_PTR dwpSystemAffinityMask;
	int nNumProcessors;

	GetProcessAffinityMask(GetCurrentProcess(), &dwpProcessAffinityMask, &dwpSystemAffinityMask);
	nNumProcessors = 0;
	for (DWORD_PTR dwpAffinityMask = 1; dwpAffinityMask != 0; dwpAffinityMask <<= 1)
	{
		if (dwpProcessAffinityMask & dwpAffinityMask)
			nNumProcessors++;
	}

	return nNumProcessors;
}

CThreadManager::CThreadManager(void)
{
	DWORD_PTR dwpProcessAffinityMask;
	DWORD_PTR dwpSystemAffinityMask;

	_RPT1(_CRT_WARN, "enter CThreadManager::CThreadManager() this=%p\n", this);
	m_nNumThreads = 0;
	m_nNumJobs = 0;
	for (int i = 0; i < MAX_THREAD; i++)
	{
		m_hThread[i] = NULL;
		m_hThreadSemaphore[i] = NULL;
	}

	GetProcessAffinityMask(GetCurrentProcess(), &dwpProcessAffinityMask, &dwpSystemAffinityMask);
	for (DWORD_PTR dwpThreadAffinityMask = 1; dwpThreadAffinityMask != 0; dwpThreadAffinityMask <<= 1)
	{
		if (dwpProcessAffinityMask & dwpThreadAffinityMask)
		{
			m_hThread[m_nNumThreads] = CreateThread(NULL, 0, StaticThreadProc, this, CREATE_SUSPENDED, &m_dwThreadId[m_nNumThreads]);
			SetThreadAffinityMask(m_hThread[m_nNumThreads], dwpThreadAffinityMask);
			m_hThreadSemaphore[m_nNumThreads] = CreateSemaphore(NULL, 0, 0x7fffffff, NULL);
			m_nNumThreads++;
		}
	}

	InitializeCriticalSection(&m_csJob);

	for (int i = 0; i < m_nNumThreads; i++)
		ResumeThread(m_hThread[i]);
	_RPT1(_CRT_WARN, "leave CThreadManager::CThreadManager() this=%p\n", this);
}

CThreadManager::~CThreadManager(void)
{
	_ASSERT(m_nNumJobs == 0);
	_RPT1(_CRT_WARN, "enter CThreadManager::~CThreadManager() this=%p\n", this);
	EnterCriticalSection(&m_csJob);
	for (int i = 0; i < m_nNumThreads; i++)
	{
		m_queueJob[i].push(NULL);
		ReleaseSemaphore(m_hThreadSemaphore[i], 1, 0);
	}
	LeaveCriticalSection(&m_csJob);

	WaitForMultipleObjects(m_nNumThreads, m_hThread, TRUE, INFINITE);
	for (int i = 0; i < m_nNumThreads; i++)
	{
		CloseHandle(m_hThread[i]);
		CloseHandle(m_hThreadSemaphore[i]);
	}
	DeleteCriticalSection(&m_csJob);
	_RPT1(_CRT_WARN, "leave CThreadManager::~CThreadManager() this=%p\n", this);
}

DWORD WINAPI CThreadManager::StaticThreadProc(LPVOID lpParameter)
{
	CThreadManager *pThis = (CThreadManager *)lpParameter;
	DWORD dwThreadId;
	int nThreadIndex;

	dwThreadId = GetCurrentThreadId();
	for (nThreadIndex = 0; nThreadIndex < pThis->m_nNumThreads; nThreadIndex++)
		if (pThis->m_dwThreadId[nThreadIndex] == dwThreadId)
			break;
	_ASSERT(nThreadIndex < pThis->m_nNumThreads);
	return pThis->ThreadProc(nThreadIndex);
}

DWORD CThreadManager::ThreadProc(int nThreadIndex)
{
	_RPT3(_CRT_WARN, "enter CThreadManager::ThreadProc() this=%p index=%d ID=%08X\n", this, nThreadIndex, GetCurrentThreadId());
	for (;;)
	{
		CThreadJob *pJob;

		WaitForSingleObject(m_hThreadSemaphore[nThreadIndex], INFINITE);
		EnterCriticalSection(&m_csJob);
		pJob = m_queueJob[nThreadIndex].front();
		m_queueJob[nThreadIndex].pop();
		LeaveCriticalSection(&m_csJob);
		if (pJob == NULL)
			break;
		pJob->JobProc(this);
		SetEvent(pJob->m_hCompletionEvent);
		delete pJob;
	}
	_RPT3(_CRT_WARN, "leave CThreadManager::ThreadProc() this=%p index=%d ID=%08X\n", this, nThreadIndex, GetCurrentThreadId());
	return 0;
}

void CThreadManager::SubmitJob(CThreadJob *pJob, uint32_t dwAffinityHint)
{
	HANDLE hCompletionEvent;
	int nThreadIndex;

	_ASSERT(pJob != NULL);
	_ASSERT(m_nNumJobs < MAX_JOB);

	hCompletionEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	pJob->m_hCompletionEvent = hCompletionEvent;
	nThreadIndex = dwAffinityHint % m_nNumThreads;
	m_hCompletionEvent[m_nNumJobs++] = hCompletionEvent;
	EnterCriticalSection(&m_csJob);
	m_queueJob[nThreadIndex].push(pJob);
	LeaveCriticalSection(&m_csJob);
	ReleaseSemaphore(m_hThreadSemaphore[nThreadIndex], 1, NULL);
}

void CThreadManager::WaitForJobCompletion(void)
{
	// �ҋ@���ɃW���u���ǉ�����邱�Ƃ͍l�����Ă��Ȃ��B
	WaitForMultipleObjects(m_nNumJobs, m_hCompletionEvent, TRUE, INFINITE);
	for (int i = 0; i < m_nNumJobs; i++)
		CloseHandle(m_hCompletionEvent[i]);
	m_nNumJobs = 0;
}

#endif

#if defined(__APPLE__) || defined(__unix__)

int CThreadManager::GetNumProcessors(void)
{
	return sysconf(_SC_NPROCESSORS_ONLN); // XXX
}

CThreadManager::CThreadManager(void)
{
	m_nNumJobs = 0;
	m_nNumCompleteJobs = 0;
	for (int i = 0; i < MAX_THREAD; i++)
		m_ptidThread[i] = 0;

	pthread_mutex_init(&m_ptmJobMutex, NULL);
	pthread_cond_init(&m_ptcJobCond, NULL);

	m_nNumThreads = sysconf(_SC_NPROCESSORS_ONLN);
	if (m_nNumThreads > MAX_THREAD)
		m_nNumThreads = MAX_THREAD;
	for (int i = 0; i < m_nNumThreads; i++)
	{
		pthread_create(&m_ptidThread[i], NULL, StaticThreadProc, this);
	}
}

CThreadManager::~CThreadManager(void)
{
	pthread_mutex_lock(&m_ptmJobMutex);
	for (int i = 0; i < m_nNumThreads; i++)
		m_queueJob.push(NULL);
	m_nNumJobs = m_nNumThreads;
	pthread_cond_broadcast(&m_ptcJobCond);
	pthread_mutex_unlock(&m_ptmJobMutex);

	for (int i = 0; i < m_nNumThreads; i++)
		pthread_join(m_ptidThread[i], NULL);

	pthread_mutex_destroy(&m_ptmJobMutex);
	pthread_cond_destroy(&m_ptcJobCond);
}

void *CThreadManager::StaticThreadProc(void *arg)
{
	CThreadManager *pThis = (CThreadManager *)arg;

	return pThis->ThreadProc(0);
}

void *CThreadManager::ThreadProc(int nThreadIndex)
{
	CThreadJob *pJob;

	for (;;)
	{
		pthread_mutex_lock(&m_ptmJobMutex);
		while (m_queueJob.size() == 0)
			pthread_cond_wait(&m_ptcJobCond, &m_ptmJobMutex);
		pJob = m_queueJob.front();
		m_queueJob.pop();
		pthread_mutex_unlock(&m_ptmJobMutex);
		if (pJob == NULL)
			break;
		pJob->JobProc(this);
		pthread_mutex_lock(&m_ptmJobMutex);
		m_nNumCompleteJobs++;
		pthread_cond_broadcast(&m_ptcJobCond);
		pthread_mutex_unlock(&m_ptmJobMutex);
		delete pJob;
	}

	return NULL;
}

void CThreadManager::SubmitJob(CThreadJob *pJob, uint32_t dwAffinityHint)
{
	pthread_mutex_lock(&m_ptmJobMutex);
	m_queueJob.push(pJob);
	m_nNumJobs++;
	pthread_cond_signal(&m_ptcJobCond);
	pthread_mutex_unlock(&m_ptmJobMutex);
}

void CThreadManager::WaitForJobCompletion(void)
{
	pthread_mutex_lock(&m_ptmJobMutex);
	while (m_nNumCompleteJobs != m_nNumJobs)
		pthread_cond_wait(&m_ptcJobCond, &m_ptmJobMutex);
	pthread_mutex_unlock(&m_ptmJobMutex);

	m_nNumJobs = 0;
	m_nNumCompleteJobs = 0;
}

#endif

CThreadJob::CThreadJob(void)
{
}

CThreadJob::~CThreadJob(void)
{
}

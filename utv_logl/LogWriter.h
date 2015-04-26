/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: LogWriter.h 1283 2015-04-17 16:42:10Z umezawa $ */

#pragma once

#ifndef DLLEXPORT
#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif
#endif

int InitializeLogWriter(void);
int DLLEXPORT WriteLog(const char *p);
int UninitializeLogWriter(void);


#if defined(_WIN32)

// �֐��̏ꍇ�̓C���|�[�g���鑤�� dllexport �œ����Ă��܂����A�ϐ��̏ꍇ�͂����͂����Ȃ��B
#ifdef LOGWRITER_OWNER
extern __declspec(dllexport) HANDLE hLogPipe;
#else
extern __declspec(dllimport) HANDLE hLogPipe;
#endif

static inline bool IsLogWriterInitialized()
{
	return ::hLogPipe != INVALID_HANDLE_VALUE;
}

static inline void OutputDebugLog(const char *str)
{
	OutputDebugStringA(str);
}

#elif defined(__APPLE__) || defined(__unix__)

extern int fdLogSock;

static inline bool IsLogWriterInitialized()
{
	return ::fdLogSock != -1;
}

static inline void OutputDebugLog(const char *str)
{
	fprintf(stderr, "%s", str); // XXX
}

#endif


#ifdef _DEBUG

static inline bool IsLogWriterInitializedOrDebugBuild()
{
	return true;
}

#define LOGPRINTF(...) \
	do \
	{ \
		char __LOGPRINTF_local_buf1__[256]; \
		char __LOGPRINTF_local_buf2__[256]; \
		sprintf(__LOGPRINTF_local_buf1__, __VA_ARGS__); \
		sprintf(__LOGPRINTF_local_buf2__, "<%s> %s", LOG_MODULE_NAME, __LOGPRINTF_local_buf1__); \
		if (IsLogWriterInitialized()) \
		{ \
			WriteLog(__LOGPRINTF_local_buf2__); \
		} \
		strcat(__LOGPRINTF_local_buf2__, "\n"); \
		OutputDebugLog(__LOGPRINTF_local_buf2__); \
	} while (false)

#define DBGPRINTF(...) \
	do \
	{ \
		char __LOGPRINTF_local_buf1__[256]; \
		char __LOGPRINTF_local_buf2__[256]; \
		sprintf(__LOGPRINTF_local_buf1__, __VA_ARGS__); \
		sprintf(__LOGPRINTF_local_buf2__, "<%s> %s", LOG_MODULE_NAME, __LOGPRINTF_local_buf1__); \
		strcat(__LOGPRINTF_local_buf2__, "\n"); \
		OutputDebugLog(__LOGPRINTF_local_buf2__); \
	} while (false)

#else

static inline bool IsLogWriterInitializedOrDebugBuild()
{
	return IsLogWriterInitialized();
}

#define LOGPRINTF(...) \
	do \
	{ \
		if (IsLogWriterInitialized()) \
		{ \
			char __LOGPRINTF_local_buf1__[256]; \
			char __LOGPRINTF_local_buf2__[256]; \
			sprintf(__LOGPRINTF_local_buf1__, __VA_ARGS__); \
			sprintf(__LOGPRINTF_local_buf2__, "<%s> %s", LOG_MODULE_NAME, __LOGPRINTF_local_buf1__); \
			WriteLog(__LOGPRINTF_local_buf2__); \
		} \
	} while (false)

#define DBGPRINTF(...) \
	do \
	{ \
	} while (false)

#endif

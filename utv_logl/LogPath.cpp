/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: LogPath.cpp 1283 2015-04-17 16:42:10Z umezawa $ */

#include "stdafx.h"
#include "LogPath.h"

#if defined(_WIN32)

int GetLogSocketPath(char *pszPath, size_t cchPath)
{
	HANDLE hToken;
	char *buf[256];
	TOKEN_USER *tu = (TOKEN_USER *)buf;
	DWORD cb = sizeof(buf);
	char *pszSid;

	if (!OpenProcessToken(GetCurrentProcess(), GENERIC_READ, &hToken))
		return -1;

	if (!GetTokenInformation(hToken, TokenUser, tu, cb, &cb))
	{
		DWORD err = GetLastError();
		CloseHandle(hToken);
		SetLastError(err);
		return -1;
	}

	if (!ConvertSidToStringSid(tu->User.Sid, &pszSid))
	{
		DWORD err = GetLastError();
		CloseHandle(hToken);
		SetLastError(err);
		return -1;
	}

	sprintf(pszPath, "\\\\.\\pipe\\utvideo-log-%s", pszSid);

	LocalFree(pszSid);
	CloseHandle(hToken);
	return 0;
}

#elif defined(__APPLE__) || defined(__unix__)

int GetLogSocketPath(char *pszPath, size_t cchPath)
{
	sprintf(pszPath, "/tmp/utvideo-log-%d", getuid());
	return 0;
}

#endif

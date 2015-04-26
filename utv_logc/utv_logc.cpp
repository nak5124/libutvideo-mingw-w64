/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: utv_logc.cpp 1286 2015-04-18 11:14:25Z umezawa $ */

#include "stdafx.h"

#include <LogReader.h>


void OpenProc(int id, int idx, const char *ident);
void ReadProc(int id, int idx, const char *ident, const char *buf);
void CloseProc(int id, int idx, const char *ident);

int main(int argc, char **argv)
{
	InitializeLogReader(OpenProc, ReadProc, CloseProc, true);
#if defined(_WIN32)
	printf("%d\n", GetLastError());
#elif defined(__APPLE__) || defined(__unix__)
	printf("%s (%d)\n", strerror(errno), errno);
#endif
	return 0;
}

void OpenProc(int id, int idx, const char *ident)
{
	printf("{%d} %s: OPEN\n", id, ident);
}

void ReadProc(int id, int idx, const char *ident, const char *buf)
{
	printf("{%d} %s: %s\n", id, ident, buf);
}

void CloseProc(int id, int idx, const char *ident)
{
	printf("{%d} %s: CLOSE\n", id, ident);
}

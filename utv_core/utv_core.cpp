/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: utv_core.cpp 1289 2015-04-18 14:34:30Z umezawa $ */

#include "stdafx.h"
#include "utvideo.h"
#include "TunedFunc.h"

#ifdef _WIN32

HMODULE hModule;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		LOGPRINTF("DllMain(hModule=%p, dwReason=DLL_PROCESS_ATTACH, lpReserved=%p)", hModule, lpReserved);
		::hModule = hModule;
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		LOGPRINTF("DllMain(hModule=%p, dwReason=DLL_PROCESS_DETACH, lpReserved=%p)", hModule, lpReserved);
	}

	return TRUE;
}

#endif

#ifdef _DEBUG

DLLEXPORT void UnitTest_core(void)
{
}

#endif

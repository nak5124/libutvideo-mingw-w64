/* $Id: utv_core.cpp 974 2013-03-20 14:07:51Z umezawa $ */

#include "stdafx.h"
#include "utvideo.h"
#include "TunedFunc.h"

#ifdef _WIN32

HMODULE hModule;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    //_RPT3(_CRT_WARN, "DllMain(HMODULE, DWORD, LPVOID) hModule=%p dwReason=%08X lpReserved=%p\n", hModule, dwReason, lpReserved);

    if (dwReason == DLL_PROCESS_ATTACH)
    {
        ::hModule = hModule;
    }

    return TRUE;
}

#endif

#ifdef _DEBUG

DLLEXPORT void UnitTest_core(void)
{
}

#endif

/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: utv_qt.cpp 1289 2015-04-18 14:34:30Z umezawa $ */

// utv_qt.cpp : DLL �A�v���P�[�V�����p�ɃG�N�X�|�[�g�����֐����`���܂��B
//

#include "stdafx.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		LOGPRINTF("DllMain(hModule=%p, dwReason=DLL_PROCESS_ATTACH, lpReserved=%p)", hModule, lpReserved);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		LOGPRINTF("DllMain(hModule=%p, dwReason=DLL_PROCESS_DETACH, lpReserved=%p)", hModule, lpReserved);
	}

	return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif


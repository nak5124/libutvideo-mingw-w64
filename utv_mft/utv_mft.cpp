/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: utv_mft.cpp 1289 2015-04-18 14:34:30Z umezawa $ */

// utv_mft.cpp : DLL �G�N�X�|�[�g�̎����ł��B


#include "stdafx.h"
#include "resource.h"
#include "utv_mft.h"


class CUtVideoMFTModule : public CAtlDllModuleT<CUtVideoMFTModule>
{
public :
	DECLARE_LIBID(LIBID_UtVideoMFT)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_UTVIDEOMFT, "{274346FB-B683-4FE3-905A-0A63165E1F98}")
};

CUtVideoMFTModule _AtlModule;


#ifdef _MANAGED
#pragma managed(push, off)
#endif

// DLL �G���g�� �|�C���g
extern "C" BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		LOGPRINTF("DllMain(hModule=%p, dwReason=DLL_PROCESS_ATTACH, lpReserved=%p)", hModule, lpReserved);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		LOGPRINTF("DllMain(hModule=%p, dwReason=DLL_PROCESS_DETACH, lpReserved=%p)", hModule, lpReserved);
	}

	return _AtlModule.DllMain(dwReason, lpReserved);
}

#ifdef _MANAGED
#pragma managed(pop)
#endif




// DLL �� OLE �ɂ���ăA�����[�h�ł���悤�ɂ��邩�ǂ������w�肵�܂��B
STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}


// �v�����ꂽ�^�̃I�u�W�F�N�g���쐬����N���X �t�@�N�g����Ԃ��܂��B
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - �G���g�����V�X�e�� ���W�X�g���ɒǉ����܂��B
STDAPI DllRegisterServer(void)
{
    // �I�u�W�F�N�g�A�^�C�v ���C�u��������у^�C�v ���C�u�������̂��ׂẴC���^�[�t�F�C�X��o�^���܂�
    HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}


// DllUnregisterServer - �G���g�������W�X�g������폜���܂��B
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}


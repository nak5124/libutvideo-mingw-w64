/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: utv_cfg.cpp 567 2010-08-30 10:07:07Z umezawa $ */

// utv_cfg.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "utv_cfg.h"
#include <GlobalConfig.h>

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	return (int)GlobalConfigDialog(NULL);
}

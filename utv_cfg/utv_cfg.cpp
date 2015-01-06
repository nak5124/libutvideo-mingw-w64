/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id: utv_cfg.cpp 567 2010-08-30 10:07:07Z umezawa $ */

// utv_cfg.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "utv_cfg.h"
#include <GlobalConfig.h>

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	return (int)GlobalConfigDialog(NULL);
}

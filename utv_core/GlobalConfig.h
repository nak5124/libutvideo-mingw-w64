/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id: GlobalConfig.h 737 2011-09-03 08:09:45Z umezawa $ */

#pragma once

#ifdef _WIN32
__declspec(dllexport) INT_PTR GlobalConfigDialog(HWND hwnd);
#endif

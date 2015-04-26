/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: stdafx.h 1262 2015-03-28 18:33:39Z umezawa $ */

#pragma once

#ifdef _WIN32

// ���Ŏw�肳�ꂽ��`�̑O�ɑΏۃv���b�g�t�H�[�����w�肵�Ȃ���΂Ȃ�Ȃ��ꍇ�A�ȉ��̒�`��ύX���Ă��������B
// �قȂ�v���b�g�t�H�[���ɑΉ�����l�Ɋւ���ŐV���ɂ��ẮAMSDN ���Q�Ƃ��Ă��������B
#ifndef WINVER				// Windows XP �ȍ~�̃o�[�W�����ɌŗL�̋@�\�̎g�p�������܂��B
#define WINVER 0x0501		// ����� Windows �̑��̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif

#ifndef _WIN32_WINNT		// Windows XP �ȍ~�̃o�[�W�����ɌŗL�̋@�\�̎g�p�������܂��B                   
#define _WIN32_WINNT 0x0501	// ����� Windows �̑��̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif						

#ifndef _WIN32_WINDOWS		// Windows 98 �ȍ~�̃o�[�W�����ɌŗL�̋@�\�̎g�p�������܂��B
#define _WIN32_WINDOWS 0x0410 // ����� Windows Me �܂��͂���ȍ~�̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif

#ifndef _WIN32_IE			// IE 6.0 �ȍ~�̃o�[�W�����ɌŗL�̋@�\�̎g�p�������܂��B
#define _WIN32_IE 0x0600	// ����� IE. �̑��̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif

#define WIN32_LEAN_AND_MEAN		// Windows �w�b�_�[����g�p����Ă��Ȃ����������O���܂��B
#define _CRT_SECURE_NO_WARNINGS

// Windows �w�b�_�[ �t�@�C��:
#include <windows.h>

inline BOOL EnableDlgItem(HWND hwndParent, UINT nID, BOOL bEnable)
{
	return EnableWindow(GetDlgItem(hwndParent, nID), bEnable);
}

#include <stdio.h>

#endif

#if defined(__APPLE__) || defined(__unix__)
#include <sys/types.h>
#include <sys/mman.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#endif

#ifndef _MSC_VER // XXX
#define _countof(x) (sizeof(x) / sizeof((x)[0]))
#define _ASSERT(x) do {} while(/*CONSTCOND*/0)
#endif

#include <queue>
#include <algorithm>
using namespace std;

#include <stdint.h>
#include <myinttypes.h>

inline uint32_t ROUNDUP(uint32_t a, uint32_t b)
{
	_ASSERT(b > 0 && (b & (b - 1)) == 0); // b �� 2 �̗ݏ�ł���B
	return ((a + b - 1) / b) * b;
}

inline uint64_t ROUNDUP(uint64_t a, uint64_t b)
{
	_ASSERT(b > 0 && (b & (b - 1)) == 0); // b �� 2 �̗ݏ�ł���B
	return ((a + b - 1) / b) * b;
}

#ifdef __APPLE__
inline size_t ROUNDUP(size_t a, size_t b)
{
	_ASSERT(b > 0 && (b & (b - 1)) == 0); // b �� 2 �̗ݏ�ł���B
	return ((a + b - 1) / b) * b;
}
#endif

inline bool IS_ALIGNED(uintptr_t v, uintptr_t a)
{
	_ASSERT(a > 0 && (a & (a - 1)) == 0); // a �� 2 �̗ݏ�ł���B
	return (v & (a - 1)) == 0; // v �� a �̔{���ł���B
}

inline bool IS_ALIGNED(const void *p, uintptr_t a)
{
	return IS_ALIGNED((uintptr_t)p, a);
}

#define LOG_MODULE_NAME "utv_core"
#define LOGWRITER_OWNER 1
#include <LogWriter.h>
#include <LogUtil.h>

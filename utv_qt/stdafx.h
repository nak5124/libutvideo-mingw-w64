/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: stdafx.h 1275 2015-04-05 14:06:51Z umezawa $ */

// stdafx.h : �W���̃V�X�e�� �C���N���[�h �t�@�C���̃C���N���[�h �t�@�C���A�܂���
// �Q�Ɖ񐔂������A�����܂�ύX����Ȃ��A�v���W�F�N�g��p�̃C���N���[�h �t�@�C��
// ���L�q���܂��B
//

#pragma once

#define _CRT_SECURE_NO_WARNINGS 1

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <myinttypes.h>

#ifdef _WIN32
#include <windows.h>
#endif

/*
* QuickTime SDK for Windows �Ɋ܂܂��A
* �Â� Visual C++ �̂��߂ɗp�ӂ���Ă���
* <GNUCompatibility/stdint.h> ��
* <GNUCompatibility/stdbool.h> ��
* include ��j�~����B
*/
#ifdef _MSC_VER
#define _STDINT_H 1
#define __STDBOOL_H__ 1
#endif

/*
* Windows 8/Server2012 �� GetProcessInformation ���ǉ�����Ă��āA
* ���ꂪ�Փ˂���̂� Mac �̕������l�[������B
* �ǂ�����Ă΂Ȃ��̂ŃR���p�C���G���[�ɂȂ�Ȃ���΂���ł����B
*/
#ifdef _WIN32
#define GetProcessInformation MacGetProcessInformation
#endif

/*
* QuickTime SDK for Windows ��
* QuickTime SDK for Mac �ƂŃf�B���N�g�����Ⴄ�B
* ���������̂�߂Ăق����B
*/
#ifdef _WIN32
#include <QuickTimeComponents.h>
#include <ImageCodec.h>
#else
#include <QuickTime/QuickTimeComponents.h>
#include <QuickTime/ImageCodec.h>
#endif

#define LOG_MODULE_NAME "utv_qt  "

#include <LogWriter.h>
#include <LogUtil.h>

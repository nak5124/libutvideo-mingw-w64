/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: stdafx.h 1208 2014-12-29 01:38:56Z umezawa $ */

// stdafx.h : �W���̃V�X�e�� �C���N���[�h �t�@�C���̃C���N���[�h �t�@�C���A�܂���
// �Q�Ɖ񐔂������A�����܂�ύX����Ȃ��A�v���W�F�N�g��p�̃C���N���[�h �t�@�C��
// ���L�q���܂��B
//

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

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

/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: QuickTimeFormat.h 1203 2014-12-27 06:05:50Z umezawa $ */

#pragma once

int UtVideoFormatToQuickTimeFormat(OSType *pixelFormat, utvf_t utvf);
int QuickTimeFormatToUtVideoFormat(utvf_t *utvf, OSType pixelFormat);

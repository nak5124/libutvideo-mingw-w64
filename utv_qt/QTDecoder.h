/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id: QTDecoder.h 1208 2014-12-29 01:38:56Z umezawa $ */

#pragma once

#include "QTCodec.h"
#include "Codec.h"
#include "Mutex.h"

struct CQTDecoder : public CQTCodec
{
	OSType **wantedDestinationPixelTypes;
	int beginBandDone;
};

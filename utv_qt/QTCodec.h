/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id: QTCodec.h 1208 2014-12-29 01:38:56Z umezawa $ */

#pragma once

#include "Codec.h"
#include "Mutex.h"

struct CQTCodec
{
	ComponentInstance self;
	ComponentInstance delegateComponent;
	ComponentInstance target;
	OSType componentSubType;
	CCodec *codec;
	CMutex *mutex;
};

pascal ComponentResult QTCodecOpen(CQTCodec *glob, ComponentInstance self);
pascal ComponentResult QTCodecClose(CQTCodec *glob, ComponentInstance self);
pascal ComponentResult QTCodecVersion(CQTCodec *glob);
pascal ComponentResult QTCodecTarget(CQTCodec *glob, ComponentInstance target);
pascal ComponentResult QTCodecGetCodecInfo(CQTCodec *glob, CodecInfo *info);

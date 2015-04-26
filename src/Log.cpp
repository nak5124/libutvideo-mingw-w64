/* $Id: Log.cpp 1245 2015-03-18 15:04:35Z umezawa $ */

#include "stdafx.h"
#include "utvideo.h"
#include "Log.h"

CLogInitializer::CLogInitializer()
{
    Initialize();
}

CLogInitializer::~CLogInitializer()
{
    UninitializeLogWriter();
}

void CLogInitializer::Initialize()
{
    if (IsLogWriterInitialized())
        return;

    InitializeLogWriter();

    LOGPRINTF("Ut Video Codec Suite %s (%s)", UTVIDEO_VERSION_STR, UTVIDEO_IMPLEMENTATION_STR);
}

CLogInitializer CLogInitializer::__li__;

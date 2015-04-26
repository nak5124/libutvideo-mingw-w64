/* $Id: Log.h 1245 2015-03-18 15:04:35Z umezawa $ */

#pragma once

class CLogInitializer
{
private:
    CLogInitializer();
    ~CLogInitializer();
    static CLogInitializer __li__;

public:
    static void Initialize();
};

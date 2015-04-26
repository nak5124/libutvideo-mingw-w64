/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
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

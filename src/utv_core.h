/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id: utv_core.h 807 2011-11-26 17:36:37Z umezawa $ */

#pragma once

#ifdef _WIN32
extern HINSTANCE hModule;
#endif

#ifdef _DEBUG
DLLEXPORT void UnitTest_core(void);
#endif

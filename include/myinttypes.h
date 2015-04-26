/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id: myinttypes.h 1289 2015-04-18 14:34:30Z umezawa $ */

#pragma once

#include <stdint.h>
#include <inttypes.h>

#if defined(_MSC_VER)
#define PRISZT_PREFIX "I"
#elif defined(__GNUC__)
#define PRISZT_PREFIX "z"
#else
#error
#endif

#define PRIdSZT PRISZT_PREFIX "d"
#define PRIuSZT PRISZT_PREFIX "u"
#define PRIoSZT PRISZT_PREFIX "o"
#define PRIxSZT PRISZT_PREFIX "x"
#define PRIXSZT PRISZT_PREFIX "X"

/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id: LogReader.h 1237 2015-03-17 14:44:25Z umezawa $ */

#pragma once

typedef void (*LogReaderOpenProc)(int id, int idx, const char *ident);
typedef void (*LogReaderReadProc)(int id, int idx, const char *ident, const char *msg);
typedef void (*LogReaderCloseProc)(int id, int idx, const char *ident);

int InitializeLogReader(LogReaderOpenProc fnOpenProc, LogReaderReadProc fnReadProc, LogReaderCloseProc fnCloseProc, bool bLoop);
int UninitializeLogReader(void);

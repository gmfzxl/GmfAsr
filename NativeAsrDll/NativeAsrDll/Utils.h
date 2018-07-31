#pragma once
#include "stdafx.h"
#include <stdio.h>
#include <tchar.h>
#include <time.h>
#include "stdlib.h"

#define UnicodeToUtf8(src, dest)      Convert((void *)(src), 0, (void **)(dest), 1)
#define UnicodeToGb2312(src, dest)    Convert((void *)(src), 0, (void **)(dest), 2)
#define UnicodeToGbk(src, dest)       Convert((void *)(src), 0, (void **)(dest), 3)
#define UnicodeToBig5(src, dest)      Convert((void *)(src), 0, (void **)(dest), 4)

#define Utf8ToUnicode(src, dest)      Convert((void *)(src), 1, (void **)(dest), 0)
#define Utf8ToGb2312(src, dest)       Convert((void *)(src), 1, (void **)(dest), 2)
#define Utf8ToGbk(src, dest)          Convert((void *)(src), 1, (void **)(dest), 3)
#define Utf8ToBig5(src, dest)         Convert((void *)(src), 1, (void **)(dest), 4)

#define Gb2312ToUnicode(src, dest)    Convert((void *)(src), 2, (void **)(dest), 0)
#define Gb2312ToUtf8(src, dest)       Convert((void *)(src), 2, (void **)(dest), 1)
#define Gb2312ToGbk(src, dest)        Convert((void *)(src), 2, (void **)(dest), 3)
#define Gb2312ToBig5(src, dest)       Convert((void *)(src), 2, (void **)(dest), 4)

#define GbkToUnicode(src, dest)       Convert((void *)(src), 3, (void **)(dest), 0)
#define GbkToUtf8(src, dest)          Convert((void *)(src), 3, (void **)(dest), 1)
#define GbkToGb2312(src, dest)        Convert((void *)(src), 3, (void **)(dest), 2)
#define GbkToBig5(src, dest)          Convert((void *)(src), 3, (void **)(dest), 4)

#define Big5ToUnicode(src, dest)      Convert((void *)(src), 4, (void **)(dest), 0)
#define Big5ToUtf8(src, dest)         Convert((void *)(src), 4, (void **)(dest), 1)
#define Big5ToGb2312(src, dest)       Convert((void *)(src), 4, (void **)(dest), 2)
#define Big5ToGbk(src, dest)          Convert((void *)(src), 4, (void **)(dest), 3)


using namespace std;
void OutputDebugStringEx(const wchar_t *strOutputString, ...);
void OutputDebugStringEx(const char *strOutputString, ...);
char * getTime();
int Convert(void *sstr, int scp, void **dstr, int dcp);
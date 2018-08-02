#pragma once
#include "stdafx.h"
#include <stdio.h>
#include <tchar.h>
#include <time.h>
#include "stdlib.h"
#define WAV_HEADER_LEN 44

#pragma pack(1)
typedef struct
{
	char riff[4];                 // "RIFF" 标志  
	int  totalLen;                // 文件长度      
	char wave[4];                 // "WAVE" 标志  
	char fmt[4];                  // "fmt" 标志 
	int  dwFMTLen;                // 过渡字节（不定）  一般为16
	short fmtPcm;                 // 格式类别  
	short  channels;              // 声道数  
	int fmtSamplehz;              // 采样率 
	int fmtBytepsec;              // 位速  
	short fmtBlockAlign;          // 一个采样多声道数据块大小  
	short fmtBitPerSample;        // 一个采样占的 bit 数  
	char data[4];                 // 数据标记符＂data ＂  
	int  dwDATALen;               // 语音数据的长度，比文件长度小44一般
}WaveHeader;

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
void copyStr(char ** dest, char * src);
void saveFree(void **str);
void addWaveHeader(char * filePath, int samplesPerSec, int bitsPerSample, int channle);
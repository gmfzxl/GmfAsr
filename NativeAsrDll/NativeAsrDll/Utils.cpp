#include "stdafx.h"
#include "Utils.h"
#include <time.h>
#include <stdio.h>
#include <tchar.h>
#include <stdarg.h>
#include<Windows.h>   


void OutputDebugStringEx(const char *strOutputString, ...)
{
	va_list vlArgs = NULL;
	va_start(vlArgs, strOutputString);
	size_t nLen = _vscprintf(strOutputString, vlArgs) + 1;
	char *strBuffer = new char[nLen];
	_vsnprintf_s(strBuffer, nLen, nLen, strOutputString, vlArgs);
	va_end(vlArgs);
	OutputDebugStringA(strBuffer);
	delete[] strBuffer;
}

void OutputDebugStringEx(const wchar_t *strOutputString, ...)

{
	va_list vlArgs = NULL;
	va_start(vlArgs, strOutputString);
	size_t nLen = _vscwprintf(strOutputString, vlArgs) + 1;
	wchar_t *strBuffer = new wchar_t[nLen + 8];
	_vsnwprintf_s(strBuffer, nLen, nLen, strOutputString, vlArgs);
	va_end(vlArgs);
	OutputDebugStringW(strBuffer);
	delete[] strBuffer;
}

char * getTime()
{
	time_t timep;
	time(&timep);
	char * tmp = (char *)malloc(sizeof(char) * 64);
	strftime(tmp, sizeof(char) * 64, "%Y%m%d%H%M%S", localtime(&timep));
	return tmp;
}

int Convert(void *sstr, int scp, void **dstr, int dcp)
{
#define CP_GBK        936
#define CP_BIG5        950
#define CP_UTF8        65001

	enum { _unicode, _utf8, _gb2312, _gbk, _big5 };
	enum { _wc2mb, _mb2wc, _sc2tc, _tc2sc };

	LCID lcid;
	void *src;
	void *dest;
	int cch;
	int scp0;
	int act;
	UINT cp;

	if (((scp < _unicode) || (scp > _big5)) ||
		((dcp < _unicode) || (dcp > _big5)))
		return -1;

	src = NULL;
	dest = sstr;
	cch = 0;
	scp0 = scp;

	while (scp != dcp)
	{
		src = dest;
		switch (scp)
		{
		case _unicode:
			switch (dcp)
			{
			case _utf8:
				scp = _utf8;
				act = _wc2mb;
				cp = CP_UTF8;
				break;
			case _gb2312:
				scp = ((scp0 == _big5) ? _gbk : _gb2312);
				act = _wc2mb;
				cp = CP_GBK;
				break;
			case _gbk:
				scp = _gbk;
				act = _wc2mb;
				cp = CP_GBK;
				break;
			case _big5:
				scp = _big5;
				act = _wc2mb;
				cp = CP_BIG5;
				break;
			}
			break;
		case _utf8:
			switch (dcp)
			{
			case _unicode:
			case _gb2312:
			case _gbk:
			case _big5:
				scp = _unicode;
				act = _mb2wc;
				cp = CP_UTF8;
				break;
			}
			break;
		case _gb2312:
			switch (dcp)
			{
			case _unicode:
			case _utf8:
				scp = _unicode;
				act = _mb2wc;
				cp = CP_GBK;
				break;
			case _gbk:
			case _big5:
				scp = _gbk;
				act = _sc2tc;
				break;
			}
			break;
		case _gbk:
			switch (dcp)
			{
			case _unicode:
			case _utf8:
			case _big5:
				scp = _unicode;
				act = _mb2wc;
				cp = CP_GBK;
				break;
			case _gb2312:
				scp = _gb2312;
				act = _tc2sc;
				break;
			}
			break;
		case _big5:
			switch (dcp)
			{
			case _unicode:
			case _utf8:
			case _gb2312:
			case _gbk:
				scp = _unicode;
				act = _mb2wc;
				cp = CP_BIG5;
				break;
			}
			break;
		}

		switch (act)
		{
		case _wc2mb:
			cch = WideCharToMultiByte(cp, 0, (wchar_t *)src, -1, NULL, 0, NULL, NULL);
			dest = malloc(cch * sizeof(char));
			WideCharToMultiByte(cp, 0, (wchar_t *)src, -1, (char *)dest, cch, NULL, NULL);
			break;
		case _mb2wc:
			cch = MultiByteToWideChar(cp, 0, (char *)src, -1, NULL, 0);
			dest = malloc(cch * sizeof(wchar_t));
			MultiByteToWideChar(cp, 0, (char *)src, -1, (wchar_t *)dest, cch);
			break;
		case _sc2tc:
			lcid = GetSystemDefaultLCID();
			cch = LCMapString(lcid, LCMAP_TRADITIONAL_CHINESE, (char *)src, -1, NULL, 0);
			dest = malloc(cch * sizeof(char));
			LCMapString(lcid, LCMAP_TRADITIONAL_CHINESE, (char *)src, -1, (char *)dest, cch);
			break;
		case _tc2sc:
			lcid = GetSystemDefaultLCID();
			cch = LCMapString(lcid, LCMAP_SIMPLIFIED_CHINESE, (char *)src, -1, NULL, 0);
			dest = malloc(cch * sizeof(char));
			LCMapString(lcid, LCMAP_SIMPLIFIED_CHINESE, (char *)src, -1, (char *)dest, cch);
			break;
		}

		if (src && (src != sstr))
		{
			free(src);
		}
	}

	if (dstr)
	{
		*dstr = dest;
	}
	else
	{
		free(dest);
	}

	return cch;
}

void copyStr(char ** dest, char * src)
{
	if (!dest) return;
	int len = strlen(src);
	char * destTemp = (char *)malloc(len + 1);
	memmove(destTemp, src, len + 1);
	*dest = destTemp;
}

void saveFree(void ** str)
{
	if (*str)
	{
		free(*str);
		*str = NULL;
	}
}
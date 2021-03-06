// NativeAsr.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>    
#include <Windows.h> 
#include "RecordThread.h"
#include "NativeEngine.h"
#include "NativeEngineHolder.h"
#include "Utils.h"
#pragma comment(lib, "winmm.lib")  


int _tmain(int argc, _TCHAR* argv[])
{
	//int totalSize = 0;
	//RecordThread recordThread = RecordThread();
	//boolean result = recordThread.start();
	//if (result)
	//{
	//	char * time = getTime();
	//	char  pcm[5] = ".pcm";
	//	int len = strlen(time);
	//	char * fileName = (char *)malloc(sizeof(char) * len + sizeof(pcm));
	//	memmove(fileName, time, len * sizeof(char));
	//	memmove(fileName + len, pcm, sizeof(pcm));
	//	free(time);
	//	FILE * fp = fopen(fileName, "wb");
	//	if (fp != NULL)
	//	{
	//		while (true)
	//		{
	//			char * byte = (char*)malloc(sizeof(char) * 1200);
	//			int size = recordThread.read(byte, 1200);
	//			if (size > 0)
	//			{
	//				fwrite(byte, sizeof(char) * size, 1, fp);
	//			}
	//			free(byte);
	//			totalSize += size;
	//			if (totalSize > 1024 * 300 || size < 0)
	//			{
	//				recordThread.cancel();
	//				fclose(fp);
	//				break;
	//			}
	//		}
	//	}
	//}
	//NativeEngine native = NativeEngine();
	//boolean ret = native.initEngine();
	//if (ret)
	//{
	//	native.startAsr();
	//}
	//return 0;
	int ret = initEngine();
	if (ret == 0)
	{
		int ret = startAsr();
		if (ret == 0)
		{
			getchar();
			cancelAsr();
			getchar();
		}
	}
}

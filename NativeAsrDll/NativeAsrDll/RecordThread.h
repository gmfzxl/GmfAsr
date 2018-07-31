#pragma once
#include "stdio.h"    
#include "stdafx.h"
#include <thread>
#include <mutex> 
#include "Windows.h" 
#include "mmeapi.h"
#pragma comment(lib, "winmm.lib")  
#define MAX_RETRY_COUNT 50
using namespace std;
typedef void(*recordClosedCallBack)(void *);
class RecordThread
{

private:
	std::mutex mut;
	int retryCount;
	recordClosedCallBack callBack;
	void * pCall;
	int nSamplesPerSec;
	volatile int isRun;
	volatile int isCancel;
	HWAVEIN hWaveIn;  //�����豸  
	WAVEFORMATEX waveform; //�ɼ���Ƶ�ĸ�ʽ���ṹ��   
	WAVEHDR wHdr[8]; //�ɼ���Ƶʱ�������ݻ���Ľṹ��
	char * recordBuffer;
	char * tempBuffer[8];
	int recordBufferSize;
	int tempBufferSize;
	int size;
	volatile int readSize;
	void recovery();
public:
	RecordThread();
	int start();
	void setCallBack(recordClosedCallBack cb, void * pCall);
	void cancel();
	int read(char * pData, int size);
	void micCallBack2(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
	static void CALLBACK  micCallBack(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
	~RecordThread();
};


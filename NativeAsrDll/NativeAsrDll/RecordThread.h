#pragma once
#include "stdio.h"    
#include "stdafx.h"
#include <thread>
#include <mutex> 
#include "Windows.h" 
#include "mmeapi.h"
#pragma comment(lib, "winmm.lib")  
#define WAV_HEADER_LEN 44
#define MAX_RETRY_COUNT 50
#define BITS_PER_SAMPLE 16
#define CHANNLEL 1
using namespace std;
typedef void(*recordClosedCallBack)(void *);

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
	HWAVEIN hWaveIn;  //输入设备  
	WAVEFORMATEX waveform; //采集音频的格式，结构体   
	WAVEHDR wHdr[8]; //采集音频时包含数据缓存的结构体
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
	void setSamplesPerSec(int samplesPerSec);
	int getSamplesPerSec();
	int read(char * pData, int size);
	void micCallBack2(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
	static void CALLBACK  micCallBack(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
	void addWaveHeader(char * filePath, int samplesPerSec);
	~RecordThread();
};


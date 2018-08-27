#include "stdafx.h"
#include "RecordThread.h"
#include "Utils.h"

RecordThread::RecordThread()
{
	isRun = FALSE;
	nSamplesPerSec = 16000;
	size = 0;
	recordBufferSize = 1024 * 1000;
	recordBuffer = (char *)malloc(recordBufferSize);
	int size = sizeof(tempBuffer) / sizeof(char *);
	tempBufferSize = nSamplesPerSec * BITS_PER_SAMPLE * CHANNLEL / 8 / 40;
	for (int i = 0; i < size; i++)
	{
		tempBuffer[i] = (char *)malloc(tempBufferSize);
	}
}

void RecordThread::setCallBack(recordClosedCallBack cb, void *pCall)
{
	this->callBack = cb;
	this->pCall = pCall;
}

void RecordThread::setSamplesPerSec(int nSamplesPerSec)
{
	this->nSamplesPerSec = nSamplesPerSec;
}

int RecordThread::getSamplesPerSec()
{
	return nSamplesPerSec;
}

int RecordThread::start()
{
	recovery();
	cancel();
	isRun = TRUE;
	waveform.wFormatTag = WAVE_FORMAT_PCM;//声音格式为PCM  
	waveform.nSamplesPerSec = nSamplesPerSec;//采样率，16000次/秒  
	waveform.wBitsPerSample = 16;//采样比特，16bits/次  
	waveform.nChannels = 1;//采样声道数，1声道  
	waveform.nAvgBytesPerSec = nSamplesPerSec;//每秒的数据率，就是每秒能采集多少字节的数据  
	waveform.nBlockAlign = 2;
	waveform.cbSize = 0;//一般为0  
	MMRESULT mmr = waveInOpen(&hWaveIn, WAVE_MAPPER, &waveform, (DWORD)(micCallBack), (DWORD)this, CALLBACK_FUNCTION);
	if (mmr == MMSYSERR_NOERROR)
	{
		int size = sizeof(wHdr) / sizeof(WAVEHDR);
		for (int i = 0; i < size; i++)
		{
			wHdr[i].lpData = (LPSTR)tempBuffer[i];
			wHdr[i].dwBufferLength = tempBufferSize;
			wHdr[i].dwBytesRecorded = 0;
			wHdr[i].dwUser = 0;
			wHdr[i].dwFlags = 0;
			wHdr[i].dwLoops = 1;
			waveInPrepareHeader(hWaveIn, &wHdr[i], sizeof(WAVEHDR));//准备一个波形数据块头用于录音  
			waveInAddBuffer(hWaveIn, &wHdr[i], sizeof(WAVEHDR));//指定波形数据块为录音输入缓存 
		}
		waveInStart(hWaveIn);
		return TRUE;
	}
	isRun = FALSE;
	return FALSE;
}
void RecordThread::micCallBack2(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	DWORD threadId = 0;
	LPWAVEHDR pHdr = (LPWAVEHDR)dwParam1;
	RecordThread *pWnd = (RecordThread*)dwInstance;
	switch (uMsg)
	{
	case WIM_OPEN:
		threadId = GetCurrentThreadId();
		OutputDebugStringEx("WIM_OPEN %d\n", threadId);
		break;
	case WIM_DATA:
		if (isRun)
		{
			OutputDebugStringEx("WIM_DATA\n", threadId);
			{
				std::lock_guard<std::mutex> lock(mut);
				OutputDebugStringEx("WIM_DATA %d\n", pHdr->dwBytesRecorded);
				int tempSize = pHdr->dwBytesRecorded;
				OutputDebugStringEx("WIM_DATA2\n");
				memmove(recordBuffer + size, pHdr->lpData, pHdr->dwBytesRecorded);
				size += tempSize;
			}
			OutputDebugStringEx("WIM_DATA3\n", threadId);
			waveInUnprepareHeader(hwavein, pHdr, sizeof(WAVEHDR));
			OutputDebugStringEx("WIM_DATA4\n", threadId);
			waveInPrepareHeader(hwavein, pHdr, sizeof(WAVEHDR));
			waveInAddBuffer(hwavein, pHdr, sizeof(WAVEHDR));
			OutputDebugStringEx("WIM_DATA5\n", threadId);
		}
		break;
	case WIM_CLOSE:
		threadId = GetCurrentThreadId();
		OutputDebugStringEx("WIM_CLOSE %d\n", threadId);
		if (this->callBack)
		{
			this->callBack(this->pCall);
		}
		break;
	default:
		break;
	}
	return;
}
void RecordThread::micCallBack(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	RecordThread *pWnd = (RecordThread*)dwInstance;
	if (pWnd)
	{
		pWnd->micCallBack2(hwavein, uMsg, dwInstance, dwParam1, dwParam2);
	}
}

int RecordThread::read(char * pData, int readSize)
{
	retryCount = 0;
	while (true)
	{
		if (size >= readSize)
		{
			{
				std::lock_guard<std::mutex> lock(mut);
				memmove(pData, recordBuffer, readSize);
				if (size - readSize > 0)
				{
					memmove(recordBuffer, recordBuffer + readSize, size - readSize);
				}
				size -= readSize;
			}
			break;
		}
		else if (!isRun)
		{
			{
				std::lock_guard<std::mutex> lock(mut);
				readSize = size;
				memmove(pData, recordBuffer, size);
				size = 0;
			}
			break;
		}
		else
		{
			if (retryCount < MAX_RETRY_COUNT)
			{
				retryCount++;
				OutputDebugStringEx("retryCount %d\n", retryCount);
				Sleep(10);
			}
			else
			{
				return -1;
			}
		}
	}
	return readSize;
}

void RecordThread::cancel()
{
	OutputDebugStringEx("record cancel %d\n", isRun);
	if (isRun)
	{
		isRun = FALSE;
		OutputDebugStringEx("waveInStop\n");
		waveInStop(hWaveIn);
		OutputDebugStringEx("waveInReset\n");
		waveInReset(hWaveIn);
		OutputDebugStringEx("waveInClose start\n");
		waveInClose(hWaveIn);
		OutputDebugStringEx("waveInClose end\n");
	}
}

void RecordThread::recovery()
{
	OutputDebugStringEx("recovery %d\n", isRun);
	size = 0;
	isRun = FALSE;

}

void RecordThread::addWaveHeader(char * filePath, int samplesPerSec)
{
	FILE * fp = fopen(filePath, "rb+");
	long size = 0;
	if (fp)
	{
		int writeSize = 0;
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		if (size > 0)
		{
			WaveHeader waveHeader;
			if (sizeof(waveHeader) != WAV_HEADER_LEN)
			{
				return;
			}
			memmove(waveHeader.riff, "RIFF", 4);
			memmove(waveHeader.wave, "WAVE", 4);
			memmove(waveHeader.fmt, "fmt ", 4);
			memmove(waveHeader.data, "data", 4);
			waveHeader.totalLen = size - 8;
			waveHeader.dwFMTLen = 16;
			waveHeader.fmtPcm = 1;
			waveHeader.channels = CHANNLEL;
			waveHeader.fmtSamplehz = samplesPerSec;
			waveHeader.fmtBytepsec = samplesPerSec * CHANNLEL*BITS_PER_SAMPLE / 8;
			waveHeader.fmtBlockAlign = CHANNLEL * BITS_PER_SAMPLE / 8;
			waveHeader.fmtBitPerSample = BITS_PER_SAMPLE;
			waveHeader.dwDATALen = size - 44;
			fseek(fp, 0, SEEK_SET);
			writeSize = fwrite(&waveHeader, WAV_HEADER_LEN, 1, fp);
		}
		fclose(fp);
		if (writeSize <= 0)
		{
			remove(filePath);
		}
	}
}

RecordThread::~RecordThread()
{
	free(recordBuffer);
	int size = sizeof(tempBuffer) / sizeof(char *);
	for (int i = 0; i < size; i++)
	{
		free(tempBuffer[i]);
	}
}


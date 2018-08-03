#include "stdafx.h"
#include "RecordThread.h"
#include "Utils.h"

RecordThread::RecordThread()
{
	isRun = FALSE;
	isCancel = TRUE;
	nSamplesPerSec = 16000;
	size = 0;
	recordBufferSize = 1024 * 1000;
	readSize = recordBufferSize / 2;
	recordBuffer = (char *)malloc(recordBufferSize);
	int size = sizeof(tempBuffer) / sizeof(char *);
	tempBufferSize = nSamplesPerSec * BITS_PER_SAMPLE * CHANNLEL / 8 / 20;
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
	cancel();
	isRun = TRUE;
	isCancel = FALSE;
	waveform.wFormatTag = WAVE_FORMAT_PCM;//������ʽΪPCM  
	waveform.nSamplesPerSec = nSamplesPerSec;//�����ʣ�16000��/��  
	waveform.wBitsPerSample = 16;//�������أ�16bits/��  
	waveform.nChannels = 1;//������������1����  
	waveform.nAvgBytesPerSec = nSamplesPerSec;//ÿ��������ʣ�����ÿ���ܲɼ������ֽڵ�����  
	waveform.nBlockAlign = 2;
	waveform.cbSize = 0;//һ��Ϊ0  
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
			waveInPrepareHeader(hWaveIn, &wHdr[i], sizeof(WAVEHDR));//׼��һ���������ݿ�ͷ����¼��  
			waveInAddBuffer(hWaveIn, &wHdr[i], sizeof(WAVEHDR));//ָ���������ݿ�Ϊ¼�����뻺�� 
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
	RecordThread *pWnd = (RecordThread*)dwInstance;
	switch (uMsg)
	{
	case WIM_OPEN:
		threadId = GetCurrentThreadId();
		OutputDebugStringEx("WIM_OPEN %d\n", threadId);
		break;
	case WIM_DATA:
		if (!isCancel)
		{
			OutputDebugStringEx("WIM_DATA\n", threadId);
			LPWAVEHDR pHdr = (LPWAVEHDR)dwParam1;
			{
				std::lock_guard<std::mutex> lock(mut);
				OutputDebugStringEx("WIM_DATA %d %d\n", isCancel, pHdr->dwBytesRecorded);
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
		recovery();
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
		this->readSize = readSize;
		if (size >= readSize)
		{
			std::lock_guard<std::mutex> lock(mut);
			memmove(pData, recordBuffer, readSize);
			if (size - readSize > 0)
			{
				memmove(recordBuffer, recordBuffer + readSize, size - readSize);
			}
			size -= readSize;
			break;
		}
		else
		{
			if (isRun && retryCount < MAX_RETRY_COUNT)
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
	OutputDebugStringEx("cancel %d\n", isRun);
	if (isRun)
	{
		isCancel = TRUE;
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
	isCancel = TRUE;
	recordBufferSize = 1024 * 1000;
	readSize = recordBufferSize / 2;
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


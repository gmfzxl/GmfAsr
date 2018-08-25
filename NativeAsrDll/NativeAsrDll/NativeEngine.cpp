#include "stdafx.h"
#include <windows.h>
#include "NativeEngine.h"
#include "Utils.h"
#include <direct.h>
#include <io.h>

NativeEngine::NativeEngine()
{
	nSamplesPerSec = 16000;
	pEngine = (Engine *)malloc(sizeof(Engine));
	initSuccess = FALSE;
	frameSize = 1200;
	dllCallBack = NULL;
	setEngineState(ENGINE_STATE_NOT_INIT);
	recordThread.setCallBack(recordCallBack, this);
}

void NativeEngine::callBack(int event, const char * result, int data)
{
	if (dllCallBack)
	{
		dllCallBack(event, result, data);
	}
}

int NativeEngine::getEngineState()
{
	return getEngineState();
}

void NativeEngine::setEngineState(int state)
{
	OutputDebugStringEx("setEngineState %d\n", state);
	if (this->state != state)
	{
		this->state = state;
		callBack(ENGINE_STATE_CHANGE, NULL, state);
	}
}

void NativeEngine::setCallBack(eventCallBack callBack)
{
	OutputDebugStringEx("setCallBack %d\n", callBack);
	this->dllCallBack = callBack;
}

int NativeEngine::resetGrammer(char * grammerPath, char * tagName)
{
	if (!initSuccess) {
		OutputDebugStringEx("not init\n");
		callBack(EVENT_ENGINE_ERROR, "engine not init", ERROR_ENGINE_NOT_INIT);
		return ERROR_ENGINE_NOT_INIT;
	}
	cancelAsr();
	int ret = pEngine->pReset(grammerPath, "command");
	if (ret == 0)
	{
		copyStr(&this->tagName, tagName);
		return 0;
	}
	return -1;
}

int NativeEngine::initEngine(char * grammerPath, char * tagName, eventCallBack callBack)
{
	this->dllCallBack = callBack;
	if (initSuccess) {
		OutputDebugStringEx("double init\n");
		callBack(EVENT_ENGINE_ERROR, "double init", ERROR_DOUBLE_INIT);
		return ERROR_DOUBLE_INIT;
	}
	if (!pEngine) {
		OutputDebugStringEx("malloc fail\n");
		callBack(EVENT_ENGINE_ERROR, "malloc fail", ERROR_MALLOC_FAIL);
		return ERROR_MALLOC_FAIL;
	}
	pEngine->hModule = LoadLibraryEx("..\\dll\\libengine.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (!pEngine->hModule) {
		OutputDebugStringEx("load dll fail\n");
		callBack(EVENT_ENGINE_ERROR, "load dll fail", ERROR_LOAD_DLL_FAIL);
		return ERROR_LOAD_DLL_FAIL;
	}
	pEngine->pInit = (init)GetProcAddress(pEngine->hModule, "init");
	pEngine->pSetOption = (setOption)GetProcAddress(pEngine->hModule, "setOptionInt");
	pEngine->pStart = (start)GetProcAddress(pEngine->hModule, "start");
	pEngine->pRecognize = (recognize)GetProcAddress(pEngine->hModule, "recognize");
	pEngine->pLoadCompiledJsgf = (loadCompiledJsgf)GetProcAddress(pEngine->hModule, "loadCompiledJsgf");
	pEngine->pReset = (reset)GetProcAddress(pEngine->hModule, "reset");
	pEngine->pStop = (stop)GetProcAddress(pEngine->hModule, "stop");
	pEngine->pGetResult = (getResult)GetProcAddress(pEngine->hModule, "getResult");
	pEngine->pInitUserDataCompiler = (initUserDataCompiler)GetProcAddress(pEngine->hModule, "initUserDataCompiler");
	pEngine->pVersion = (version)GetProcAddress(pEngine->hModule, "version");
	pEngine->pRelease = (release)GetProcAddress(pEngine->hModule, "release");
	pEngine->pPartialCompileUserData = (partialCompileUserData)GetProcAddress(pEngine->hModule, "partialCompileUserData");
	if (!pEngine->pInit || !pEngine->pSetOption || !pEngine->pStart ||
		!pEngine->pRecognize || !pEngine->pStop || !pEngine->pGetResult ||
		!pEngine->pRelease || !pEngine->pReset || !pEngine->pLoadCompiledJsgf ||
		!pEngine->pInitUserDataCompiler || !pEngine->pPartialCompileUserData ||
		!pEngine->pVersion) {
		OutputDebugStringEx("get api fail\n");
		callBack(EVENT_ENGINE_ERROR, "get api fail", ERROR_GET_API_FAIL);
		return ERROR_GET_API_FAIL;
	}
	int ret = pEngine->pInit("..\\module", grammerPath, 79);
	if (ret != 0)
	{
		OutputDebugStringEx("init fail\n");
		callBack(EVENT_ENGINE_ERROR, "init fail", ERROR_INIT_FAIL);
		return ERROR_INIT_FAIL;
	}
	copyStr(&this->tagName, tagName);
	initSuccess = TRUE;
	DWORD threadId = GetCurrentThreadId();
	OutputDebugStringEx("init success id %d\n", threadId);
	callBack(EVENT_ENGINE_SUCCESS, "init success", NULL);
	setEngineState(ENGINE_STATE_IDLE);
	return SUCCESS;
}

const char * NativeEngine::getVersion()
{
	if (pEngine)
	{
		char * version = pEngine->pVersion();
		OutputDebugStringEx(version);
		return version;
	}
	return NULL;
}

void NativeEngine::saveRecord(int save)
{
	OutputDebugStringEx("saveRecord %d\n", save);
	this->save = save;
}

int NativeEngine::startAsr()
{
	OutputDebugStringEx("startAsr %d\n", initSuccess);
	if (!initSuccess)
	{
		OutputDebugStringEx("engine not init\n");
		callBack(EVENT_ENGINE_ERROR, "engine not init", ERROR_ENGINE_NOT_INIT);
		return ERROR_ENGINE_NOT_INIT;
	}
	if (state == ENGINE_STATE_ASR)
	{
		OutputDebugStringEx("engine already start\n");
		callBack(EVENT_ENGINE_ERROR, "engine already start", ERROR_ALREADY_START);
		return ERROR_ALREADY_START;
	}
	recordThread.setSamplesPerSec(nSamplesPerSec);
	pEngine->pSetOption(OPTION_SET_SAMPLES, nSamplesPerSec / 1000);
	int result = recordThread.start();
	OutputDebugStringEx("recordThread.start\n");
	if (!result)
	{
		OutputDebugStringEx("open record fail\n");
		callBack(EVENT_ENGINE_ERROR, "open record fail", ERROR_OPEN_RECORD_FAIL);
		return ERROR_OPEN_RECORD_FAIL;
	}
	isRun = TRUE;
	if (save)
	{
		int code = _mkdir("Record");
		OutputDebugStringEx("mkdir %d\n", code);
		char * time = getTime();
		const char  * flolder = "Record\\";
		char  pcm[5] = ".wav";
		int len = strlen(time) + strlen(flolder);
		fileName = (char *)malloc(sizeof(char) * len + sizeof(pcm));
		memmove(fileName, flolder, strlen(flolder) * sizeof(char));
		memmove(fileName + strlen(flolder), time, strlen(time) * sizeof(char));
		memmove(fileName + len, pcm, sizeof(pcm));
		free(time);
		OutputDebugStringEx(fileName);
		closeFile();
		fp = fopen(fileName, "wb");
		if (fp)
		{
			char wavHeader[WAV_HEADER_LEN];
			fwrite(wavHeader, WAV_HEADER_LEN, 1, fp);
		}
	}
	OutputDebugStringEx("enterAsr thread\n");
	pThread = (pthread_t *)malloc(sizeof(pthread_t));
	pthread_create(pThread, NULL, enterAsr2, (void *)this);
	DWORD threadId = GetCurrentThreadId();
	OutputDebugStringEx("start asr success %d\n", threadId);
	callBack(EVENT_ENGINE_SUCCESS, "start asr success", 0);
	setEngineState(ENGINE_STATE_ASR);
	return 0;
}

int NativeEngine::startTest(char * wavPath)
{
	OutputDebugStringEx("startTest %d\n", initSuccess);
	if (!initSuccess)
	{
		OutputDebugStringEx("engine not init\n");
		callBack(EVENT_ENGINE_ERROR, "engine not init", ERROR_ENGINE_NOT_INIT);
		return ERROR_ENGINE_NOT_INIT;
	}
	if (!wavPath)
	{
		OutputDebugStringEx("wav path null\n");
		callBack(EVENT_ENGINE_ERROR, "engine not init", ERROR_WAVE_PATH_NULL);
		return ERROR_WAVE_PATH_NULL;
	}
	saveFree((void **)&testPath);
	int len = strlen(wavPath);
	testPath = (char *)malloc(strlen(wavPath) + 1);
	memmove(testPath, wavPath, len + 1);
	if (state == ENGINE_STATE_ASR)
	{
		OutputDebugStringEx("engine already start\n");
		callBack(EVENT_ENGINE_ERROR, "engine already start", ERROR_ALREADY_START);
		return ERROR_ALREADY_START;
	}
	pEngine->pSetOption(OPTION_SET_SAMPLES, nSamplesPerSec / 1000);
	isRun = TRUE;
	OutputDebugStringEx("enterTest thread\n");
	pThread = (pthread_t *)malloc(sizeof(pthread_t));
	pthread_create(pThread, NULL, enterTest2, (void *)this);
	DWORD threadId = GetCurrentThreadId();
	OutputDebugStringEx("start asr success %d\n", threadId);
	callBack(EVENT_ENGINE_SUCCESS, "start asr success", 0);
	setEngineState(ENGINE_STATE_ASR);
	return 0;
}

void NativeEngine::cancelAsr()
{
	OutputDebugStringEx("engine cancel is Run %d\n", isRun);
	if (isRun)
	{
		isRun = FALSE;
		recordThread.cancel();
		if (pThread)
		{
			pthread_join(*pThread, NULL);
			free(pThread);
			pThread = NULL;
		}
	}
	callBack(EVENT_ENGINE_SUCCESS, "cancel asr success", 0);
}

int NativeEngine::insertVocab(const char * vocab)
{
	OutputDebugStringEx("insertVocab %d\n", initSuccess);
	OutputDebugStringEx(vocab);
	if (!initSuccess)
	{
		OutputDebugStringEx("engine not init\n");
		callBack(EVENT_ENGINE_ERROR, "engine not init", ERROR_ENGINE_NOT_INIT);
		return ERROR_ENGINE_NOT_INIT;
	}
	cancelAsr();
	char *pDest;
	Gb2312ToUtf8(vocab, &pDest);
	string jsgf = getJsgfString("asr_main");
	int ret = pEngine->pPartialCompileUserData(handle, "..\\grammar\\asr_main_partial.dat", jsgf.data(), pDest,
		"..\\grammar\\asr_main2.dat", "..\\grammar\\asr_main_partial.dat");
	if (ret != 0)
	{
		OutputDebugStringEx("compile fail %d\n", ret);
		callBack(EVENT_ENGINE_ERROR, "ompile fail", ERROR_COMPILE_JSGF_FAIL);
		return ERROR_COMPILE_JSGF_FAIL;
	}
	OutputDebugStringEx("compile success\n");
	ret = pEngine->pReset("..\\grammar\\asr_main2.dat", "command");
	if (ret != 0)
	{
		OutputDebugStringEx("reset fail %d\n", ret);
		callBack(EVENT_ENGINE_ERROR, "reset fail", ERROR_RESET_JSGF_FAIL);
		return ERROR_RESET_JSGF_FAIL;
	}
	OutputDebugStringEx("reset success\n");
	callBack(EVENT_ENGINE_SUCCESS, "insertVocab success", 0);
	return SUCCESS;
}

void NativeEngine::enterTest()
{
	int len = strlen(testPath);
	const char * wav = "*.wav";
	char * path = (char *)malloc(len + strlen(wav) + 1);
	memmove(path, testPath, len);
	memmove(path + len, wav, strlen(wav) + 1);
	long handler;
	_finddata_t fileDir;
	char * tempData = (char *)malloc(frameSize);
	if ((handler = _findfirst(path, &fileDir)) == -1L)
	{
		OutputDebugStringEx("folder is empty\n");
	}
	else
	{
		do {
			OutputDebugStringEx(fileDir.name);
			char * filePath = (char *)malloc(len + strlen(fileDir.name) + 1);
			memmove(filePath, testPath, len);
			memmove(filePath + len, fileDir.name, strlen(fileDir.name) + 1);
			OutputDebugStringEx(filePath);
			FILE * file = fopen(filePath, "rb");
			if (file)
			{
				fseek(file, WAV_HEADER_LEN, SEEK_SET);
				OutputDebugStringEx("pEngine->pStart\n");
				int ret = pEngine->pStart(this->tagName, 53);
				if (ret != 0) {
					OutputDebugStringEx("start fail!\n");
					callBack(EVENT_ENGINE_ERROR, "start fail", ERROR_START_ASR_FAIL);
					return;
				}
				while (isRun)
				{
					int read = fread((void *)tempData, sizeof(char), frameSize, file);
					if (read > 0)
					{
						int code = pEngine->pRecognize((char *)tempData, read);
						if (code == 2) {
							char * cache = (char *)pEngine->pGetResult();
							char *pDest;
							Utf8ToGb2312(cache, &pDest);
							DWORD threadId = GetCurrentThreadId();
							OutputDebugStringEx("get result %d\n", threadId);
							OutputDebugStringEx(pDest);
							const char * split = "@@@";
							char * result = (char *)malloc(strlen(fileDir.name) + strlen(split) + strlen(pDest) + 1);
							memmove(result, fileDir.name, strlen(fileDir.name));
							memmove(result + strlen(fileDir.name), split, strlen(split));
							memmove(result + strlen(fileDir.name) + strlen(split), pDest, strlen(pDest) + 1);
							callBack(EVENT_TEST_RESULT, result, 0);
							saveFree((void **)&pDest);
							saveFree((void **)&result);
						}
					}
					else
					{
						break;
					}
				}
				fclose(file);
				pEngine->pStop();
			}
			saveFree((void **)&filePath);
		} while (_findnext(handler, &fileDir) == 0 && isRun);
		_findclose(handler);
	}
	saveFree((void **)&testPath);
	saveFree((void **)&path);
	saveFree((void **)&tempData);
	if (isRun)
	{
		callBack(EVENT_TEST_END, "test end", 0);
	}
	setEngineState(ENGINE_STATE_IDLE);
	OutputDebugStringEx("engine stop\n");
	callBack(EVENT_ENGINE_SUCCESS, "engine stop", 0);
}

void NativeEngine::enterAsr()
{
	int read = 0;
	OutputDebugStringEx("pEngine->pStart\n");
	int ret = pEngine->pStart(this->tagName, 53);
	if (ret != 0) {
		OutputDebugStringEx("start fail!\n");
		callBack(EVENT_ENGINE_ERROR, "start fail", ERROR_START_ASR_FAIL);
		return;
	}
	while (isRun)
	{
		char * pData = (char *)malloc(frameSize * sizeof(char));
		read = recordThread.read(pData, frameSize);
		if (addNoise)
		{
			soundMix.process(pData, read, this->ratio);
		}
		if (read > 0)
		{
			if (fp)
			{
				fwrite(pData, sizeof(char), read, fp);
			}
			int code = pEngine->pRecognize((char *)pData, frameSize);
			if (code == 2) {
				char * cache = (char *)pEngine->pGetResult();
				char *pDest;
				Utf8ToGb2312(cache, &pDest);
				DWORD threadId = GetCurrentThreadId();
				OutputDebugStringEx("get result %d\n", threadId);
				OutputDebugStringEx(pDest);
				callBack(EVENT_ASR_RESULT, pDest, 0);
				free(pDest);
			}
		}
		free(pData);
		if (read < 0)
		{
			break;
		}
	}
	closeFile();
	pEngine->pStop();
	if (isRun && read < 0)
	{
		callBack(EVENT_ENGINE_ERROR, NULL, ERROR_READ_RECORD_ERROR);
	}
	setEngineState(ENGINE_STATE_IDLE);
	OutputDebugStringEx("engine stop\n");
	callBack(EVENT_ENGINE_SUCCESS, "engine stop", 0);
}

void NativeEngine::recordCallBack(void * p2)
{
	OutputDebugStringEx("recordCallBack\n");
	NativeEngine *p = (NativeEngine *)p2;
	p->cancelAsr();
}

void NativeEngine::closeFile()
{
	if (fp)
	{
		fclose(fp);
		fp = NULL;
		recordThread.addWaveHeader(fileName, recordThread.getSamplesPerSec());
		saveFree((void **)&fileName);
	}
}
void* NativeEngine::enterAsr2(void * p2)
{
	NativeEngine *p = (NativeEngine *)p2;
	p->enterAsr();
	return NULL;
}

void* NativeEngine::enterTest2(void * p2)
{
	NativeEngine *p = (NativeEngine *)p2;
	p->enterTest();
	return NULL;
}

void NativeEngine::releaseEngine()
{
	if (initSuccess)
	{
		cancelAsr();
		pEngine->pRelease();
		initSuccess = FALSE;
		setEngineState(ENGINE_STATE_NOT_INIT);
		callBack(EVENT_ENGINE_SUCCESS, "engine release", 0);
	}
	return;
}

int NativeEngine::setSamplesPerSec(int samplesPerSec)
{
	if (samplesPerSec != 16000 && samplesPerSec != 8000)
	{
		return -1;
	}
	this->nSamplesPerSec = samplesPerSec;
	return 0;
}

int NativeEngine::setNoiseData(int addNoise, char * file, float ratio)
{
	this->addNoise = addNoise;
	this->ratio = ratio;
	if (this->addNoise)
	{
		copyStr(&this->noiseDataFile, file);
		FILE * fp = fopen(this->noiseDataFile, "rb");
		if (!fp)
		{
			this->addNoise = 0;
			return ERROR_NOISE_FILE_NULL;
		}
		fclose(fp);
		if (this->ratio == 0)
		{
			this->addNoise = 0;
			return ERROR_NOISE_RATIO;
		}
		return soundMix.init(file);
	}
	else
	{
		return SUCCESS;
	}
}

string NativeEngine::getJsgfString(const char * modelTag) {
	string tag = modelTag;
	string jsgf = "#JSGF V1.0 utf-8 cn;\n grammar " + tag + ";\n"
		+ "public <" + modelTag + "> =( \"<s>\" (\n"
		+ "<NAME>\n" + ") \"</s>\");";
	OutputDebugStringEx(jsgf.data());
	return jsgf;
}

NativeEngine::~NativeEngine()
{
	soundMix.release();
	saveFree((void **)&testPath);
	saveFree((void **)&noiseDataFile);
	recordThread.setCallBack(NULL, NULL);
	free(pEngine);
	cancelAsr();
	saveFree((void **)(&this->tagName));
	closeFile();
}

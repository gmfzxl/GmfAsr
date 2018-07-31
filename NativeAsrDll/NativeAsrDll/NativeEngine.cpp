#include "stdafx.h"
#include <windows.h>
#include "NativeEngine.h"
#include "Utils.h"
#include <direct.h>

NativeEngine::NativeEngine()
{
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

int NativeEngine::initEngine(eventCallBack callBack)
{
	this->dllCallBack = callBack;
	if (initSuccess) {
		OutputDebugStringEx("double init\n");
		callBack(EVENT_ENGINE_ERROR, "double init", NULL);
		return ERROR_DOUBLE_INIT;
	}
	if (!pEngine) {
		OutputDebugStringEx("malloc fail\n");
		callBack(EVENT_ENGINE_ERROR, "malloc fail", NULL);
		return ERROR_MALLOC_FAIL;
	}
	pEngine->hModule = LoadLibraryEx("..\\dll\\libengine.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (!pEngine->hModule) {
		OutputDebugStringEx("load dll fail\n");
		callBack(EVENT_ENGINE_ERROR, "load dll fail", NULL);
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
	pEngine->pRelease = (release)GetProcAddress(pEngine->hModule, "release");
	pEngine->pVersion = (version)GetProcAddress(pEngine->hModule, "version");
	pEngine->pPartialCompileUserData = (partialCompileUserData)GetProcAddress(pEngine->hModule, "partialCompileUserData");
	if (!pEngine->pInit || !pEngine->pSetOption || !pEngine->pStart ||
		!pEngine->pRecognize || !pEngine->pStop || !pEngine->pGetResult ||
		!pEngine->pRelease || !pEngine->pReset || !pEngine->pLoadCompiledJsgf ||
		!pEngine->pInitUserDataCompiler || !pEngine->pPartialCompileUserData ||
		!pEngine->pVersion) {
		OutputDebugStringEx("get api fail\n");
		callBack(EVENT_ENGINE_ERROR, "get api fail", NULL);
		return ERROR_GET_API_FAIL;
	}
	int ret = pEngine->pInit("..\\module", "..\\grammar\\tv_main.dat", 79);
	if (ret != 0)
	{
		OutputDebugStringEx("init fail\n");
		callBack(EVENT_ENGINE_ERROR, "init fail", NULL);
		return ERROR_INIT_FAIL;
	}
	handle = pEngine->pInitUserDataCompiler("..\\module");
	if (handle <= 0)
	{
		OutputDebugStringEx("initUserDataCompiler fail %ld\n", handle);
		callBack(EVENT_ENGINE_ERROR, "initUserDataCompiler fail", NULL);
		return ERROR_INIT_COMPILE_FAIL;
	}
	OutputDebugStringEx("initUserDataCompiler %ld\n", handle);
	ret = pEngine->pLoadCompiledJsgf(handle, "..\\grammar\\asr_main.dat");
	OutputDebugStringEx("load jsgf result %d\n", ret);
	if (ret != 0)
	{
		callBack(EVENT_ENGINE_ERROR, "load jsgf result", NULL);
		return ERROR_LOAD_JSGF_FAIL;
	}
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
		callBack(EVENT_ENGINE_ERROR, "engine not init", 0);
		return ERROR_ENGINE_NOT_INIT;
	}
	if (state == ENGINE_STATE_ASR)
	{
		OutputDebugStringEx("engine already start\n");
		callBack(EVENT_ENGINE_ERROR, "engine already start", 0);
		return ERROR_ALREADY_START;
	}
	int result = recordThread.start();
	OutputDebugStringEx("recordThread.start\n");
	if (!result)
	{
		OutputDebugStringEx("open record fail\n");
		callBack(EVENT_ENGINE_ERROR, "open record fail", 0);
		return ERROR_OPEN_RECORD_FAIL;
	}
	OutputDebugStringEx("pEngine->pStart\n");
	int ret = pEngine->pStart("asr_main", 53);
	if (ret != 0) {
		OutputDebugStringEx("start fail!\n");
		callBack(EVENT_ENGINE_ERROR, "start fail", 0);
		return ERROR_START_ASR_FAIL;
	}
	isRun = TRUE;
	if (save)
	{
		int code = _mkdir("Record");
		OutputDebugStringEx("mkdir %d\n", code);
		char * time = getTime();
		const char  * flolder = "Record\\";
		char  pcm[5] = ".pcm";
		int len = strlen(time) + strlen(flolder);
		char * fileName = (char *)malloc(sizeof(char) * len + sizeof(pcm));
		memmove(fileName, flolder, strlen(flolder) * sizeof(char));
		memmove(fileName + strlen(flolder), time, strlen(time) * sizeof(char));
		memmove(fileName + len, pcm, sizeof(pcm));
		free(time);
		OutputDebugStringEx(fileName);
		fp = fopen(fileName, "wb");
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
		OutputDebugStringEx("engine stop\n");
		pEngine->pStop();
		setEngineState(ENGINE_STATE_IDLE);
		OutputDebugStringEx("engine cancel\n");
		callBack(EVENT_ENGINE_SUCCESS, "engine cancel", 0);
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
		callBack(EVENT_ENGINE_ERROR, "engine not init", 0);
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
		callBack(EVENT_ENGINE_ERROR, "ompile fail", 0);
		return ERROR_COMPILE_JSGF_FAIL;
	}
	OutputDebugStringEx("compile success\n");
	ret = pEngine->pReset("..\\grammar\\asr_main2.dat", "command");
	if (ret != 0)
	{
		OutputDebugStringEx("reset fail %d\n", ret);
		callBack(EVENT_ENGINE_ERROR, "reset fail", 0);
		return ERROR_RESET_JSGF_FAIL;
	}
	OutputDebugStringEx("reset success\n");
	callBack(EVENT_ENGINE_SUCCESS, "insertVocab success", 0);
	return SUCCESS;
}

void NativeEngine::enterAsr()
{
	int read = 0;
	while (isRun)
	{
		char * pData = (char *)malloc(frameSize * sizeof(char));
		read = recordThread.read(pData, frameSize);
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
	if (fp)
	{
		fclose(fp);
		fp = NULL;
	}
	if (isRun && read < 0)
	{
		callBack(EVENT_ENGINE_ERROR, NULL, ERROR_READ_RECORD_ERROR);
	}
	OutputDebugStringEx("enterAsr cancel\n");
}

void NativeEngine::recordCallBack(void * p2)
{
	OutputDebugStringEx("recordCallBack\n");
	NativeEngine *p = (NativeEngine *)p2;
	p->cancelAsr();
}

void* NativeEngine::enterAsr2(void * p2)
{
	NativeEngine *p = (NativeEngine *)p2;
	p->enterAsr();
	return NULL;
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
	recordThread.setCallBack(NULL, NULL);
	free(pEngine);
	cancelAsr();
	if (fp)
	{
		fclose(fp);
		fp = NULL;
	}
}

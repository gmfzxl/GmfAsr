#pragma once
#include <stdio.h>  
#include "stdafx.h"
#include "RecordThread.h" 
#include "pthread.h"
#pragma comment(lib, "pthreadVC2.lib")  

#define SUCCESS 0

#define ERROR_START -1000
#define ERROR_DOUBLE_INIT -999
#define ERROR_MALLOC_FAIL -998
#define ERROR_LOAD_DLL_FAIL -997
#define ERROR_GET_API_FAIL -996
#define ERROR_INIT_FAIL -995
#define ERROR_INIT_COMPILE_FAIL -994
#define ERROR_LOAD_JSGF_FAIL -993
#define ERROR_COMPILE_JSGF_FAIL -992
#define ERROR_RESET_JSGF_FAIL -991
#define ERROR_ENGINE_NOT_INIT -990
#define ERROR_OPEN_RECORD_FAIL -989
#define ERROR_START_ASR_FAIL -988
#define ERROR_ALREADY_START -987
#define ERROR_READ_RECORD_ERROR -986

#define EVENT_START 1000

#define EVENT_ENGINE_SUCCESS 2001
#define EVENT_ENGINE_ERROR 2001
#define EVENT_ASR_RESULT 2003

#define ENGINE_STATE_CHANGE 999
#define ENGINE_STATE_NOT_INIT 1000
#define ENGINE_STATE_IDLE 1001
#define ENGINE_STATE_ASR 1002

typedef  void(*eventCallBack)(int, const char *, int);

typedef int(__cdecl *init)(const char* path_of_am, const char* path_of_grammar, short port);
typedef int(__cdecl *start)(const char* grammar_tag, int am_index);
typedef int(__cdecl *recognize)(char* raw_audio, int len);
typedef int(__cdecl *reset)(const char* grammar_path, const char* type);
typedef int(__cdecl *loadCompiledJsgf)(long handler, const char* jsgf_path);
typedef int(__cdecl *stop)();
typedef char *(__cdecl *version)();
typedef char* (__cdecl *getResult)();
typedef int(__cdecl *release)();
typedef int(__cdecl *setOption)(int, int);
typedef long(__cdecl *initUserDataCompiler)(const char* modeldir);
typedef int(__cdecl *partialCompileUserData)(long handle, const char* in_partial_data_path, const char* jsgf, const char* vocab, const char* out_dir, const char* out_partial_data_path);
typedef struct {
	HMODULE hModule;
	init pInit;
	initUserDataCompiler pInitUserDataCompiler;
	start pStart;
	loadCompiledJsgf pLoadCompiledJsgf;
	recognize pRecognize;
	stop pStop;
	setOption pSetOption;
	partialCompileUserData pPartialCompileUserData;
	reset pReset;
	getResult pGetResult;
	version pVersion;
	release pRelease;
} Engine;

class NativeEngine
{
private:
	FILE * fp;
	int frameSize;
	long handle;
	int save;
	int state;
	eventCallBack dllCallBack;
	RecordThread recordThread;
	int initSuccess;
	Engine * pEngine;
	pthread_t * pThread;
	volatile int isRun;
	std::string getJsgfString(const char * modelTag);
public:
	NativeEngine();
	int initEngine(eventCallBack callBack);
	int getEngineState();
	void setEngineState(int state);
	int startAsr();
	void setCallBack(eventCallBack dllCallBack);
	void saveRecord(int save);
	const char * getVersion();
	void enterAsr();
	static void* enterAsr2(void * p);
	static void recordCallBack(void * p);
	void callBack(int event, const char * result, int data);
	int insertVocab(const char * vocab);
	void cancelAsr();
	~NativeEngine();
};


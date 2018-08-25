#pragma once
#include <stdio.h>  
#include "stdafx.h"
#include "RecordThread.h" 
#include "pthread.h"
#include "SoundMix.h"
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
#define ERROR_WAVE_PATH_NULL -985
#define ERROR_NOISE_FILE_NULL -984
#define ERROR_NOISE_RATIO -983

#define EVENT_START 1000

#define EVENT_ENGINE_SUCCESS 2001
#define EVENT_ENGINE_ERROR 2002
#define EVENT_ASR_RESULT 2003
#define EVENT_TEST_RESULT 2004
#define EVENT_TEST_END 2005

#define ENGINE_STATE_CHANGE 999
#define ENGINE_STATE_NOT_INIT 1000
#define ENGINE_STATE_IDLE 1001
#define ENGINE_STATE_ASR 1002

#define OPTION_SET_SAMPLES 600

typedef  void(*eventCallBack)(int, const char *, int);

typedef int(__cdecl *init)(const char* path_of_am, const char* path_of_grammar, short port);
typedef int(__cdecl *start)(const char* grammar_tag, int am_index);
typedef int(__cdecl *recognize)(char* raw_audio, int len);
typedef int(__cdecl *reset)(const char* grammar_path, const char* type);
typedef int(__cdecl *loadCompiledJsgf)(long handler, const char* jsgf_path);
typedef int(__cdecl *stop)();
typedef char *(__cdecl *version)();
typedef char* (__cdecl *getResult)();
typedef void(__cdecl *release)();
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
	char * tagName;
	FILE * fp;
	int frameSize;
	long handle;
	int nSamplesPerSec;
	int save;
	int state;
	char * fileName;
	char * testPath;
	eventCallBack dllCallBack;
	RecordThread recordThread;
	int initSuccess;
	Engine * pEngine;
	pthread_t * pThread;
	volatile int isRun;
	SoundMix soundMix;
	int addNoise;
	float ratio;
	char * noiseDataFile;
	std::string getJsgfString(const char * modelTag);
public:
	NativeEngine();
	int initEngine(char * grammerPath, char * tagName, eventCallBack callBack);
	int setSamplesPerSec(int samplesPerSec);
	int setNoiseData(int addNoise, char * file, float ratio);
	int resetGrammer(char * grammerPath, char * tagName);
	int getEngineState();
	void setEngineState(int state);
	int startAsr();
	int startTest(char * wavPath);
	void setCallBack(eventCallBack dllCallBack);
	void saveRecord(int save);
	const char * getVersion();
	void enterAsr();
	static void* enterAsr2(void * p);
	void enterTest();
	static void* enterTest2(void * p);
	static void recordCallBack(void * p);
	void callBack(int event, const char * result, int data);
	int insertVocab(const char * vocab);
	void cancelAsr();
	void releaseEngine();
	void closeFile();
	~NativeEngine();
};


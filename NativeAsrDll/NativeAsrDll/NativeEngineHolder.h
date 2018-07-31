#pragma once
#include "stdafx.h"
#include <stdio.h>
#include "NativeEngine.h" 

extern "C" __declspec(dllexport) int __stdcall initEngine(eventCallBack callBack);
extern "C" __declspec(dllexport) int __stdcall startAsr();
extern "C" __declspec(dllexport) void __stdcall cancelAsr();
extern "C" __declspec(dllexport) int __stdcall insertVocab(const char * vocab);
extern "C" __declspec(dllexport) const char * __stdcall getVersion();
extern "C" __declspec(dllexport) void __stdcall saveRecord(int save);
extern "C" __declspec(dllexport) int __stdcall getEngineState();
extern "C" __declspec(dllexport) void __stdcall setCallBack(eventCallBack callBack);

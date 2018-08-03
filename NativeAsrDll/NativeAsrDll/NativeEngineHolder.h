#pragma once
#include "stdafx.h"
#include <stdio.h>
#include "NativeEngine.h" 

extern "C" __declspec(dllexport) int __stdcall initEngine(char * grammerPath, char * tagName, eventCallBack callBack);
extern "C" __declspec(dllexport) int __stdcall startAsr();
extern "C" __declspec(dllexport) void __stdcall cancelAsr();
extern "C" __declspec(dllexport) const char * __stdcall getVersion();
extern "C" __declspec(dllexport) void __stdcall saveRecord(int save);
extern "C" __declspec(dllexport) void __stdcall resetGrammer(char * grammerPath, char * tagName);
extern "C" __declspec(dllexport) int __stdcall getEngineState();
extern "C" __declspec(dllexport) void __stdcall setCallBack(eventCallBack callBack);
extern "C" __declspec(dllexport) void __stdcall releaseEngine();
extern "C" __declspec(dllexport) int __stdcall setSamplesPerSec(int samplesPerSec);

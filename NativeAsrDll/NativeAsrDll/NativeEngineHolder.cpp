#include "stdafx.h"
#include "NativeEngineHolder.h"

static NativeEngine nativeEngine;
int __stdcall initEngine(char * grammerPath, char * tagName, eventCallBack callBack)
{
	return nativeEngine.initEngine(grammerPath, tagName, callBack);
}
int __stdcall startAsr()
{
	return nativeEngine.startAsr();
}
void __stdcall cancelAsr()
{
	nativeEngine.cancelAsr();
	return;
}
const char * __stdcall getVersion()
{
	return nativeEngine.getVersion();
}
void __stdcall saveRecord(int save)
{
	return nativeEngine.saveRecord(save);
}
int __stdcall getEngineState()
{
	return nativeEngine.getEngineState();
}
void __stdcall setCallBack(eventCallBack callBack)
{
	nativeEngine.setCallBack(callBack);
}
void __stdcall resetGrammer(char * grammerPath, char * tagName)
{
	nativeEngine.resetGrammer(grammerPath, tagName);
}
void __stdcall releaseEngine()
{
	nativeEngine.releaseEngine();
}

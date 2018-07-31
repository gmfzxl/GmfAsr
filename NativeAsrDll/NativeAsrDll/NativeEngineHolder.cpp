#include "stdafx.h"
#include "NativeEngineHolder.h"

static NativeEngine nativeEngine;
int __stdcall initEngine(eventCallBack callBack)
{
	return nativeEngine.initEngine(callBack);
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
int __stdcall insertVocab(const char * vocab)
{
	return nativeEngine.insertVocab(vocab);
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

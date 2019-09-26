#include "tc_functionhook_material.h"
#include <map>
#include "MiniMPL\macro_assert.h"

extern std::map<PVOID, PVOID> hookApis;

void __stdcall myOutputDebugStringA(LPCSTR lpOutputString)
{
	std::map<PVOID, PVOID>::iterator it = hookApis.find(myOutputDebugStringA);
	AssertB((it != hookApis.end()));
	TpOutputDebugStringA pOld = (TpOutputDebugStringA)it->second;

	std::stringstream ss;
	ss << "[in myOutputDebugStringA] " << lpOutputString;
	pOld(ss.str().c_str());
}

int __stdcall myMessageBoxA(__in_opt HWND hWnd, __in_opt LPCSTR lpText, __in_opt LPCSTR lpCaption, __in UINT uType)
{
	std::stringstream ss;
	ss << "myMessageBoxA: ready to call MessageBoxA" << lpText << "\r\n";
	OutputDebugStringA(ss.str().c_str());

	ss.str("");
	ss << "old msg:" << lpText << "\r\n new msg : I am here\r\n";

	std::map<PVOID, PVOID>::iterator it = hookApis.find(myMessageBoxA);
	if (it != hookApis.end())
	{
		TpMessageBoxA pOld = (TpMessageBoxA)it->second;
		pOld(hWnd, ss.str().c_str(), lpCaption, uType);
	}

	ss.str("");
	ss << "myMessageBoxA: after calling MessageBoxA" << lpText << "\r\n";
	OutputDebugStringA(ss.str().c_str());

	return 0;
}

//////////////////////////////////////////////////////////////////////////
bool WINAPI oldFunc(LPCSTR lpFileName)
{
	ALWAYS_LOG_HERE();

	std::stringstream ss;
	ss << "[ oldFunc ] " << lpFileName << "\r\n";
	OutputDebugStringA(ss.str().c_str());
	return true;
}

bool WINAPI newFunc(LPCSTR lpFileName)
{
	ALWAYS_LOG_HERE();

	std::stringstream ss;
	ss << "[ newFunc ] " << lpFileName << "\r\n";
	OutputDebugStringA(ss.str().c_str());

	std::map<PVOID, PVOID>::iterator it = hookApis.find(newFunc);
	if (it != hookApis.end())
	{
		TUserFunc pOld = (TUserFunc)it->second;
		pOld("from newFunc : call oldFunc in newFunc.\r\n");
	}

	return false;
}
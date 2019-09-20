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
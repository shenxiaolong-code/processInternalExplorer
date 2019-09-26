#ifndef tc_functionhook_material_h__
#define tc_functionhook_material_h__
#include <windows.h>

typedef void(__stdcall *pOutputDebugStringA)(LPCSTR lpOutputString);
typedef pOutputDebugStringA  TpOutputDebugStringA;
void __stdcall myOutputDebugStringA(LPCSTR lpOutputString);

//////////////////////////////////////////////////////////////////////////
typedef int(__stdcall *pMessageBoxA) (HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
typedef pMessageBoxA TpMessageBoxA;
int __stdcall myMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);

//////////////////////////////////////////////////////////////////////////
typedef bool(WINAPI *pUserFunc) (LPCSTR lpFileName);
typedef pUserFunc TUserFunc;

bool WINAPI oldFunc(LPCSTR lpFileName);
bool WINAPI newFunc(LPCSTR lpFileName);

#endif // tc_functionhook_material_h__

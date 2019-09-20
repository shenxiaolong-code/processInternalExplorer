#ifndef __STACKEXPLORER_H__
#define __STACKEXPLORER_H__
/********************************************************************
	created:	22:8:2013   14:06
	author:		Shen xiaolong
	
	purpose:	use dbghlp.dll to resolve symbol info. 
                refer to (CodeProject --> StackWalker)
*********************************************************************/
#include <MiniMPL\platformEnv.h>
#include <SystemExplorer/stackInfoDef.h>

#ifdef MSVC
//usage : try {...} __except(handleException(GetExceptionInformation()), EXCEPTION_EXECUTE_HANDLER){...}
void handleException(_In_ _EXCEPTION_POINTERS * pExp );

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class StackExplorer
{
public:
    StackExplorer(_In_ HANDLE  hProcess = GetCurrentProcess());

    //informations about address
    BOOL resloveAddrInfo(_In_ PVOID addr, _Inout_ AddressInfo& addrInfo);
    BOOL resloveModuleInfo(_In_ PVOID addr, _Inout_ ModInfo& modInfo);
    BOOL resloveFuncInfo(_In_ PVOID addr, _Inout_ FuncInfo& funcInfo);
    BOOL resloveSourceInfo(_In_ PVOID addr, _Inout_ stlString& fileName, _Inout_ DWORD& lineNumber);

    //collect information
    BOOL resloveStacks(_Inout_ CallStackS& rStacks,_In_ HANDLE hThread = GetCurrentThread(),_In_ CONTEXT* pThrdContext=NULL);
    BOOL resloveLoadedModules(_Inout_ stlVector<ModInfo>& modules);
    PVOID __stdcall getCallerAddr(unsigned iDeep=1);
    
    BOOL enumModuleSyms(_In_ stlStringA const& modName,_Inout_ stlVector<MOD_SYMBOL_INFO>& syms);

private:
    HANDLE  m_hProcess;    
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct StackInfoDisplayer
{
    void print(_In_ CallStackS const& cs);
    void print(_In_ CallStack const& sk);

    void print(_In_ stlVector<ModInfo> const& mis);
    void print(_In_  ModInfo const& mi);

    void print(_In_  AddressInfo const& ai);  

    void print(_In_ MOD_SYMBOL_INFO const& mi);
    void print(_In_ stlVector<MOD_SYMBOL_INFO> const& msis);

    void printErrorCode(DWORD errNum/*=GetLastError()*/,_Inout_ stlString* pStlStr=NULL);

    void printExceptStack(_In_ EXCEPTION_RECORD const& ExpRecord);
    void printExceptSummary(_In_ EXCEPTION_RECORD const& ExpRecord,_In_  AddressInfo const& ai);
};

#endif   //end of MSVC

#endif // __STACKEXPLORER_H__

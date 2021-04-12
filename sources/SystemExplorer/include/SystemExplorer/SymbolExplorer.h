#ifndef __STACKEXPLORER_H__
#define __STACKEXPLORER_H__
/********************************************************************
	created:	22:8:2013   14:06
	author:		Shen xiaolong
	
	purpose:	use dbghlp.dll to resolve symbol info. 
                refer to (CodeProject --> StackWalker)
*********************************************************************/
#include <MiniMPL\platformEnv.h>
#include <SystemExplorer/SymboldataDef.h>
#include <MiniMPL\macro_makeVar.h>

#ifdef MSVC
//usage : try {...} __except(handleException(GetExceptionInformation()), EXCEPTION_EXECUTE_HANDLER){...}
void handleException(_In_ _EXCEPTION_POINTERS * pExp );

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SymbolEnvironment;
DECLARE_PTRS(SymbolEnvironment);

class SymbolExplorer
{
public:
    SymbolExplorer(_In_ HANDLE  hProcess = GetCurrentProcess());
    ~SymbolExplorer();

    //symbol informations about memory address
    BOOL resloveAddrSymbol_raw(_In_ PVOID addr, _Inout_ STD_SYMBOL_INFO& addrSymbol);
    BOOL resloveAddrSymbol(_In_ PVOID addr, _Inout_ AddressInfo& addrInfo);
    BOOL resloveModuleSymbol(_In_ PVOID addr, _Inout_ ModInfo& modInfo);
    BOOL resloveFuncSymbol(_In_ PVOID addr, _Inout_ FuncInfo& funcInfo);
    BOOL resloveSourceSymbol(_In_ PVOID addr, _Inout_ stlString& fileName, _Inout_ DWORD& lineNumber);

    //symbol informations from function name
    BOOL resloveSymbolFromName(const char* szSymbolName, _Inout_ STD_SYMBOL_INFO& nameInfo);
    BOOL resloveUnDecorateSymbolName(const char* szSymbolName, _Inout_ stlString& rawSymbolName);

    //enumerates loaded modules in specified process
    BOOL resloveLoadedModules(_Inout_ stlVector<ModInfo>& modules);

    //enumerates symbol information in one module
    BOOL enumModuleSymbols(_In_ stlStringA const& modName, _Inout_ stlVector<STD_SYMBOL_INFO>& syms, const char* pMask = "*!*");

protected:
    static std::map<HANDLE, SymbolEnvironmentWptr>  gs_processSymbolMap;

protected:
    SymbolEnvironmentPtr    symInitWithProcess(HANDLE hProcess);
    bool                    isSymbolReady();
protected:
    HANDLE                  m_hProcess;
    SymbolEnvironmentPtr     m_symEnv;
};


class StackExplorer  : public  SymbolExplorer
{
public:
    StackExplorer(_In_ HANDLE  hProcess = GetCurrentProcess());

    //collect information
    BOOL resloveStacks(_Inout_ CallStackS& rStacks,_In_ HANDLE hThread = GetCurrentThread(),_In_ CONTEXT* pThrdContext=NULL);
    PVOID __stdcall getCallerAddr(unsigned iDeep=1);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SymbolInfoDisplayer
{
    void print(_In_ CallStackS const& cs);
    void print(_In_ CallStack const& sk);

    void print(_In_ stlVector<ModInfo> const& mis);
    void print(_In_  ModInfo const& mi);

    void print(_In_  AddressInfo const& ai);  

    void print(_In_ STD_SYMBOL_INFO const& mi);
    void print(_In_ stlVector<STD_SYMBOL_INFO> const& msis);

    void printErrorCode(DWORD errNum/*=GetLastError()*/,_Inout_ stlString* pStlStr=NULL);

    void printExceptStack(_In_ EXCEPTION_RECORD const& ExpRecord);
    void printExceptSummary(_In_ EXCEPTION_RECORD const& ExpRecord,_In_  AddressInfo const& ai);
};

#endif   //end of MSVC

#endif // __STACKEXPLORER_H__

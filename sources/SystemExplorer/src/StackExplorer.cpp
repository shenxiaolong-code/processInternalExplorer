#include <SystemExplorer/StackExplorer.h>

#ifdef MSVC

#include <Psapi.h>
#include <MiniMPL/fromToString.hpp>
#include <MiniMPL/macro_assert.h>
#include <win32/WinApiPack.h>

#pragma comment(lib,"Psapi")
#pragma comment(lib,"Dbghelp")
#pragma comment(lib,"Version")  // for "VerQueryValue"

#pragma region symbolAW
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if USE_UNICODE_STRING
    typedef IMAGEHLP_MODULEW64          TIMAGEHLP_MODULE64;
    typedef SYMBOL_INFOW                TSYMBOL_INFO;

    //IMAGEHLP_LINE64/IMAGEHLP_LINEW64/IMAGEHLP_LINE/IMAGEHLP_LINEW
    typedef IMAGEHLP_LINEW64            TIMAGEHLP_LINE;    //see IMAGEHLP_LINEW::FileName , it is not WCHAR.(MS Bug ??)
    #define my_SymGetLineFromAddr       SymGetLineFromAddrW64

    #define my_SymFromAddr              SymFromAddrW
    #define my_UnDecorateSymbolName     UnDecorateSymbolNameW
    #define my_SymGetModuleInfo64       SymGetModuleInfoW64

    #define my_GetModuleFileNameEx 		GetModuleFileNameExW
    #define my_GetModuleHandle			GetModuleHandleW
    #define my_GetFileVersionInfoSize	GetFileVersionInfoSizeW
    #define my_GetFileVersionInfo		GetFileVersionInfoW
    #define my_VerQueryValue			VerQueryValueW
#else
    typedef IMAGEHLP_MODULE64           TIMAGEHLP_MODULE64;
    typedef SYMBOL_INFO                 TSYMBOL_INFO;  

    typedef IMAGEHLP_LINE64             TIMAGEHLP_LINE;
    #define my_SymGetLineFromAddr       SymGetLineFromAddr64

    #define my_SymFromAddr              SymFromAddr
    #define my_UnDecorateSymbolName     UnDecorateSymbolName
    #define my_SymGetModuleInfo64       SymGetModuleInfo64

    #define my_GetModuleFileNameEx 		GetModuleFileNameExA
    #define my_GetModuleHandle			GetModuleHandleA
    #define my_GetFileVersionInfoSize	GetFileVersionInfoSizeA
    #define my_GetFileVersionInfo		GetFileVersionInfoA
    #define my_VerQueryValue			VerQueryValueA
#endif // USE_UNICODE_STRING

#include "getCurContext.c"
#pragma endregion symbolAW
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename TObj,typename TParam>
struct TThrdCallParam 
{
    TObj&           m_rCaller;
    TParam&         m_rParam;

    TThrdCallParam(TObj& rCaller,TParam& rParam): m_rCaller(rCaller), m_rParam(rParam) {}
};

BOOL CALLBACK SymEnumerateModulesProc64(IN PCSTR ModuleName, IN DWORD64 BaseOfDll,  IN PVOID UserContext )
{
    TThrdCallParam<StackExplorer,stlVector<ModInfo> >* pTypeParam = (TThrdCallParam<StackExplorer,stlVector<ModInfo> >*)UserContext;
    ASSERT_AND_LOG(pTypeParam);

    ModInfo mi;
    pTypeParam->m_rCaller.resloveModuleInfo(PVOID(BaseOfDll),mi);
    pTypeParam->m_rParam.push_back(mi);
    return TRUE;
}

BOOL CALLBACK EnumSymProc(PSYMBOL_INFO pSymInfo,ULONG SymbolSize,PVOID   UserContext)
{
    stlVector<MOD_SYMBOL_INFO>*  pSV = (stlVector<MOD_SYMBOL_INFO>*)UserContext;
    MOD_SYMBOL_INFO rSi;
    rSi = *pSymInfo;
    pSV->push_back(rSi);
    return TRUE;
}

//////////////////////////////////////////////Init symbols //////////////////////////////////////////////////////////////
BOOL    g_bSymInited    = FALSE;
HANDLE  gs_hProcess     = NULL;

void __cdecl symClean()
{
    SymCleanup(gs_hProcess);
}

void __cdecl symInit()
{
    SymSetOptions(SymGetOptions() | SYMOPT_FAIL_CRITICAL_ERRORS | SYMOPT_LOAD_LINES );
    gs_hProcess = GetCurrentProcess();
    g_bSymInited  = SymInitialize(gs_hProcess, NULL, TRUE);

    atexit(symClean);
}

#define SECNAME ".CRT$XCA"
#pragma section(SECNAME,long, read) 
typedef void (__cdecl *_PVFV)();
__declspec(allocate(SECNAME)) _PVFV dummy[] = { symInit };

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
StackExplorer::StackExplorer(HANDLE  hProcess)
: m_hProcess(hProcess)
{
    if (!g_bSymInited)
    {
        symInit();
    }
}

BOOL StackExplorer::resloveAddrInfo( _In_ PVOID addr, _Inout_ AddressInfo& addrInfo )
{
    if (!g_bSymInited)
    {
        return FALSE;
    }

    memset(&addrInfo,0,sizeof(AddressInfo));
    addrInfo.m_addr             = addr;
    resloveModuleInfo(addr,addrInfo.m_mod);    
    resloveFuncInfo(addr,addrInfo.m_func);    
    resloveSourceInfo(addr,addrInfo.m_fileName,addrInfo.m_lineNumber);
    return TRUE;
}

BOOL StackExplorer::resloveModuleInfo( _In_ PVOID addr, _Inout_ ModInfo& modInfo )
{
    if (!g_bSymInited)
    {
        return FALSE;
    }

    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(addr,&mbi,sizeof(mbi)))  //or GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,LPCTSTR(addr),modInfo.m_modHandle)
    {
        if ((mbi.State != MEM_COMMIT) || ((mbi.Protect & 0xff) == PAGE_NOACCESS) || (mbi.Protect & PAGE_GUARD)) 
        {           
            return false;     //ERROR_BAD_EXE_FORMAT
        }
        modInfo.m_baseAddr   = mbi.AllocationBase;        

        TIMAGEHLP_MODULE64  im = {0};
        im.SizeOfStruct = sizeof(im);
        if (!my_SymGetModuleInfo64(m_hProcess,DWORD64(addr),&im))
        {
            StackInfoDisplayer sid;
            sid.printErrorCode(GetLastError());
        }
        modInfo.m_timeDateStamp = im.TimeDateStamp;
        modInfo.m_checkSum      = im.CheckSum;
        modInfo.m_numSyms       = im.NumSyms;
        modInfo.m_symType       = im.SymType;
        modInfo.m_pdbFile       = MiniMPL::toString(im.CVData);

        MODULEINFO mi;
        if (GetModuleInformation(m_hProcess,HMODULE(mbi.AllocationBase),&mi,sizeof(MODULEINFO)))
        {   //or SymGetModuleInfo 
            modInfo.m_size       = mi.SizeOfImage;
            modInfo.m_entryPoint = mi.EntryPoint;
            if (modInfo.m_entryPoint != addr)  //avoid fall in loop
            {
                FuncInfo fi;
                resloveFuncInfo(modInfo.m_entryPoint,fi);
                modInfo.m_entryName = fi.m_name;
            }
        }

        stlChar   buf256[256] = {0};
        if(my_GetModuleFileNameEx(m_hProcess,HMODULE(mbi.AllocationBase),buf256,256))
        {
            modInfo.m_name = buf256;
        }
        modInfo.m_modHandle  = my_GetModuleHandle(buf256);

        DWORD dwHandle=0;
        int versionSize = my_GetFileVersionInfoSize(modInfo.m_name.c_str(),&dwHandle);
        if (versionSize)
        {
            stlVector<unsigned char>    buf(versionSize);
            my_GetFileVersionInfo(modInfo.m_name.c_str(),DWORD(modInfo.m_modHandle),buf.size(),&*buf.begin());

            UINT len=0;
            VS_FIXEDFILEINFO *fInfo = NULL;
            if (my_VerQueryValue(&*buf.begin(), TXT("\\"), (LPVOID*) &fInfo, &len))
            {
                modInfo.m_verion = ((ULONGLONG)fInfo->dwFileVersionLS) + ((ULONGLONG)fInfo->dwFileVersionMS << 32);
            }
        }

        return TRUE;
    }

    return FALSE;
}

BOOL StackExplorer::resloveFuncInfo( _In_ PVOID addr, _Inout_ FuncInfo& funcInfo )
{
    if (!g_bSymInited)
    {
        return FALSE;
    }

    DWORD64 asmDisplacement;
    ULONG64 buffer[(sizeof(TSYMBOL_INFO)+MAX_SYM_NAME*sizeof(TCHAR)+sizeof(ULONG64)-1)/sizeof(ULONG64)];
    TSYMBOL_INFO* pSymbol = (TSYMBOL_INFO*)buffer;
    pSymbol->SizeOfStruct = sizeof(TSYMBOL_INFO);
    pSymbol->MaxNameLen = MAX_SYM_NAME; 

    if (my_SymFromAddr(m_hProcess,DWORD64(addr),&asmDisplacement,pSymbol))
    {
        funcInfo.m_addr  = PVOID(pSymbol->Address);
        funcInfo.m_name  = pSymbol->Name;               //UnDecorateSymbolName
        funcInfo.m_AsmOffset= DWORD(asmDisplacement);

        stlChar undName[256]={0};
        my_UnDecorateSymbolName(pSymbol->Name,undName,ARRAYSIZE(undName),UNDNAME_NAME_ONLY);
        funcInfo.m_unDecorateName = undName;
        return TRUE;
    }

    return FALSE;
}

BOOL StackExplorer::resloveSourceInfo(_In_ PVOID addr, _Inout_ stlString& fileName, _Inout_ DWORD& lineNumber)
{
    fileName    = TXT("");
    lineNumber  = -1;
    if (!g_bSymInited)
    {
        return FALSE;
    }

    TIMAGEHLP_LINE line;
    line.SizeOfStruct = sizeof(TIMAGEHLP_LINE);
    DWORD lineDisplacement;
    if(my_SymGetLineFromAddr(m_hProcess,DWORD64(addr),&lineDisplacement,&line))
    {
        fileName    = line.FileName;
        lineNumber  = line.LineNumber;
        return TRUE;
    }

    return FALSE;
}

BOOL StackExplorer::resloveLoadedModules(_Inout_ stlVector<ModInfo>& modules)
{
    TThrdCallParam<StackExplorer,stlVector<ModInfo> > rParam(*this,modules);
    return SymEnumerateModules64(m_hProcess,SymEnumerateModulesProc64,&rParam);
}

#pragma optimize( "", off )
PVOID StackExplorer::getCallerAddr(unsigned iDeep)
{  
    // recurse in the stack frame iDeep times
    // note iDeep doesn't include this method, this is added by this script
    DWORD stack_return_addr;
    __asm {
        mov eax, 1
        mov ebx, iDeep
        mov ecx, [ebp]
startloop:
        cmp eax, ebx
        jnb endloop
        mov ecx, [ecx]
        inc eax
        jmp startloop
endloop:
        mov ecx, [ecx+4]
        mov [stack_return_addr], ecx
    }
    return (PVOID)stack_return_addr;
}
#pragma optimize( "", on )

BOOL StackExplorer::enumModuleSyms( _In_ stlStringA const& modName,_Inout_ stlVector<MOD_SYMBOL_INFO>& syms )
{
    DWORD64 BaseOfDll = SymLoadModule64(m_hProcess,NULL,modName.c_str(),NULL,0,0);
    SymEnumSymbols(m_hProcess, BaseOfDll, NULL, EnumSymProc, &syms);
    SymUnloadModule64(m_hProcess, BaseOfDll);
    return syms.size()>0;
}

#pragma optimize("", on)     //resolve warning C4748
BOOL StackExplorer::resloveStacks( _Inout_ CallStackS& rStacks,_In_ HANDLE hThread /*= GetCurrentThread()*/,_In_ CONTEXT* pThrdContext/*=NULL*/ )
{
    //refer to : .\VC\atlmfc\include\atlutil.h
    //ATL::_AtlThreadContextInfo::DoDumpStack
    if (!g_bSymInited)
    {
        return FALSE;
    }

    rStacks.m_processId  = GetProcessId(m_hProcess);
    rStacks.m_threadId   = GetThreadId(hThread); 

    bool bIsCurThread = hThread==GetCurrentThread();
    CONTEXT context; 
    if (pThrdContext)
    {
        context = *pThrdContext;
    }
    else
    {
        if (bIsCurThread)
        {
            GET_CURRENT_CONTEXT(context,CONTEXT_FULL);  //RtlCaptureContext(&context);
        }
        else
        {
            SuspendThread(hThread);
            GetThreadContext(hThread,&context);        
        }
    }

    STACKFRAME stackFrame;
    memset(&stackFrame, 0, sizeof(stackFrame));
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Mode = AddrModeFlat;
    stackFrame.AddrReturn.Mode = AddrModeFlat;
    stackFrame.AddrBStore.Mode = AddrModeFlat;

    DWORD dwMachType;
#if defined(_M_IX86)
    dwMachType                   = IMAGE_FILE_MACHINE_I386;
    stackFrame.AddrPC.Offset     = context.Eip;  //program counter
    stackFrame.AddrStack.Offset  = context.Esp;  //stack pointer
    stackFrame.AddrFrame.Offset  = context.Ebp;  //frame pointer
#else
    //refer to ATL::_AtlThreadContextInfo::DoDumpStack to impl non-x86 machine
#error("Unknown Target Machine");
#endif

    while (StackWalk(dwMachType, m_hProcess, hThread,&stackFrame, &context, NULL,
        SymFunctionTableAccess, SymGetModuleBase, NULL))
    {
        if (stackFrame.AddrPC.Offset != 0)
        {
            CallStack   sk;            
            FuncInfo    fi;
            if (resloveFuncInfo(PVOID(stackFrame.AddrPC.Offset),fi))
            {
                sk.m_funcAddr   = fi.m_addr;
                sk.m_funcName   = fi.m_name;
                sk.m_AsmOffset     = fi.m_AsmOffset;
            }

            Static_Assert(sizeof(sk.m_lineNumber)>=sizeof(DWORD));
            resloveSourceInfo(PVOID(stackFrame.AddrPC.Offset),sk.m_srcFileName,(DWORD&)sk.m_lineNumber);
            if (1)
            {
                //outputTxtV(TXT("%s(%d) :%s\n"),sk.m_srcFileName.c_str(),sk.m_lineNumber,sk.m_funcName.c_str());
            }

            if (0 != sk.m_funcAddr)
            {
                rStacks.m_stacks.push_back(sk);
            }
        }
    }

    if (!bIsCurThread) ResumeThread(hThread);

    return TRUE;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SIZE_1M      (1024*1024)

void handleException(_In_ _EXCEPTION_POINTERS * pExp )
{
    StackInfoDisplayer sid;
    sid.printExceptStack(*pExp->ExceptionRecord);

    using namespace std;
    StackExplorer se(GetCurrentProcess());
    stlVector<ModInfo> modules;
    se.resloveLoadedModules(modules);
    sid.print(modules);
    
    AddressInfo ai;
    se.resloveAddrInfo(pExp->ExceptionRecord->ExceptionAddress,ai);
    sid.printExceptSummary(*pExp->ExceptionRecord,ai);
    sid.print(ai);

    CallStackS cs;
    if (se.resloveStacks(cs,GetCurrentThread(),pExp->ContextRecord))
    {
        sid.print(cs);
    }
    else
    {
        outputTxt(TXT("Fails to get thread stacks."));
    }

    if (EXCEPTION_STACK_OVERFLOW==pExp->ExceptionRecord->ExceptionCode)
    {
        outputTxt(TXT("\n[stack overflow] try to expand stack space\n"));
        ULONG mem2m = 3*SIZE_1M;
        if (SetThreadStackGuarantee(&mem2m))
        {   //PNT_TIB pTeb=(PNT_TIB)NtCurrentTeb();
            _resetstkoflw();
        }
        else
        {
            stlString& errMsg = OS_Win32::GetLastErrorText(GetLastError());
            outputTxtV(TXT("[Fails to expand stack] %s"),errMsg.c_str());
        }
    }
}

#endif //end of MSVC

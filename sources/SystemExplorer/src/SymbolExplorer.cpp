#include <SystemExplorer/SymbolExplorer.h>

#ifdef MSVC

#include <Psapi.h>
#include <MiniMPL/macro_assert.h>
#include <win32/WinApiPack.h>
#include <map>
#include <MiniMPL/stdwrapper.hpp>

#pragma comment(lib,"Psapi")
#pragma comment(lib,"Dbghelp")
#pragma comment(lib,"Version")              // for "VerQueryValue"
#pragma comment(lib, "comsuppw.lib")        //for _bstr_t string


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
//////////////////////////////////////////////Init symbols //////////////////////////////////////////////////////////////
struct SymbolEnvironment
{
    SymbolEnvironment(HANDLE  hProcess) : m_hProcess(hProcess) 
    {
        SymSetOptions(SymGetOptions() | SYMOPT_FAIL_CRITICAL_ERRORS | SYMOPT_LOAD_LINES);
        m_bSymInited = SymInitialize(m_hProcess, NULL, TRUE);
        if (!m_bSymInited)
        {
            m_lastErrorCode = GetLastError();
        }
    };
    ~SymbolEnvironment()
    {
        if (m_bSymInited)
        {
            SymCleanup(m_hProcess);
        }        
    }
    operator bool()
    {
        return m_bSymInited;
    }

    BOOL    m_bSymInited    = FALSE;
    HANDLE  m_hProcess      = NULL;
    DWORD   m_lastErrorCode = S_OK;
};
enum class __scrt_native_startup_state;
extern "C" extern __scrt_native_startup_state __scrt_current_native_startup_state;
bool isGlobalInitState()
{
    // 2: __scrt_native_startup_state::initialized
    return 2 != int(__scrt_current_native_startup_state);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename TObj,typename TParam>
struct TThrdCallParam 
{
    TObj&           m_rCaller;
    TParam&         m_rParam;

    TThrdCallParam(TObj& rCaller,TParam& rParam): m_rCaller(rCaller), m_rParam(rParam) {}
};

BOOL CALLBACK SymEnumerateModules64_callback(IN PCSTR ModuleName, IN DWORD64 BaseOfDll,  IN PVOID UserContext )
{
    TThrdCallParam<SymbolExplorer,stlVector<ModInfo> >* pTypeParam = (TThrdCallParam<SymbolExplorer,stlVector<ModInfo> >*)UserContext;
    ASSERT_AND_LOG(pTypeParam);

    ModInfo mi;
    pTypeParam->m_rCaller.resloveModuleSymbol(PVOID(BaseOfDll),mi);
    pTypeParam->m_rParam.push_back(mi);
    return TRUE;
}

BOOL CALLBACK SymEnumSymbols_callback(PSYMBOL_INFO pSymInfo,ULONG SymbolSize,PVOID   UserContext)
{
    stlVector<STD_SYMBOL_INFO>*  pSV = (stlVector<STD_SYMBOL_INFO>*)UserContext;
    STD_SYMBOL_INFO rSi;
    rSi = *pSymInfo;
    pSV->push_back(rSi);
    return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::map<HANDLE, SymbolEnvironmentWptr>  SymbolExplorer::gs_processSymbolMap;
SymbolExplorer::SymbolExplorer(_In_ HANDLE hProcess /*= GetCurrentProcess()*/)
    : m_hProcess(hProcess)
{
    m_symEnv = symInitWithProcess(m_hProcess);
}

SymbolExplorer::~SymbolExplorer()
{
}

BOOL SymbolExplorer::resloveAddrSymbol_raw(_In_ PVOID addr, _Inout_ STD_SYMBOL_INFO& addrSymbol)
{  
    if (isSymbolReady())
    {
        return FALSE;
    }

    DWORD64 asmDisplacement = 0;
    ULONG64 buffer[(sizeof(TSYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR) + sizeof(ULONG64) - 1) / sizeof(ULONG64)] = {0};
    TSYMBOL_INFO* pSymbol = (TSYMBOL_INFO*)buffer;
    pSymbol->SizeOfStruct = sizeof(TSYMBOL_INFO);
    pSymbol->MaxNameLen = MAX_SYM_NAME;

    if (my_SymFromAddr(m_hProcess, DWORD64(addr), &asmDisplacement, pSymbol))
    {
        addrSymbol = *pSymbol;
        addrSymbol.asmOffset = asmDisplacement;
        return TRUE;
    }

    return FALSE;
}

BOOL SymbolExplorer::resloveAddrSymbol( _In_ PVOID addr, _Inout_ AddressInfo& addrInfo )
{
    if (isSymbolReady())
    {
        return FALSE;
    }

    memset(&addrInfo,0,sizeof(AddressInfo));
    addrInfo.m_addr             = addr;
    resloveModuleSymbol(addr,addrInfo.m_mod);    
    resloveFuncSymbol(addr,addrInfo.m_func);   
    stlString fileName;
    resloveSourceSymbol(addr, fileName,addrInfo.m_lineNumber);
    addrInfo.m_fileName = fileName.c_str();
    return TRUE;
}

BOOL SymbolExplorer::resloveModuleSymbol( _In_ PVOID addr, _Inout_ ModInfo& modInfo )
{
    if (isSymbolReady())
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
            SymbolInfoDisplayer sid;
            sid.printErrorCode(GetLastError());
        }
        modInfo.m_timeDateStamp = im.TimeDateStamp;
        modInfo.m_checkSum      = im.CheckSum;
        modInfo.m_numSyms       = im.NumSyms;
        modInfo.m_symType       = im.SymType;
        modInfo.m_pdbFile       = im.CVData;

        MODULEINFO mi;
        if (GetModuleInformation(m_hProcess,HMODULE(mbi.AllocationBase),&mi,sizeof(MODULEINFO)))
        {   //or SymGetModuleInfo 
            modInfo.m_size       = mi.SizeOfImage;
            modInfo.m_entryPoint = mi.EntryPoint;
            if (modInfo.m_entryPoint != addr)  //avoid fall in loop
            {
                FuncInfo fi;
                resloveFuncSymbol(modInfo.m_entryPoint,fi);
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
        int versionSize = my_GetFileVersionInfoSize(modInfo.m_name,&dwHandle);
        if (versionSize)
        {
            stlVector<unsigned char>    buf(versionSize);
            my_GetFileVersionInfo(modInfo.m_name,DWORD(modInfo.m_modHandle),buf.size(),&*buf.begin());

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

BOOL SymbolExplorer::resloveFuncSymbol( _In_ PVOID addr, _Inout_ FuncInfo& funcInfo )
{
    if (isSymbolReady())
    {
        return FALSE;
    }

    STD_SYMBOL_INFO addrSymbol;
    if (resloveAddrSymbol_raw(addr,addrSymbol))
    {
        funcInfo.m_addr         = PVOID(addrSymbol.Address);
        funcInfo.m_name         = addrSymbol.name;      //UnDecorateSymbolName
        funcInfo.m_asmOffset    = addrSymbol.asmOffset;

        stlChar undName[256]={0};
        stlString stdName = addrSymbol.name;
        my_UnDecorateSymbolName(stdName.c_str(),undName,ARRAYSIZE(undName),UNDNAME_NAME_ONLY);
        funcInfo.m_unDecorateName = undName;
        return TRUE;
    }

    return FALSE;
}

BOOL SymbolExplorer::resloveSourceSymbol(_In_ PVOID addr, _Inout_ stlString& fileName, _Inout_ DWORD& lineNumber)
{
    fileName    = TXT("");
    lineNumber  = -1;
    if (isSymbolReady())
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
       
BOOL SymbolExplorer::resloveSymbolFromName(const char* szSymbolName, _Inout_ STD_SYMBOL_INFO& nameInfo)
{   //e.g. : resloveSymbolFromName("BaseThreadInitThunk");	
    if (isSymbolReady())
    {
        return FALSE;
    }

    ULONG64 buffer[(sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR) + sizeof(ULONG64) - 1) / sizeof(ULONG64)] = { 0 };
    PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
    pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    pSymbol->MaxNameLen = MAX_SYM_NAME;
    if (SymFromName(m_hProcess, szSymbolName, pSymbol))
    { //https://docs.microsoft.com/en-us/windows/win32/debug/retrieving-symbol-information-by-name
        nameInfo = *pSymbol;
        return TRUE;
    }
    return FALSE;
}

BOOL SymbolExplorer::resloveUnDecorateSymbolName(const char* szSymbolName, _Inout_ stlString& rawSymbolName)
{
    STD_SYMBOL_INFO ssi;
    if (resloveSymbolFromName(szSymbolName,ssi))
    {  
        TCHAR szUndName[256] = { 0 };
        stlString stlName = ssi.name;
        if (my_UnDecorateSymbolName(stlName.c_str(), szUndName, sizeof(szUndName), UNDNAME_COMPLETE))
        {
            rawSymbolName = szUndName;
            return TRUE;
        }
    }
    return FALSE;
}

BOOL SymbolExplorer::resloveLoadedModules(_Inout_ stlVector<ModInfo>& modules)
{
    if (isSymbolReady())
    {
        return FALSE;
    }

    TThrdCallParam<SymbolExplorer,stlVector<ModInfo> > rParam(*this,modules);
    return SymEnumerateModules64(m_hProcess,SymEnumerateModules64_callback,&rParam);
}

SymbolEnvironmentWptr       gs_thisProcSymInitWptr;
SymbolEnvironmentPtr SymbolExplorer::symInitWithProcess(HANDLE hProcess)
{
    if (isGlobalInitState())
    {   //before main function, std lib is not initialized. so std object is not used.
        SymbolEnvironmentPtr pEvn = gs_thisProcSymInitWptr.lock();
        if (pEvn)
        {  
            return pEvn;
        }
        else
        { 
            SymbolEnvironmentPtr pEvn = _sharedPtrGenerater(hProcess);
            gs_thisProcSymInitWptr = pEvn;
            return pEvn;
        }        
    }

    auto pEnv = StackExplorer::gs_processSymbolMap.find(hProcess);
    if (StackExplorer::gs_processSymbolMap.end() == pEnv || !pEnv->second.lock())
    {
        SymbolEnvironmentPtr pEvn = _sharedPtrGenerater(hProcess);
        if (pEvn && *pEvn)
        {
            gs_processSymbolMap.emplace(hProcess, pEvn);
            return pEvn;
        }
        else
        {
            return nullptr;
        }
    }
    else
    {
        return pEnv->second.lock();
    }
}

bool SymbolExplorer::isSymbolReady()
{   
    return m_symEnv && !*m_symEnv;
}

BOOL SymbolExplorer::enumModuleSymbols(_In_ stlStringA const& modName, _Inout_ stlVector<STD_SYMBOL_INFO>& syms, const char* pMask/*="*!*"*/)
{
    //PLOADED_IMAGE	ploadImage = ImageLoad(modName, NULL);
    //intptr_t      BaseOfDll  = SymLoadModule(m_hProcess, ploadImage->hFile, modName, pdbName, 0, ploadImage->SizeOfImage);
    //e.g. : SymLoadModule(,,"win32k.sys", "win32k.pdb", 0, ploadImage->SizeOfImage)    

    DWORD64 BaseOfDll = SymLoadModule64(m_hProcess,NULL,modName.c_str(),NULL,0,0);
    SymEnumSymbols(m_hProcess, BaseOfDll, pMask, SymEnumSymbols_callback, &syms);
    SymUnloadModule64(m_hProcess, BaseOfDll);
    //ImageUnload(ploadImage);
    return syms.size()>0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
StackExplorer::StackExplorer(HANDLE  hProcess)
: SymbolExplorer(hProcess)
{
    
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

#pragma optimize("", on)     //resolve warning C4748
BOOL StackExplorer::resloveStacks( _Inout_ CallStackS& rStacks,_In_ HANDLE hThread /*= GetCurrentThread()*/,_In_ CONTEXT* pThrdContext/*=NULL*/ )
{
    //refer to : .\VC\atlmfc\include\atlutil.h
    //ATL::_AtlThreadContextInfo::DoDumpStack
    if (isSymbolReady())
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
            if (resloveFuncSymbol(PVOID(stackFrame.AddrPC.Offset),fi))
            {
                sk.m_funcAddr   = fi.m_addr;
                sk.m_funcName   = fi.m_name;
                sk.m_asmOffset     = fi.m_asmOffset;
            }

            Static_Assert(sizeof(sk.m_lineNumber)>=sizeof(DWORD));
            stlString srcFileName;
            if (resloveSourceSymbol(PVOID(stackFrame.AddrPC.Offset), srcFileName, sk.m_lineNumber))
            {
                sk.m_srcFileName = srcFileName.c_str();
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
    SymbolInfoDisplayer sid;
    sid.printExceptStack(*pExp->ExceptionRecord);

    using namespace std;
    StackExplorer se(GetCurrentProcess());
    stlVector<ModInfo> modules;
    se.resloveLoadedModules(modules);
    sid.print(modules);
    
    AddressInfo ai;
    se.resloveAddrSymbol(pExp->ExceptionRecord->ExceptionAddress,ai);
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

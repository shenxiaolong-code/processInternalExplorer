#include <SystemExplorer/SymbolExplorer.h>
#ifdef MSVC

#include <MiniMPL/productSpecialDef.h>
#include <MiniMPL/macro_makeTxt.h>
#include <MiniMPL/macro_assert.h>
#include <MiniMPL/fromToString.hpp>
#include <time.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if USE_UNICODE_STRING
    typedef LPTSTR                      TFormatMessageBuf;
    #define my_FormatMessage            FormatMessageW

    #define my_sprintf                 	swprintf_s   
    #define my_strlen                 	wcslen
    #define my_memset                 	wmemset
#else
    typedef LPSTR                       TFormatMessageBuf;
    #define my_FormatMessage            FormatMessageA

    #define my_sprintf                	sprintf_s  
    #define my_strlen                 	strlen
    #define my_memset                 	memset
#endif // USE_UNICODE_STRING

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
stlString   version2Str(unsigned __int64 fileVersion,int alignLen=-1);  //-1: auto ; 0 : No align ; N : align length
stlString   timeDateStamp2Str(DWORD tds,int alignLen=0);                //-1: auto ; 0 : No align ; N : align length

template<typename Char_T>
bool padding(Char_T* pStr,unsigned int MaxLen,unsigned int alignLen,Char_T paddingChar=Char_T(' '))
{
    if(alignLen >= MaxLen)
        return false;

    unsigned int size = my_strlen(pStr);
    unsigned int validSize = min(MaxLen-1,alignLen);
    if (size < validSize)
    {
        my_memset(&pStr[size],paddingChar,validSize-size);
        pStr[validSize]=0;
    }
    return true;
}

template<typename Char_T,unsigned int TLEN>
bool padding(Char_T (&rArr)[TLEN],unsigned int alignLen,Char_T paddingChar=Char_T(' '))
{
    return padding(rArr,TLEN,alignLen,paddingChar);
}


stlString version2Str( unsigned __int64 fileVersion,int alignLen/*=-1*/ )
{   //below form(15 chars) : 6.1.7601.17514
    stlChar  buf256[256]={0}; 
    ASSERT_AND_LOG(alignLen<int(ARRAYSIZE(buf256)));
    DWORD v4 = (DWORD) fileVersion & 0xFFFF;
    DWORD v3 = (DWORD) (fileVersion>>16) & 0xFFFF;
    DWORD v2 = (DWORD) (fileVersion>>32) & 0xFFFF;
    DWORD v1 = (DWORD) (fileVersion>>48) & 0xFFFF;
    my_sprintf(buf256, TXT("%d.%d.%d.%d"), v1, v2, v3, v4);
    if (0 != alignLen)
    {
        padding(buf256,(-1 == alignLen) ? 15 : max(15,alignLen));
    }
    return stlString(buf256);
}

stlString timeDateStamp2Str( DWORD tds,int alignLen/*=0*/ )
{
#pragma warning(disable:4996)
    char* pStr= _ctime64((time_t*)(&tds));  //below form : Wed Jan 02 02:03:55 1980\n\0
    stlString ts = pStr ? MiniMPL::toString(ctime((time_t*)(&tds))) : TXT("[invalid  timeDateStamp]\n");
    ts[ts.size()-1] = 0;
    ts.erase(--ts.end());    //remove '\n'
    return ts;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SymbolInfoDisplayer::print( CallStackS const& cs )
{
    using namespace std;
    outputTxt ((TXT("\n********************************************************************************************\n")))    ;
    outputTxtV( TXT("  Dump current process[ %d ] - thread [ %d ] stack : \n"),cs.m_processId,cs.m_threadId)  ; 
    outputTxt ( TXT("********************************************************************************************\n"))    ;
    outputTxt ( TXT("Func. Addr\tAddr. offset\tFunc. Name\n"));
    for (stlVector<CallStack>::const_iterator it=cs.m_stacks.begin();it!=cs.m_stacks.end();++it)
    {
        print(*it);
    }
}

void SymbolInfoDisplayer::print( _In_ CallStack const& sk)
{
    using namespace std;
    if (sk.m_funcAddr)
    {
        outputTxtV(TXT("0X%p\t0X%p\t\t%s\n"),sk.m_funcAddr,sk.m_asmOffset,(TCHAR*)sk.m_funcName);
    }

    if (nullptr != (TCHAR*)sk.m_srcFileName)
    { 
        outputTxtV(TXT("%s(%d)\n"), (TCHAR*)sk.m_srcFileName,sk.m_lineNumber);
    }
}

void SymbolInfoDisplayer::print( _In_ stlVector<ModInfo> const& modules )
{
    outputTxtV(TXT("Loaded module number :\t%d\nModule Range\t\t\tmofule version\ttmodule timeStamp\t\t\tModule path\n"),modules.size());
    for (stlVector<ModInfo>::const_iterator it=modules.begin();it!=modules.end();++it)
    {
        print(*it);
    }
}

void SymbolInfoDisplayer::print( _In_ AddressInfo const& ai )
{
    outputTxtV(TXT("\nAddressInfo:0X%p\n%s(%d) : %s\n"),ai.m_addr, (TCHAR*)ai.m_fileName,ai.m_lineNumber, (TCHAR*)ai.m_func.m_name);
    outputTxtV(TXT("\nASM function Info\nFunc. Addr\tExp. offset\tFunc. Name\n")); 
    outputTxtV(TXT("0X%p\t0X%p\t%s\n"),ai.m_func.m_addr,ai.m_func.m_asmOffset, (TCHAR*)ai.m_func.m_name);
}

void SymbolInfoDisplayer::print( _In_ ModInfo const& mi )
{
    stlString&  tds = timeDateStamp2Str(mi.m_timeDateStamp);
    stlString&  ver = version2Str(mi.m_verion);
    outputTxtV(TXT("0X%p-0x%p\t%s\t%s\t%s\t\t%s\n"),mi.m_baseAddr,(DWORD)(mi.m_baseAddr)+mi.m_size, ver.c_str(),tds.c_str(), (TCHAR*)mi.m_name, (TCHAR*)mi.m_pdbFile);
}

void SymbolInfoDisplayer::print( _In_ STD_SYMBOL_INFO const& msi )
{
    //  static MOD_SYMBOL_INFO last=msi;
    //  ASSERT_AND_LOG(last.TypeIndex==msi.TypeIndex);
    //  ASSERT_AND_LOG(last.ModBase==msi.ModBase);
    //  ASSERT_AND_LOG(last.Register==msi.Register);
    //  ASSERT_AND_LOG(last.Scope==msi.Scope);
    //  ASSERT_AND_LOG(last.Tag==msi.Tag);
    //  last=msi;

    outputTxtV(TXT("0x%-10X"), msi.Address);
    outputTxtV(TXT("%-4d\t"), msi.Size);
    outputTxtV(TXT("%-5d\t"), msi.Index);
    //outputTxtV(TXT("%-d\t"), msi.Tag);
    outputTxtV(TXT("0x%-X\t"), msi.Flags);
    //outputTxtV(TXT("%-d\t"), msi.TypeIndex);
    outputTxtV(TXTA("%s\t\n"), (TCHAR*)msi.name);
}

void SymbolInfoDisplayer::print( _In_ stlVector<STD_SYMBOL_INFO> const& msis )
{
    outputTxtV(TXT("\nLoaded symbol number :\t%d\n"),msis.size());

    STD_SYMBOL_INFO const& msi=msis.front();
    if (msis.size()>0)
    {
        outputTxt(TXT("ModBase\t\tTypeIndex\tRegister\tScope\tTag\n"));
        outputTxtV(TXT("0x%-10X%-12d0X%-10.8X%-8d%d\n"),
            msi.ModBase,msi.TypeIndex,msi.Register,msi.Scope,msi.Tag);
    }

    outputTxt(TXT("\nAddress\t\tSize\tIndex\tFlags\tname\n"));
    for (stlVector<STD_SYMBOL_INFO>::const_iterator it=msis.begin();it!=msis.end();++it)
    {
        print(*it);
    }
}

void SymbolInfoDisplayer::printErrorCode( DWORD errNum/*=GetLastError()*/,_Inout_ stlString* pStlStr )
{
    stlString errStr;

    if (S_OK == errNum) 
    {
        errStr = TXT("S_OK");
    }
    else
    {
        stlChar* lpMsgBuf = NULL;
        my_FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,errNum,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (TFormatMessageBuf) &lpMsgBuf,0, NULL );

        errStr = (lpMsgBuf ? lpMsgBuf : TXT("unknow Error"));
        //MessageBox(NULL, (LPCTSTR)lpMsgBuf, TEXT("Error"), MB_OK); 
        LocalFree(lpMsgBuf);
    }

    if (pStlStr)
    {
        *pStlStr = errStr;
    }
    else
    {
        outputTxt(errStr.c_str());
    }
}

void SymbolInfoDisplayer::printExceptStack( _In_ EXCEPTION_RECORD const& ExpRecord )
{
    using namespace std;
    using namespace MiniMPL;
    outputTxtV(TXT("\nException Code : 0X%X \tContinuable ?\t%s\n"),ExpRecord.ExceptionCode,(EXCEPTION_NONCONTINUABLE==ExpRecord.ExceptionFlags ? TXT("No") : TXT("Yes")));

    ExceptionItem const* pExpText=NULL ;
    for(unsigned idx=0;idx<arraySize(exps);++idx)
    {                                           
        if (exps[idx].m_expCode==ExpRecord.ExceptionCode)
        {
            pExpText=&exps[idx];
            break;
        }
    }
    if (pExpText)
    {
        outputTxtV(TXT("code explanation : %s\n"),pExpText->m_expText);
    }

    if (ExpRecord.NumberParameters)
    {
        outputTxt(TXT("parameters associated with the exception : "));
        for(DWORD i=0;i<ExpRecord.NumberParameters;++i)
        {   
            outputTxtV(TXT("0X%X\t"),ExpRecord.ExceptionInformation[i]);
        }
        outputTxt(TXT("\n"));

        switch(ExpRecord.ExceptionCode)
        { //ms-help://MS.MSDNQTR.v90.en/debug/base/exception_record_str.htm
        case EXCEPTION_ACCESS_VIOLATION:
        case EXCEPTION_IN_PAGE_ERROR:
            {
                enum Exp_OP {EXP_Read = 0, EXP_Write = 1,EXP_Execute = 8};
                Exp_OP op = Exp_OP(ExpRecord.ExceptionInformation[0]);
                outputTxtV(TXT("Access Operation: %s\n"),EXP_Read == op ? TXT("Read") : (EXP_Write==op ? TXT("Write"):TXT("Execute")));
                outputTxtV(TXT("Exception Addr  : 0X%p\n\n"),ExpRecord.ExceptionInformation[1]);
            }
            break;
        default:
            break;
        }
    }

    if (ExpRecord.ExceptionRecord)
    {
        outputTxt(TXT("Additional info ")) ;
        printExceptStack(*ExpRecord.ExceptionRecord);
    }
}

void SymbolInfoDisplayer::printExceptSummary( _In_ EXCEPTION_RECORD const& rExp,_In_ AddressInfo const& ai )
{
    outputTxtV(TXT("\nException info \nExp. Code\tExp.Addr\tModule\t\t\t\t\tModule name\n"));
    outputTxtV(TXT("0X%X\t0X%p\t0X%p-0X%p\t%s\n"),rExp.ExceptionCode,rExp.ExceptionAddress,ai.m_mod.m_baseAddr,(DWORD)(ai.m_mod.m_baseAddr)+ai.m_mod.m_size, (TCHAR*)ai.m_mod.m_name);
}

#endif


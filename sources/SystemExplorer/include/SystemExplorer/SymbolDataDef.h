#ifndef SYMINFODEF_H__
#define SYMINFODEF_H__
/************************************************************************************
author  :	Shen xiaolong , xlshen@126.com ,2009-2017
verified: 	VS2008	
purpose :	data structure definition for StackExplorer
*************************************************************************************/
#include <MiniMPL\platformEnv.h>

#ifdef MSVC
#include <MiniMPL/productSpecialDef.h>
#include <Windows.h>

#pragma warning(push)
#pragma warning(disable:4091)
#include <imagehlp.h>
#pragma warning(pop)
#include <comutil.h>

struct ModInfo
{
    HANDLE              m_modHandle;
    PVOID               m_baseAddr;
    DWORD               m_size;
    DWORD               m_timeDateStamp;
    DWORD               m_checkSum;
    _bstr_t             m_name;
    unsigned __int64    m_verion;

    DWORD               m_numSyms;
    SYM_TYPE            m_symType;
    PVOID               m_entryPoint;   //for exe : main/winMain  for dll : DllMain
    _bstr_t             m_entryName;
    _bstr_t             m_pdbFile;

    ModInfo();
};

struct FuncInfo
{
    _bstr_t         m_name;
    _bstr_t         m_unDecorateName;
    PVOID           m_addr;
    DWORD64         m_asmOffset;
    FuncInfo();
};

struct AddressInfo
{
    PVOID               m_addr;
    _bstr_t             m_fileName;
    DWORD               m_lineNumber;
    ModInfo             m_mod;
    FuncInfo            m_func;
    AddressInfo();
};

struct CallStack
{
    _bstr_t             m_funcName;
    _bstr_t             m_srcFileName;
    DWORD               m_lineNumber;
    PVOID               m_funcAddr;
    DWORD64             m_asmOffset;
    CallStack();
};

struct CallStackS
{
    DWORD                       m_processId;
    DWORD                       m_threadId;
    stlVector<CallStack>        m_stacks;   //for general purpose,can:  #define stlVector _STD vector

    CallStackS();
};

//C++ version of SYMBOL_INFO
typedef struct _STL_SYMBOL_INFO 
{   //should improve to module path/name and filter repeated info
    ULONG       TypeIndex;        // Type Index of symbol
    ULONG       Index;
    ULONG       Size;
    ULONG64     ModBase;          // Base Address of module containing this symbol
    ULONG       Flags;
    ULONG64     Value;            // Value of symbol, ValuePresent should be 1
    ULONG64     Address;          // Address of symbol including base address of module
    ULONG       Register;         // register holding value or pointer to value
    ULONG       Scope;            // scope of the symbol
    ULONG       Tag;              // pdb classification
    _bstr_t     name;             // needn't stlStringW , because symbol Name MUST be English

    DWORD64     asmOffset;

    _STL_SYMBOL_INFO& operator=(SYMBOL_INFO const& rsi);
    _STL_SYMBOL_INFO& operator=(SYMBOL_INFOW const& rsi);
} STD_SYMBOL_INFO, *LPSTD_SYMBOL_INFO;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ExceptionItem
{
    DWORD       m_expCode;
    stlChar*    m_expText;
};
extern ExceptionItem const  exps[20];

#endif   //MSVC

#endif // SYMINFODEF_H__

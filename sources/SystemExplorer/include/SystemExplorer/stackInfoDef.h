#ifndef SYMINFODEF_H__
#define SYMINFODEF_H__
/************************************************************************************
author  :	Shen Tony,2009-2017
verified: 	VS2008	
purpose :	data structure definde for StackExplorer
*************************************************************************************/
#include <MiniMPL\platformEnv.h>

#ifdef MSVC
#include <MiniMPL/productSpecialDef.h>
#include <Windows.h>
#include <imagehlp.h>

struct ModInfo
{
    HANDLE              m_modHandle;
    PVOID               m_baseAddr;
    DWORD               m_size;
    DWORD               m_timeDateStamp;
    DWORD               m_checkSum;
    stlString           m_name;
    unsigned __int64    m_verion;

    DWORD               m_numSyms;
    SYM_TYPE            m_symType;
    PVOID               m_entryPoint;   //for exe : main/winMain  for dll : DllMain
    stlString           m_entryName;
    stlString           m_pdbFile;

    ModInfo();
};

struct FuncInfo
{
    stlString       m_name;
    stlString       m_unDecorateName;
    PVOID           m_addr;
    DWORD           m_AsmOffset;
    FuncInfo();
};

struct AddressInfo
{
    PVOID               m_addr;
    stlString           m_fileName;
    DWORD               m_lineNumber;
    ModInfo             m_mod;
    FuncInfo            m_func;
    AddressInfo();
};

struct CallStack
{
    stlString           m_funcName;
    stlString           m_srcFileName;
    int                 m_lineNumber;
    PVOID               m_funcAddr;
    DWORD               m_AsmOffset;
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
typedef struct _MOD_SYMBOL_INFO 
{   //should improve to module path/name and filter repeated info
    ULONG       TypeIndex;        // Type Index of symbol
    ULONG       Index;
    ULONG       Size;
    ULONG64     ModBase;          // Base Address of module comtaining this symbol
    ULONG       Flags;
    ULONG64     Value;            // Value of symbol, ValuePresent should be 1
    ULONG64     Address;          // Address of symbol including base address of module
    ULONG       Register;         // register holding value or pointer to value
    ULONG       Scope;            // scope of the symbol
    ULONG       Tag;              // pdb classification
    stlStringA  name;             // needn't stlStringW , because symbol Name MUST be English

    _MOD_SYMBOL_INFO& operator=(SYMBOL_INFO const& rsi);
} MOD_SYMBOL_INFO, *LPMOD_SYMBOL_INFO;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ExceptionItem
{
    DWORD       m_expCode;
    stlChar*    m_expText;
};
extern ExceptionItem const  exps[20];

#endif   //MSVC

#endif // SYMINFODEF_H__

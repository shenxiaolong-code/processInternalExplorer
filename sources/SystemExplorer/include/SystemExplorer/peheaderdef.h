#ifndef __PEHEADERDEF_H__
#define __PEHEADERDEF_H__
/***********************************************************************************************************************
* Description         :
* Author              : Shen.Xiaolong (Shen Tony) (2009-2018)
* Mail                : xlshen2002@hotmail.com,  xlshen@126.com
* verified platform   : VS2008 (create/refactor on 2018-01-12 15:14:06.69)
* copyright:          : free to use / modify / sale in free and commercial software.
*                       Unique limit: MUST keep those copyright comments in all copies and in supporting documentation.
***********************************************************************************************************************/
#include <MiniMPL/libConfig.h>
#include <Windows.h>
#include <string>

//////////////////////////////////////////////////////////////////////////
typedef struct _PEHeader
{
    _PEHeader()              {   ZeroMemory(this,sizeof(*this));     }
    virtual ~_PEHeader()     {   /* prevent from leaking resource */ }

    PIMAGE_DOS_HEADER           pDosHeader;
    PIMAGE_NT_HEADERS           pNtHeader;
    PIMAGE_FILE_HEADER          pFileHeader;
    PIMAGE_NT_HEADERS32         pNtHeader32;
    PIMAGE_NT_HEADERS64         pNtHeader64;
    PIMAGE_FILE_HEADER          pImageHeader;
    PIMAGE_SECTION_HEADER       pSectonHeader;
    PIMAGE_DATA_DIRECTORY       pDirectory;
} PEHeader,*LPPEHeader;

typedef struct _FuncThunk
{
    _FuncThunk()            {   ZeroMemory(this,offsetof(_FuncThunk,m_name)); }
    virtual ~_FuncThunk()   {   /* prevent from leaking resource */ }

    unsigned        m_hint;             //func ID
    DWORD           m_addr;             //func addr,void*
    DWORD           m_instructionAddr;  //the address to story func addr. void**

    std::string     m_name;
    std::string     m_moduleName;
} FuncThunk,*LPFuncThunk;

#endif // __PEHEADERDEF_H__

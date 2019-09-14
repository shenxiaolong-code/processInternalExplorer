#include <SystemExplorer/PEFileExplorer.h>
#include <MiniMPL/macro_assert.h>
#include <SystemExplorer/peheaderhelper.h>

//////////////////////////////////////////////////////////////////////////
//RVA : Relative Virtual Address
//(Virtual address 0x10464)-(base address 0x10000) = RVA 0x00464
static inline PBYTE RvaAdjust(PIMAGE_DOS_HEADER pDosHeader, DWORD raddr)
{
    if (raddr != NULL) 
    {
        return ((PBYTE)pDosHeader) + raddr;
    }
    return NULL;
}

//////////////////////////////////////////////////////////////////////////
void processExportedModule(PIMAGE_DOS_HEADER pDosHeader,IMAGE_EXPORT_DIRECTORY& rExportDir,std::vector<FuncThunk>& fts)
{
    FuncThunk ft;
    ft.m_moduleName = (char*)rExportDir.Name;

    PDWORD pAddressOfFunctions  = (PDWORD)RvaAdjust(pDosHeader, rExportDir.AddressOfFunctions);
    PDWORD pAddressOfNames      = (PDWORD)RvaAdjust(pDosHeader, rExportDir.AddressOfNames);
    PWORD AddressOfNameOrdinals = (PWORD )RvaAdjust(pDosHeader, rExportDir.AddressOfNameOrdinals);

    for (DWORD nFunc = 0; nFunc < rExportDir.NumberOfFunctions; nFunc++) 
    {
        PDWORD pFuncAddr = (pAddressOfFunctions != NULL)? (PDWORD)RvaAdjust(pDosHeader, pAddressOfFunctions[nFunc]) : NULL;
        ft.m_addr = (DWORD)pFuncAddr;
        ft.m_hint = nFunc;
        for (DWORD nName = 0; nName < rExportDir.NumberOfNames; nName++) 
        {
            if (AddressOfNameOrdinals[nName] == nFunc) 
            {   
                PCHAR pszName = (pAddressOfNames != NULL) ? (PCHAR)RvaAdjust(pDosHeader, pAddressOfNames[nName]) : NULL;
                ft.m_name = pszName;
                TRACE_HERE_INFO((TXTA("0X[%X]%s"),pFuncAddr,pszName));
                break;
            }
        }
        fts.push_back(ft);

        ULONG nOrdinal = rExportDir.Base + nFunc;
    }
}

void processImportedModule(PIMAGE_DOS_HEADER pDosHeader,IMAGE_IMPORT_DESCRIPTOR& rImportModules,std::vector<FuncThunk>& fts)
{  
    FuncThunk ft;

    PBYTE moduleBase = (PBYTE)pDosHeader;
    PIMAGE_IMPORT_DESCRIPTOR pImportModules=&rImportModules;
    while(pImportModules->Characteristics != 0)
    {
        //see structure : https://msdn.microsoft.com/en-us/library/ms809762.aspx        
        ft.m_moduleName = (const char*)(pImportModules->Name + moduleBase); //imported DLL name

        //pImportedAddressTable is 1:1 map to pHintNameTable
        //pImportedAddressTable is overwritten by PE loader
        PIMAGE_THUNK_DATA pImportedAddressTable = (PIMAGE_THUNK_DATA)(pImportModules->FirstThunk + moduleBase);
        //pHintNameTable is original data
        PIMAGE_THUNK_DATA pHintNameTable        = (PIMAGE_THUNK_DATA)(pImportModules->OriginalFirstThunk + moduleBase);

        int index = 0;
        while(pHintNameTable->u1.AddressOfData != 0)
        {
            PIMAGE_THUNK_DATA pImportedFunctionEntry = pImportedAddressTable + index;

            //test whether is IMAGE_IMPORT_BY_NAME structure or DWORD value
            if (IMAGE_SNAP_BY_ORDINAL(pImportedFunctionEntry->u1.Ordinal))
            {
                DWORD nExportOrdinalValue  = (DWORD)IMAGE_ORDINAL(pImportedFunctionEntry->u1.Ordinal);
                continue;
            }

            //else interpreted to PIMAGE_IMPORT_BY_NAME structure
            PIMAGE_IMPORT_BY_NAME pHintName = (PIMAGE_IMPORT_BY_NAME)(moduleBase+pHintNameTable->u1.AddressOfData);
            ft.m_name               = (char*)pHintName->Name;
            ft.m_hint               = pHintName->Hint;
            ft.m_addr               = pImportedFunctionEntry->u1.Function;
            ft.m_instructionAddr    = (DWORD)&(pImportedFunctionEntry->u1.Function);
            fts.push_back(ft);

            TRACE_HERE_INFO((TXTA("ModuleName=%s,FuncName=%s,funcId=%d\n"),
                ft.m_moduleName,ft.m_name,ft.m_hint));

            index++;
            pHintNameTable++;
        }

        pImportModules++;
    }
}

//////////////////////////////////////////////////////////////////////////
PEFileExplorer::PEFileExplorer()
{

}

PEFileExplorer::~PEFileExplorer()
{

}

bool PEFileExplorer::getPEHeader( PEHeader& rPEHeader,HMODULE hPeMod/*=GetModuleHandle()*/ )
{
    ZeroMemory(&rPEHeader,sizeof(rPEHeader));
    CHECK_TRUE_ELSE_RETURN_VAL(0 !=hPeMod , false);

    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hPeMod;
    CHECK_TRUE_ELSE_RETURN_VAL(pDosHeader->e_magic == IMAGE_DOS_SIGNATURE && (DWORD)pDosHeader->e_lfanew >= sizeof(*pDosHeader), false);
    rPEHeader.pDosHeader = pDosHeader;

    PBYTE   pAddrBase = (PBYTE)pDosHeader;
    PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((PBYTE)pDosHeader +pDosHeader->e_lfanew);
    CHECK_TRUE_ELSE_RETURN_VAL(pNtHeader->Signature == IMAGE_NT_SIGNATURE, false);
    CHECK_TRUE_ELSE_RETURN_VAL( 
        IMAGE_NT_OPTIONAL_HDR32_MAGIC == pNtHeader->OptionalHeader.Magic ||    //32 bit module
        IMAGE_NT_OPTIONAL_HDR64_MAGIC == pNtHeader->OptionalHeader.Magic       //64 bit module 
        ,false);

    rPEHeader.pNtHeader     = pNtHeader;
    rPEHeader.pFileHeader   = &pNtHeader->FileHeader;
    //only difference between IMAGE_OPTIONAL_HEADER32 and IMAGE_OPTIONAL_HEADER64
    //IMAGE_OPTIONAL_HEADER32 has member BaseOfData, but IMAGE_OPTIONAL_HEADER64 hasn't
    rPEHeader.pNtHeader32 = (IMAGE_NT_OPTIONAL_HDR32_MAGIC==pNtHeader->OptionalHeader.Magic) ? (PIMAGE_NT_HEADERS32)pNtHeader : NULL;
    rPEHeader.pNtHeader64 = (IMAGE_NT_OPTIONAL_HDR64_MAGIC==pNtHeader->OptionalHeader.Magic) ? (PIMAGE_NT_HEADERS64)pNtHeader : NULL;

    rPEHeader.pImageHeader = &pNtHeader->FileHeader;
    rPEHeader.pSectonHeader = IMAGE_FIRST_SECTION(pNtHeader);

    CHECK_TRUE_ELSE_RETURN_VAL(pNtHeader->FileHeader.SizeOfOptionalHeader != 0,false);
    rPEHeader.pDirectory = pNtHeader->OptionalHeader.DataDirectory;
    return true;
}

IMAGE_COR20_HEADER* PEFileExplorer::getCLR20(HMODULE hPeMod)
{
    PEHeaderHelper rHeader;
    getPEHeader(rHeader,hPeMod ? hPeMod:GetModuleHandle(NULL));
    //check ERROR_BAD_EXE_FORMAT
    CHECK_TRUE_ELSE_RETURN_VAL(rHeader.pDirectory,NULL);

    return rHeader.getDirectory<IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR>();
}

bool PEFileExplorer::getImportedAPIThunk(DWORD pExportedApi,FuncThunk& rFuncThunk)
{
    PEHeaderHelper rHeader;
    getPEHeader(rHeader);
    //check ERROR_BAD_EXE_FORMAT
    CHECK_TRUE_ELSE_RETURN_VAL(rHeader.pDirectory,false);
    PIMAGE_IMPORT_DESCRIPTOR pImportModules= rHeader.getDirectory<IMAGE_DIRECTORY_ENTRY_IMPORT>();
    CHECK_TRUE_ELSE_RETURN_VAL(pImportModules,false);

    PBYTE moduleBase = rHeader.base();
    while(pImportModules->Characteristics != 0)
    {
        //see structure : https://msdn.microsoft.com/en-us/library/ms809762.aspx
        char const* pImportedDllName = (const char*)(pImportModules->Name + moduleBase);

        //pImportedAddressTable is 1:1 map to pHintNameTable
        //pImportedAddressTable is overwritten by PE loader
        PIMAGE_THUNK_DATA pImportedAddressTable = (PIMAGE_THUNK_DATA)(pImportModules->FirstThunk + moduleBase);
        //pHintNameTable is original data
        PIMAGE_THUNK_DATA pHintNameTable        = (PIMAGE_THUNK_DATA)(pImportModules->OriginalFirstThunk + moduleBase);

        int index = 0;
        while(pHintNameTable->u1.AddressOfData != 0)
        {
            PIMAGE_THUNK_DATA pImportedFunctionEntry = pImportedAddressTable + index;

            //test whether is IMAGE_IMPORT_BY_NAME structure or DWORD value
            if (IMAGE_SNAP_BY_ORDINAL(pImportedFunctionEntry->u1.Ordinal))
            {
                DWORD nExportOrdinalValue  = (DWORD)IMAGE_ORDINAL(pImportedFunctionEntry->u1.Ordinal);
                continue;
            }

            //else interpreted to PIMAGE_IMPORT_BY_NAME structure
            PIMAGE_IMPORT_BY_NAME pHintName = (PIMAGE_IMPORT_BY_NAME)(moduleBase+pHintNameTable->u1.AddressOfData);
            char* pFuncName = (char*)pHintName->Name;
            WORD iFuncId    = pHintName->Hint;

            if (pExportedApi== pImportedFunctionEntry->u1.Function)
            {
                rFuncThunk.m_hint               = iFuncId;
                rFuncThunk.m_addr               = pImportedFunctionEntry->u1.Function;
                rFuncThunk.m_instructionAddr    = (DWORD)&(pImportedFunctionEntry->u1.Function);
                rFuncThunk.m_name               = pFuncName;
                rFuncThunk.m_moduleName         = pImportedDllName;
                TRACE_HERE_INFO((TXTA("ModuleName=%s,FuncName=%s,funcId=%d\n"),pImportedDllName,pFuncName,iFuncId));
                return true;
            }

            index++;
            pHintNameTable++;
        }

        pImportModules++;
    }

    return false;
}

std::vector<FuncThunk>  PEFileExplorer::enumFunc(EFType et, HMODULE hPeMod )
{
    std::vector<FuncThunk> fts;
    PEHeaderHelper rHeader;
    getPEHeader(rHeader,hPeMod ? hPeMod:GetModuleHandle(NULL));
    //check ERROR_BAD_EXE_FORMAT
    CHECK_TRUE_ELSE_RETURN_VAL(rHeader.pDirectory,fts);

    if (F_export&et)
    {
        //exported function
        PIMAGE_EXPORT_DIRECTORY pExportDir= rHeader.getDirectory<IMAGE_DIRECTORY_ENTRY_EXPORT>();
        if (pExportDir)
        {
            processExportedModule(rHeader,*pExportDir,fts);
        }
    }
    
    if (F_import&et)
    {
        //imported function
        PIMAGE_IMPORT_DESCRIPTOR pImportedFuncTable= rHeader.getDirectory<IMAGE_DIRECTORY_ENTRY_IMPORT>();
        if (pImportedFuncTable)
        {
            processImportedModule(rHeader,*pImportedFuncTable,fts);
        }
    }     

    return fts;
}

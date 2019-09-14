#include <SystemExplorer/peheaderhelper.h>
#include "MiniMPL/macro_assert.h"

PBYTE PEHeaderHelper::base()
{
    return (PBYTE)pDosHeader;
}

PEHeaderHelper::operator HMODULE()
{
    return (HMODULE)pDosHeader;
}

PEHeaderHelper::operator PIMAGE_DOS_HEADER()
{
    return pDosHeader;
}

void* PEHeaderHelper::getDirectory(unsigned idx)
{
    CHECK_TRUE_ELSE_RETURN_VAL(idx<IMAGE_NUMBEROF_DIRECTORY_ENTRIES,NULL);
    if (0==pDirectory[idx].VirtualAddress || 0==pDirectory[idx].Size)
    {
        return NULL;
    }
    return  base()+pDirectory[idx].VirtualAddress;
}

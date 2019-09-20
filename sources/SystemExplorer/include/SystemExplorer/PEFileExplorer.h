#ifndef __PEFILEEXPLORER_H__
#define __PEFILEEXPLORER_H__
/***********************************************************************************************************************
* Description         : windows PE file parser
* Author              : Shen.Xiaolong (2009-2016)
* Mail                : xlshen2002@hotmail.com,  xlshen@126.com
* verified platform   : VS2008 (2016-12-07 18:28:02.80)
* copyright:          : free to use / modify / sale in free and commercial software.
*                       Unique limit: MUST keep those copyright comments in all copies and in supporting documentation.
*                       latest Version of The Code Project Open License (CPOL : http://www.codeproject.com/)
***********************************************************************************************************************/
#include <MiniMPL/libConfig.h>
#include <SystemExplorer/peheaderdef.h>
#include <vector>

class PEFileExplorer
{
public:
    PEFileExplorer();
    virtual ~PEFileExplorer();

    bool getPEHeader(PEHeader& rPEHeader,HMODULE hPeMod=GetModuleHandle(NULL));
    IMAGE_COR20_HEADER* getCLR20(HMODULE hPeMod = NULL);

    bool getImportedAPIThunk(DWORD pExportedApi,FuncThunk& rFuncThunk);

    enum EFType{F_export=1,F_import=2,F_All=3};
    std::vector<FuncThunk> enumFunc(EFType et,HMODULE hPeMod = NULL); //enumerate Function in specified module
};

#endif // __PEFILEEXPLORER_H__

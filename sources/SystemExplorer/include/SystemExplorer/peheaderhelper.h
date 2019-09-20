#ifndef __PEHEADERHELPER_H__
#define __PEHEADERHELPER_H__
/***********************************************************************************************************************
* Description         : wrapper for window PE file parser
* Author              : Shen.Xiaolong (2009-2018)
* Mail                : xlshen2002@hotmail.com,  xlshen@126.com
* verified platform   : VS2008 (create/refactor on 2018-01-12 15:24:25.89)
* copyright:          : free to use / modify / sale in free and commercial software.
*                       Unique limit: MUST keep those copyright comments in all copies and in supporting documentation.
***********************************************************************************************************************/
#include <MiniMPL/libConfig.h>
#include <MiniMPL/kitType.hpp>
#include <SystemExplorer/peheaderdef.h>

#define PEHeaderList(_)                                             \
_(IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR, PIMAGE_COR20_HEADER)        \
_(IMAGE_DIRECTORY_ENTRY_EXPORT,         PIMAGE_EXPORT_DIRECTORY)    \
_(IMAGE_DIRECTORY_ENTRY_IMPORT,         PIMAGE_IMPORT_DESCRIPTOR)

template<unsigned>
struct image_directory_type : public MiniMPL::Type2Type<void*>{};
#define PE_DIR_BIND(I,T) template<> struct image_directory_type< I > : public MiniMPL::Type2Type< T >{};
PEHeaderList(PE_DIR_BIND);

struct PEHeaderHelper : public PEHeader
{
    operator HMODULE();
    operator PIMAGE_DOS_HEADER(); 

    PBYTE base();
    void* getDirectory(unsigned idx);

    template<unsigned idx>
    typename image_directory_type<idx>::type getDirectory()
    {
        Static_Assert(idx<IMAGE_NUMBEROF_DIRECTORY_ENTRIES);
        typedef typename image_directory_type<idx>::type TEntry;
        return (TEntry)getDirectory(idx);
    }

};


#endif // __PEHEADERHELPER_H__

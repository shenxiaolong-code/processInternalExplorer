#ifndef __FUNCTIONHOOK_H__
#define __FUNCTIONHOOK_H__
/***********************************************************************************************************************
* Description         : hook any window platform/x86 function (include exported function and user function), and provide simply restore interface
* Author              : Shen.Xiaolong (2009-2019)
* Mail                : xlshen2002@hotmail.com,  xlshen@126.com
* verified platform   : vs2015
* copyright:          : free to use / modify / sale in free and commercial software.
*                       Unique limit: MUST keep those copyright comments in all copies and in supporting documentation.
***********************************************************************************************************************/
#include <MiniMPL/libConfig.h>
#include <SystemExplorer/PEFileExplorer.h>
#include <MiniMPL/macro_makeVar.h>

namespace Win_x86
{
	struct ExportedFuncThunk;
	DECLARE_PTR(ExportedFuncThunk);
	FARPROC getOldFunc(ExportedFuncThunkPtr pThunk);

	class CExportedFuncHook : public PEFileExplorer
	{
	public:
		typedef ExportedFuncThunkPtr		ThunkType;
		
		template<typename TF> ThunkType hookImportedAPI(TF pOldApi, TF pNewApi)
		{
			return hookImportedAPIImpl((DWORD)pOldApi, (DWORD)pNewApi);
		}

		bool restoreHook(ThunkType pThunk);
	protected:
		ThunkType hookImportedAPIImpl(DWORD pOldApi, DWORD pNewApi);
	};

	//////////////////////////////////////////////////////////////////////////
	struct UserFunctionThunk;
	DECLARE_PTR(UserFunctionThunk);
	FARPROC getOldFunc(UserFunctionThunkPtr pThunk);

	struct UserFuncHook
	{
		typedef UserFunctionThunkPtr		ThunkType;

		template<typename TF> ThunkType hookUserFunc(TF pOldFunc, TF pNewFunc)
		{  //advantage : ensure fNew has same signature with fOld. (e.g. call conversion)
			return hookUserFuncImpl(pOldFunc, pNewFunc);
		}

		bool restoreHook(ThunkType pThunk);

	protected:
		ThunkType hookUserFuncImpl(LPVOID pOldFunc, LPVOID pNewFunc);
		ThunkType tryHookFuncBody(ThunkType pThunk,LPVOID pOldFunc, LPVOID pNewFunc);
	};
}	//Win_x86

#endif // __FUNCTIONHOOK_H__

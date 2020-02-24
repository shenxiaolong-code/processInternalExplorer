#include <SystemExplorer/functionhook.h>
#include <MiniMPL/macro_assert.h>
#include <MiniMPL/stdwrapper.hpp>
#include <SystemExplorer/executablememoryreadwriter.h>

//////////////////////////////////////////////////////////////////////////
namespace Win_x86
{
	struct ExportedFuncThunk : public FuncThunk
	{
		ExportedFuncThunk()
		{
			ZeroMemory(((FuncThunk*)this) + 1, offsetof(ExportedFuncThunk, m_pNewFuncName) - sizeof(FuncThunk));
		}
		DWORD           m_pNewFuncAddr;

		std::string     m_pNewFuncName;
		std::string     m_pNewFuncModuleName;
	};

	FARPROC getOldFunc(ExportedFuncThunkPtr pThunk)
	{
		if (pThunk)
		{
			return (FARPROC)pThunk->m_addr;
		}
		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	//refer:
	//http://win32assembly.programminghorizon.com/pe-tut1.html
	//http://www.programlife.net/category/windev/pe-coff-format
	//https://msdn.microsoft.com/en-us/library/ms809762.aspx?f=255&MSPPError=-2147217396
	//////////////////////////////////////////////////////////////////////////
	CExportedFuncHook::ThunkType CExportedFuncHook::hookImportedAPIImpl(DWORD pOldApi, DWORD pNewApi)
	{
		CExportedFuncHook::ThunkType pThunk = _sharedPtrGenerater;
		ExportedFuncThunk& rFuncThunk = *pThunk;
		if (getImportedAPIThunk(pOldApi, rFuncThunk))
		{
			rFuncThunk.m_pNewFuncAddr = pNewApi;
			DWORD* pInstAddr = (DWORD*)rFuncThunk.m_instructionAddr;
			InstructionRWGuard iw(GetCurrentProcessId(), pInstAddr, sizeof(pNewApi));
			if (iw)
			{
				*pInstAddr = pNewApi;   //implement hook
				return pThunk;
			}
		}
		return nullptr;
	}

	bool CExportedFuncHook::restoreHook(CExportedFuncHook::ThunkType pThunk)
	{		
		if (pThunk)
		{
			ExportedFuncThunk& rThunk = *pThunk;
			DWORD* pInstr = (DWORD*)rThunk.m_instructionAddr;
			InstructionRWGuard iw(GetCurrentProcessId(), pInstr, sizeof(rThunk.m_instructionAddr));
			if (iw)
			{
				*pInstr = rThunk.m_addr;   //implement hook
			}
			return iw;
		}
		return false;		
	}
} //Win_x86

//////////////////////////////////////////////////////////////////////////


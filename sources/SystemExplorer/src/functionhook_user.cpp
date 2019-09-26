#include <SystemExplorer/functionhook.h>
#include <MiniMPL/macro_assert.h>
#include <MiniMPL/stdwrapper.hpp>
#include <SystemExplorer/executablememoryreadwriter.h>

//////////////////////////////////////////////////////////////////////////
namespace Win_x86
{
	//////////////////////////////////////////////////////////////////////////
	//refer:
	//http://win32assembly.programminghorizon.com/pe-tut1.html
	//http://www.programlife.net/category/windev/pe-coff-format
	//https://msdn.microsoft.com/en-us/library/ms809762.aspx?f=255&MSPPError=-2147217396

	//////////////////////////////////////////////////////////////////////////
#pragma pack(1)
	struct JmpInstruction_t
	{
		BYTE    nPatchType;
		DWORD   dwOffset;
	};
#pragma pack()
	Static_Assert(sizeof(JmpInstruction_t) == 5);

	struct UserFunctionThunk
	{
		UserFunctionThunk() { ZeroMemory(this, sizeof(*this)); }

		JmpInstruction_t    m_oldJmpInstruction;
		LPVOID              m_instructionAddr;
		DWORD               m_dwOldProtectValue;

		PVOID				m_originalOldFuncEntryE8;		//new allocated execute memory space to store hooked opcode

		char                m_pFuncName[64];
		char                m_pModuleName[MAX_PATH];
	};

	FARPROC getOldFunc(UserFunctionThunkPtr pThunk)
	{
		if (pThunk)
		{
			UserFunctionThunk& rThunk = *pThunk;
			if (rThunk.m_originalOldFuncEntryE8)
			{
				return FARPROC(DWORD(rThunk.m_originalOldFuncEntryE8));
			}
			if (rThunk.m_instructionAddr)
			{
				return FARPROC(DWORD(rThunk.m_instructionAddr) + rThunk.m_oldJmpInstruction.dwOffset + sizeof(JmpInstruction_t));
			}
		}
		return nullptr;
	}	
	//////////////////////////////////////////////////////////////////////////
	bool UserFuncHook::restoreHook(ThunkType pThunk)
	{
		if (pThunk)
		{
			UserFunctionThunk& rThunk = *pThunk;
			if (rThunk.m_originalOldFuncEntryE8)
			{
				VirtualFreeEx(GetCurrentProcess(), rThunk.m_originalOldFuncEntryE8, 15, MEM_RELEASE);
				rThunk.m_originalOldFuncEntryE8 = nullptr;
			}

			if (0 != rThunk.m_instructionAddr)
			{
				InstructionRW rw(GetCurrentProcessId(), rThunk.m_instructionAddr, sizeof(JmpInstruction_t));
				if (rw)
				{
					return rw.writeMemory(rThunk.m_instructionAddr, &rThunk.m_oldJmpInstruction, sizeof(rThunk.m_oldJmpInstruction));
				}
			}			
		}
		
		return false;
	}

	UserFuncHook::ThunkType UserFuncHook::hookUserFuncImpl(LPVOID pOldFunc, LPVOID pNewFunc)
	{
		UserFuncHook::ThunkType pThunk = _sharedPtrGenerater;
		UserFunctionThunk& rThunk = *pThunk;
		rThunk.m_instructionAddr = pOldFunc;
		InstructionRW rw(GetCurrentProcessId(), pOldFunc, sizeof(JmpInstruction_t));
		if (rw)
		{
			rw.readMemory(pOldFunc, &rThunk.m_oldJmpInstruction, sizeof(rThunk.m_oldJmpInstruction));
			if (rThunk.m_oldJmpInstruction.nPatchType == 0xE9)
			{   // short jump to relative address , e.g. 00A0424B E9 00 B5 00 00   jmp   oldFunc (0A0F750h)  
				// instead of jumping to absolute address (0xFF 25) , e.g. 768B6466 FF 25 80 0E 92 76  jmp  dword ptr ds:[76920E80h]
				
				//verified : pOldFunc_jmp + dwOffset   + 5	  = OldFunc   //E9 xx xx xx xx is 5 bytes : E9 dwOffset			
				//pOldFunc_jmp : 00A0424B + 0x0000B500 + 5	  = OldFunc	: 0x0A0F750
				//need	   : pOldFunc_jmp + new_dwOffset + 5  = pNewFunc_jmp
				//  or	     pOldFunc_jmp + new_dwOffset + 5  = NewFunc = pNewFunc_jmp + m_thunk.m_oldJmpInstruction.dwOffset + 5
				// => new_dwOffset = pNewFunc_jmp -(pOldFunc_jmp + 5)
				JmpInstruction_t pNewJmpInstruction = { 0xE9,(DWORD)pNewFunc - ((DWORD)pOldFunc + sizeof(JmpInstruction_t)) };
				if (rw.writeMemory(pOldFunc, &pNewJmpInstruction, sizeof(pNewJmpInstruction)))
				{
					return pThunk;
				}
			}
			else
			{
				return tryHookFuncBody(pThunk, pOldFunc, pNewFunc);
			}
		}
		return nullptr;
	}

	UserFuncHook::ThunkType UserFuncHook::tryHookFuncBody(ThunkType pThunk, LPVOID pOldFunc, LPVOID pNewFunc)
	{	//in old function , the first 5 bytes MUST be complete opcode. is is fine for most function
		if (pThunk)
		{
			//build new function entry for pOldFunc
			LPVOID pOldFuncEntry = VirtualAllocEx(GetCurrentProcess(), NULL, 3*sizeof(JmpInstruction_t), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			memcpy(pOldFuncEntry, &pThunk->m_oldJmpInstruction, sizeof(JmpInstruction_t));
			JmpInstruction_t pNewJmpInstruction = { 0xE9,DWORD(pOldFunc) - DWORD(pOldFuncEntry) - sizeof(JmpInstruction_t) };
			memcpy((unsigned char*)pOldFuncEntry + sizeof(JmpInstruction_t), &pNewJmpInstruction, sizeof(JmpInstruction_t));
			pThunk->m_originalOldFuncEntryE8 = pOldFuncEntry;

			//hook old function
			InstructionRW rw(GetCurrentProcessId(), pOldFunc, sizeof(JmpInstruction_t));
			JmpInstruction_t pHookJmpInstruction = { 0xE9,(DWORD)pNewFunc - ((DWORD)pOldFunc + sizeof(JmpInstruction_t)) };
			if (rw.writeMemory(pOldFunc, &pHookJmpInstruction, sizeof(pHookJmpInstruction)))
			{				
				return pThunk;
			}			
		}
		return nullptr;
	}

} //Win_x86

//////////////////////////////////////////////////////////////////////////


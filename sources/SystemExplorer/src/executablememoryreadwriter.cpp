#include <SystemExplorer/executablememoryreadwriter.h>

namespace Win_x86
{
	InstructionRWGuard::InstructionRWGuard(DWORD dwProcessId, LPVOID lpAddress, SIZE_T isize, DWORD flNewProtect /*= PAGE_EXECUTE_READWRITE*/) 
        : m_status(false), m_lpAddress(lpAddress), m_size(isize), m_flNewProtect(flNewProtect), m_flOldProtect(m_flNewProtect)
	{
		m_hProcess  = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
        m_status    = INVALID_HANDLE_VALUE != m_hProcess;
		if (m_status && IsProcessorFeaturePresent(PF_NX_ENABLED))
		{//write new memory access protection flag and backup old protect value            
			m_status = TRUE == VirtualProtectEx(m_hProcess, m_lpAddress, m_size, m_flNewProtect, &m_flOldProtect);
		}
	}

	InstructionRWGuard::operator bool()
	{
		return m_status;
	}

	InstructionRWGuard::~InstructionRWGuard()
	{
		if (INVALID_HANDLE_VALUE != m_hProcess)
		{
/*
            #define PAGE_EXECUTE            0x10    
            #define PAGE_EXECUTE_READ       0x20    
            #define PAGE_EXECUTE_READWRITE  0x40    
            #define PAGE_EXECUTE_WRITECOPY  0x80
*/
            if ((m_flOldProtect != m_flNewProtect) && (m_flOldProtect & 0xF0) && IsProcessorFeaturePresent(PF_NX_ENABLED))
			{//write back old protect flag
				VirtualProtectEx(m_hProcess, m_lpAddress, m_size, m_flOldProtect, &m_flOldProtect);
			}
			CloseHandle(m_hProcess);
		}
	}

	InstructionRW::InstructionRW(DWORD dwProcessId, LPVOID lpAddress, SIZE_T isize) 
		: InstructionRWGuard(dwProcessId, lpAddress, isize, PAGE_EXECUTE_READWRITE)
	{

	}

	bool InstructionRW::readMemory(LPCVOID lpSrcAddr, LPVOID lpBuffer, SIZE_T nBufSize, SIZE_T *lpNumberOfBytesRead /*= NULL*/)
	{
		if (m_status && lpSrcAddr >= m_lpAddress && ((char*)lpSrcAddr + nBufSize) <= ((char*)m_lpAddress + m_size))
		{
			return 0 != ReadProcessMemory(m_hProcess, lpSrcAddr, lpBuffer, nBufSize, lpNumberOfBytesRead);
		}
		return false;
	}

	bool InstructionRW::writeMemory(LPVOID lpSrcAddr, LPVOID lpBuffer, SIZE_T nBufSize, SIZE_T *lpNumberOfBytesRead /*= NULL*/)
	{
		if (m_status && lpSrcAddr >= m_lpAddress && ((char*)lpSrcAddr + nBufSize) <= ((char*)m_lpAddress + m_size))
		{
			if (0 != WriteProcessMemory(m_hProcess, lpSrcAddr, lpBuffer, nBufSize, lpNumberOfBytesRead))
			{
				FlushInstructionCache(m_hProcess, lpBuffer, nBufSize);
				return true;
			}
		}
		return false;
	}

}
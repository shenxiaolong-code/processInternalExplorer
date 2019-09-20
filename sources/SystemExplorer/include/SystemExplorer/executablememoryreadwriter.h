#ifndef __EXECUTABLEMEMORYREADWRITER_H__
#define __EXECUTABLEMEMORYREADWRITER_H__
/***********************************************************************************************************************
* Description         : simply read/write execute memory and restore its protect mask automatically
* Author              : Shen.Xiaolong (at Thu 09/19/2019) , xlshen@126.com  xlshen2002@hotmail.com
* Copyright           : free to use / modify / sale in free and commercial software with those head comments.
***********************************************************************************************************************/
#include <MiniMPL/libConfig.h>
#include <windows.h>

namespace Win_x86
{
	struct InstructionRWGuard
	{
	protected:
		bool		m_status;
		HANDLE		m_hProcess;
		DWORD		m_flNewProtect;
		DWORD		m_flOldProtect;
		LPVOID		m_lpAddress;
		SIZE_T		m_size;

	public:
		InstructionRWGuard(DWORD dwProcessId, LPVOID  lpAddress, SIZE_T  isize, DWORD flNewProtect = PAGE_READWRITE);
		~InstructionRWGuard();

		operator bool();
	};

	struct InstructionRW : public InstructionRWGuard
	{
		InstructionRW(DWORD dwProcessId, LPVOID  lpAddress, SIZE_T  isize);
		bool readMemory(LPCVOID lpSrcAddr, LPVOID  lpBuffer, SIZE_T  nBufSize, SIZE_T  *lpNumberOfBytesRead = NULL);
		bool writeMemory(LPVOID lpSrcAddr, LPVOID  lpBuffer, SIZE_T  nBufSize, SIZE_T  *lpNumberOfBytesRead = NULL);
	};
}
#endif // __EXECUTABLEMEMORYREADWRITER_H__

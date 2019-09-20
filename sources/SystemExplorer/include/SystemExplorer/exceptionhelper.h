#ifndef __EXCEPTIONHELPER_H__
#define __EXCEPTIONHELPER_H__
/***********************************************************************************************************************
* Description         : provide one simply way to enable/disable exception wrapper, and dump file generator.
* Author              : Shen.Xiaolong (2009-2019)
* Mail                : xlshen2002@hotmail.com,  xlshen@126.com
* verified platform   : vs2015
* copyright:          : free to use / modify / sale in free and commercial software.
*                       Unique limit: MUST keep those copyright comments in all copies and in supporting documentation.
***********************************************************************************************************************/
#include <MiniMPL/libConfig.h>
#include <windows.h>
#include <memory>
#include <MiniMPL/macro_makeVar.h>

DECLARE_PTRS_STRUCT(ExceptionData);

class ExceptionHelper
{
public:
	ExceptionDataPtr	enableExceptionWrapper(bool bIncludeProcess,bool bIncludeThread=false);
	void				disableExceptionWrapper(ExceptionDataPtr& pExpRecordBackup);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class DumpGenerater
{
public:
	DumpGenerater(DWORD processID, DWORD threadID);
	~DumpGenerater();

	void WriteDump(char const* pFileFullPath, PEXCEPTION_POINTERS pExceptionInfo,bool bFullDump=true);

protected:
	bool getContext(CONTEXT& rThreadContext);
	bool prepareCapture(bool bPrepare);

private:
	HANDLE		m_hProcess;
	DWORD		m_processId;
	DWORD		m_threadId;
};

#endif // __EXCEPTIONHELPER_H__

#include <SystemExplorer/exceptionhelper.h>
#include <MiniMPL/macro_traceLog.h>
#include <win32/WinApiPack.h>
#include <Aclapi.h>
#include <TlHelp32.h>
#include <signal.h>
#include <new.h>
#include <SystemExplorer/StackExplorer.h>
#include "MiniMPL/fromToString.hpp"
#include "MiniMPL/stdwrapper.hpp"
#include <winnt.h>

#pragma comment(lib,"Dbghelp.lib")  //for MiniDumpWriteDump

//////////////////////////////////////////////////////////////////////////
namespace ExceptionHandler
{
	template<class ... TAgrs>
	void UnifiedExceptionHandler(PEXCEPTION_POINTERS pExceptionPtrs, TAgrs ...args)
	{		
		StackExplorer se;
		PVOID pCaller = se.getCallerAddr(1);
		AddressInfo ai;
		se.resloveAddrInfo(pCaller, ai);

		std::wstringstream ss;
		ss << L"\r\ncaller : " << ai.m_func.m_name << L"\r\nparameter info :";
		int exp[] = { 0,(ss << args << L"," ,0)... , (ss << L"\r\n" ,0) };
		outputTxt(ss.str().c_str());

		CallStackS cs;
		se.resloveStacks(cs);

		StackInfoDisplayer sid;
		sid.print(ai);
		sid.print(cs);

		char modulename[MAX_PATH] = { 0 };
		GetModuleFileNameA(NULL, modulename, MAX_PATH);
		std::string dmpPath = modulename;
		dmpPath = dmpPath.substr(0, dmpPath.size()-4)+".dmp";

		DumpGenerater dg(GetCurrentProcessId(), GetCurrentThreadId());
		dg.WriteDump(dmpPath.c_str(), pExceptionPtrs, true);

		//MessageBoxA(NULL, dmpPath.c_str(), "UnifiedExceptionHandler", MB_OK);
		//MessageBoxW(NULL, ss.str().c_str(), L"UnifiedExceptionHandler", MB_OK);
	}

	namespace ProcessException
	{
		void __cdecl InvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved)
		{
			//outputTxtStreamA("expression:" << expression << ",function:" << function << "," << file << "(" << line << ")");
			UnifiedExceptionHandler(NULL,expression,function,file,line,pReserved);
		};

		LONG WINAPI UnhandledExceptionHandler(PEXCEPTION_POINTERS pExceptionPtrs)	
		{	UnifiedExceptionHandler(pExceptionPtrs);	return EXCEPTION_EXECUTE_HANDLER;	};
		void __cdecl TerminateHandler()												{	UnifiedExceptionHandler(NULL);							};
		void __cdecl PureVirtulCallHandler()										{	UnifiedExceptionHandler(NULL);							};
		int __cdecl NewHandler(size_t iSize)										{	UnifiedExceptionHandler(NULL,iSize);	return iSize;	};
		void SigAbortHandler(int iSigValue)											{	UnifiedExceptionHandler(NULL,iSigValue);				};
		void SigTerminateHandler(int iSigValue)										{	UnifiedExceptionHandler(NULL, iSigValue);				};
		void SigInterruptHandler(int iSigValue)										{	UnifiedExceptionHandler(NULL, iSigValue);				};
	}

	namespace ThreadException
	{
		void __cdecl TerminateHandler()												{ UnifiedExceptionHandler(NULL);							};
		void __cdecl UnexpectedHandler()											{ UnifiedExceptionHandler(NULL);							};

		void __cdecl SigFloatPointerErrorHandler(int iSigValue, int subcode)		{ UnifiedExceptionHandler(NULL, iSigValue, subcode);		};
		void SigIllgealInstructionHandler(int iSigValue)							{ UnifiedExceptionHandler(NULL, iSigValue);					};
		void SigStorageAccessErrorHandler(int iSigValue)							{ UnifiedExceptionHandler(NULL, iSigValue);					};
	}

	//*/
	static BOOL PreventHookSetUnhandledExceptionFilter()
	{ 
		//when enable exception handler, disable hook & hijack SetUnhandledExceptionFilter function by any other purpose.
		//http://blog.kalmbach-software.de/2013/05/23/improvedpreventsetunhandledexceptionfilter/
		//theory : return directly to invalid any jmp-hijack behavior , 76920E80h is real SetUnhandledExceptionFilter function code block
		/*
		768B6460 8B FF                mov         edi,edi
		768B6462 55                   push        ebp
		768B6463 8B EC                mov         ebp,esp
		768B6465 5D                   pop         ebp
		768B6466 FF 25 80 0E 92 76    jmp         dword ptr ds:[76920E80h]
		*/
		HMODULE hKernel32 = LoadLibrary(L"kernel32.dll");
		if (hKernel32 == NULL) return FALSE;
		void *pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
		if (pOrgEntry == NULL) return FALSE;

#ifdef _M_IX86
		// Code for x86:
		// 33 C0                xor         eax,eax  
		// C2 04 00             ret         4 
		unsigned char szExecute[] = { 0x33, 0xC0, 0xC2, 0x04, 0x00 };
#elif _M_X64
		// 33 C0                xor         eax,eax 
		// C3                   ret  
		unsigned char szExecute[] = { 0x33, 0xC0, 0xC3 };
#else
	#error "The following code only works for x86 and x64!"
#endif

		SIZE_T bytesWritten = 0;
		BOOL bRet = WriteProcessMemory(GetCurrentProcess(),
			pOrgEntry, szExecute, sizeof(szExecute), &bytesWritten);
		return bRet;
	}
	//*/
}

namespace ExceptionRecord
{
	struct ProcessExceptionRecord
	{
		LPTOP_LEVEL_EXCEPTION_FILTER	m_UnhandledExceptionHandler;
		_purecall_handler				m_PureVirtulCallHandler;
		_PNH							m_NewHandler;
		_invalid_parameter_handler		m_InvalidParameterHandler;
		_crt_signal_t					m_SigAbortHandler;
		_crt_signal_t					m_SigInterruptHandler;
		_crt_signal_t					m_SigTerminateHandler;

		ProcessExceptionRecord() { ZeroMemory(this, sizeof(*this)); };
	};

	struct ThreadExceptionRecord
	{
		terminate_handler	m_TerminateHandler;
		unexpected_handler	m_UnexpectedHandler;
		_crt_signal_t		m_SigFloatPointerErrorHandler;
		_crt_signal_t		m_SigIllgealInstructionHandler;
		_crt_signal_t		m_SigStorageAccessErrorHandler;

		UINT				m_threadId;
		ThreadExceptionRecord() { ZeroMemory(this, sizeof(*this)); m_threadId = GetCurrentThreadId(); };
	};

}

DECLARE_PTRS_STRUCT_NS(ExceptionRecord,	ProcessExceptionRecord);
DECLARE_PTRS_STRUCT_NS(ExceptionRecord, ThreadExceptionRecord);

struct ExceptionData
{
	ProcessExceptionRecordPtr	m_pProcessExpRecord;
	ThreadExceptionRecordPtr	m_pThreadExpRecord;
};

//////////////////////////////////////////////////////////////////////////
class ExceptionHelperImpl
{
public:
	bool	SetProcessExceptionHandlers(ProcessExceptionRecordPtr);
	bool	disableProcessExceptionHandlers(ProcessExceptionRecordPtr);

	bool	SetThreadExceptionHandlers(ThreadExceptionRecordPtr);
	bool	disableThreadExceptionHandlers(ThreadExceptionRecordPtr);
};

bool ExceptionHelperImpl::SetProcessExceptionHandlers(ProcessExceptionRecordPtr pOldHandlerBackup)
{//https://www.codeproject.com/Articles/207464/Exception-Handling-in-Visual-Cplusplus

	using namespace ExceptionHandler::ProcessException;	
	ExceptionRecord::ProcessExceptionRecord			oldHandler;
	oldHandler.m_UnhandledExceptionHandler	= SetUnhandledExceptionFilter(UnhandledExceptionHandler);
	oldHandler.m_PureVirtulCallHandler		= _set_purecall_handler(PureVirtulCallHandler);
	oldHandler.m_NewHandler					= _set_new_handler(NewHandler);
	oldHandler.m_InvalidParameterHandler	= _set_invalid_parameter_handler(InvalidParameterHandler);

	// Set up C++ signal handlers
	_set_abort_behavior(_CALL_REPORTFAULT, _CALL_REPORTFAULT);

	// Catch an abnormal program termination
	oldHandler.m_SigAbortHandler			= signal(SIGABRT, SigAbortHandler);

	// Catch illegal instruction handler
	oldHandler.m_SigInterruptHandler		= signal(SIGINT, SigInterruptHandler);

	// Catch a termination request
	oldHandler.m_SigTerminateHandler		= signal(SIGTERM, SigTerminateHandler);

	if (pOldHandlerBackup)
	{
		*pOldHandlerBackup = oldHandler;
	}
	return true;
}

bool ExceptionHelperImpl::SetThreadExceptionHandlers(ThreadExceptionRecordPtr pOldHandlerBackup)
{//https://www.codeproject.com/Articles/207464/Exception-Handling-in-Visual-Cplusplus

	using namespace ExceptionHandler::ThreadException;	
	ExceptionRecord::ThreadExceptionRecord	oldHandler;

	// http://msdn.microsoft.com/en-us/library/t6fk7h29.aspx
	oldHandler.m_TerminateHandler				= set_terminate(TerminateHandler);
	// http://msdn.microsoft.com/en-us/library/h46t5b69.aspx  
	oldHandler.m_UnexpectedHandler				= set_unexpected(UnexpectedHandler);

	// Catch a floating point error	
	oldHandler.m_SigFloatPointerErrorHandler	= signal(SIGFPE, (_crt_signal_t)SigFloatPointerErrorHandler);
	// Catch an illegal instruction
	oldHandler.m_SigIllgealInstructionHandler	= signal(SIGILL, SigIllgealInstructionHandler);
	// Catch illegal storage access errors
	oldHandler.m_SigStorageAccessErrorHandler	= signal(SIGSEGV, SigStorageAccessErrorHandler);
	
	if (pOldHandlerBackup)
	{
		*pOldHandlerBackup = oldHandler;
	}
	return true;
}

bool ExceptionHelperImpl::disableProcessExceptionHandlers(ProcessExceptionRecordPtr pProcessExpHandler)
{
	if (!pProcessExpHandler)
	{
		return false;
	}

	ExceptionRecord::ProcessExceptionRecord&	oldHandler = *pProcessExpHandler;
	//ExceptionHandler::PreventHookSetUnhandledExceptionFilter();
	SetUnhandledExceptionFilter(oldHandler.m_UnhandledExceptionHandler);
	_set_purecall_handler(oldHandler.m_PureVirtulCallHandler);
	_set_new_handler(oldHandler.m_NewHandler);
	_set_invalid_parameter_handler(oldHandler.m_InvalidParameterHandler);
	signal(SIGABRT, oldHandler.m_SigAbortHandler);
	signal(SIGINT, oldHandler.m_SigInterruptHandler);
	signal(SIGTERM, oldHandler.m_SigTerminateHandler);
	return true;
}

bool ExceptionHelperImpl::disableThreadExceptionHandlers(ThreadExceptionRecordPtr pThreadExpHandler)
{
	if (!pThreadExpHandler || pThreadExpHandler->m_threadId != GetCurrentThreadId())
	{
		return false;
	}
	ExceptionRecord::ThreadExceptionRecord&		oldHandler = *pThreadExpHandler;
	set_terminate(oldHandler.m_TerminateHandler);
	set_unexpected(oldHandler.m_UnexpectedHandler);
	signal(SIGFPE, (_crt_signal_t)oldHandler.m_SigFloatPointerErrorHandler);
	signal(SIGILL, oldHandler.m_SigIllgealInstructionHandler);
	signal(SIGSEGV, oldHandler.m_SigStorageAccessErrorHandler);
	return true;
}

//////////////////////////////////////////////////////////////////////////
ExceptionDataPtr ExceptionHelper::enableExceptionWrapper(bool bIncludeProcess, bool bIncludeThread/*=false*/)
{
	ExceptionDataPtr ptr = _sharedPtrGenerater;
	ExceptionHelperImpl	impl;
	if (bIncludeProcess)
	{
		ptr->m_pProcessExpRecord = _sharedPtrGenerater;
		impl.SetProcessExceptionHandlers(ptr->m_pProcessExpRecord);
	}
	if (bIncludeThread)
	{
		ptr->m_pThreadExpRecord = _sharedPtrGenerater;
		impl.SetThreadExceptionHandlers(ptr->m_pThreadExpRecord);
	}
	return ptr;
}

void ExceptionHelper::disableExceptionWrapper(ExceptionDataPtr& pExpRecordBackup)
{
	if (!pExpRecordBackup)
	{
		return;
	}

	ExceptionHelperImpl	impl;
	if (pExpRecordBackup->m_pProcessExpRecord && impl.disableProcessExceptionHandlers(pExpRecordBackup->m_pProcessExpRecord))
	{
		pExpRecordBackup->m_pProcessExpRecord.reset();
	}
	if (pExpRecordBackup->m_pThreadExpRecord && impl.disableThreadExceptionHandlers(pExpRecordBackup->m_pThreadExpRecord))
	{
		pExpRecordBackup->m_pThreadExpRecord.reset();
	}	
}

//////////////////////////////////////////////////////////////////////////
DumpGenerater::DumpGenerater(DWORD processID, DWORD threadID)
	: m_processId(processID)
	, m_threadId(threadID)
{
	DWORD  dwDesiredAccess = PROCESS_ALL_ACCESS;
	if (GetCurrentProcessId() != m_processId)
	{//https://docs.microsoft.com/en-us/windows/win32/api/minidumpapiset/nf-minidumpapiset-minidumpwritedump
		dwDesiredAccess = PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_DUP_HANDLE;

		//https://social.msdn.microsoft.com/Forums/windowsdesktop/en-US/2dfd711f-e81e-466f-a566-4605e78075f6/why-does-minidumpwritedump-fail?forum=windbg
		//8007012b: Only part of a ReadProcessMemory or WriteProcessMemory request was completed.
		//OpenProcess MUST has PROCESS_VM_READ privilege, else cause 0x8007012b error.
	}
	m_hProcess = OpenProcess(dwDesiredAccess, FALSE, m_processId);
	if (INVALID_HANDLE_VALUE==m_hProcess)
	{
		outputTxtV("OpenProcess failed");
	}
}

DumpGenerater::~DumpGenerater()
{
	if (INVALID_HANDLE_VALUE != m_hProcess)
	{
		CloseHandle(m_hProcess);
	}
}

bool DumpGenerater::getContext(CONTEXT& rThreadContext)
{//https://social.msdn.microsoft.com/Forums/en-US/805801ab-b89b-4f9e-91af-c9872a4adfd1/get-context-of-a-remote-process?forum=vcgeneral
	if (!SetSecurityInfo(m_hProcess, SE_KERNEL_OBJECT, THREAD_GET_CONTEXT, NULL, NULL, NULL, NULL))
	{
		outputTxtV("[warning] Elevate permissions fails. ");
	}

	//https://social.msdn.microsoft.com/Forums/windowsdesktop/en-US/2dfd711f-e81e-466f-a566-4605e78075f6/why-does-minidumpwritedump-fail?forum=windbg
	rThreadContext.ContextFlags = CONTEXT_FULL;	//else cause error 0x8007012B
	HANDLE hThread = OpenThread(THREAD_GET_CONTEXT/*THREAD_ALL_ACCESS*/, FALSE, m_threadId);
	if (!hThread)
	{
		outputTxtV("OpenThread -FAILED (LastError:%u)\n", GetLastError());
		return false;
	}
	bool bGetThreadContext = 0!=GetThreadContext(hThread, &rThreadContext);
	CloseHandle(hThread);
	if (!bGetThreadContext)
	{
		outputTxtV("GetThreadContext -FAILED (LastError:%u)\n", GetLastError());
	}
	return bGetThreadContext;
}

bool DumpGenerater::prepareCapture(bool bPrepare)
{
	//https://stackoverflow.com/questions/11010165/how-to-suspend-resume-a-process-in-windows
	if (GetCurrentProcessId() == m_processId)
	{		
		HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		if (INVALID_HANDLE_VALUE ==hThreadSnapshot)
		{
			outputTxtV("CreateToolhelp32Snapshot -FAILED (LastError:%u)\n", GetLastError());
			return false;
		}

		DWORD dResult = -1;
		THREADENTRY32 threadEntry = { 0 };
		threadEntry.dwSize = sizeof(threadEntry);
		if (!Thread32First(hThreadSnapshot, &threadEntry))
		{
			outputTxtV("Thread32First -FAILED (LastError:%u)\n", GetLastError());
			CloseHandle(hThreadSnapshot);
			return false;
		}		
		do
		{
			if (threadEntry.th32OwnerProcessID == m_processId)
			{
				if (threadEntry.th32ThreadID != m_threadId)
				{   //SuspendThread except myself
					HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME/*THREAD_ALL_ACCESS*/, FALSE, threadEntry.th32ThreadID);
					dResult = bPrepare ? SuspendThread(hThread) : ResumeThread(hThread);
					CloseHandle(hThread);
				}
			}
		} while (Thread32Next(hThreadSnapshot, &threadEntry));
		CloseHandle(hThreadSnapshot);
		return -1 != dResult;
	}
	else
	{
		return bPrepare ? 0!=DebugActiveProcess(m_processId) : 0 != DebugActiveProcessStop(m_processId);
	}
}

void DumpGenerater::WriteDump(char const* pFileFullPath, PEXCEPTION_POINTERS pExceptionInfo, bool bFullDump/*=true*/)
{
	if (INVALID_HANDLE_VALUE == m_hProcess)
	{
		outputTxtV("[warning] invalid process handle. ");
		return;
	}

	//check directory exist
	std::string str(pFileFullPath);
	DWORD dwAttr = GetFileAttributesA(str.substr(0,str.rfind('\\')).c_str());
	if (dwAttr == 0xffffffff || !(dwAttr & FILE_ATTRIBUTE_DIRECTORY))
	{
		outputTxtV("[warning] directory doesn't exist. ");
		return;
	}

	if (!prepareCapture(true))
	{
		outputTxtV("[warning] prepareCapture fails. ");
	}	

	PEXCEPTION_POINTERS		pExceptionRecord = NULL;
	if (pExceptionInfo)
	{
		pExceptionRecord = pExceptionInfo;
	}
	else if (GetCurrentProcessId() == m_processId)
	{		 
		 static CONTEXT	threadContext = { 0 };
		 if (!getContext(threadContext))
		 {	//if no thread CONTEXT , the dump file size will be 0.		
			 return;
		 }
		static EXCEPTION_RECORD	ExceptionRecord = { 0 };
		ExceptionRecord.ExceptionAddress = _ReturnAddress();

		static EXCEPTION_POINTERS pException = { 0 };
		pException.ContextRecord	= &threadContext;
		pException.ExceptionRecord	= &ExceptionRecord;

		pExceptionRecord = &pException;
	}	

	MINIDUMP_EXCEPTION_INFORMATION minidump_exception = { 0 };
	minidump_exception.ThreadId = m_threadId;
	minidump_exception.ExceptionPointers = pExceptionRecord;
	minidump_exception.ClientPointers = GetCurrentProcessId() != m_processId;
	
	HANDLE hFile = CreateFileA(pFileFullPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		outputTxtV("CreateFileA -FAILED (LastError:%u)\n", GetLastError());
		return;
	}

	SetLastError(0L);
	MINIDUMP_TYPE mt = MiniDumpNormal;	//mini dump
	if (bFullDump)
	{
		mt = MINIDUMP_TYPE(MiniDumpWithFullMemory | MiniDumpWithFullMemoryInfo
			| MiniDumpWithHandleData | MiniDumpWithUnloadedModules | MiniDumpWithThreadInfo);
	}
	if (!MiniDumpWriteDump(m_hProcess, m_processId, hFile, mt, &minidump_exception, NULL, NULL))
	{
		outputTxtV("MiniDumpWriteDump -FAILED (LastError:%u)\n", GetLastError());
	}
	FlushFileBuffers(hFile);
	CloseHandle(hFile);
	prepareCapture(false);
}
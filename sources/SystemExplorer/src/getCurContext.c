//internal use,only for StackExplorer.cpp

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The "ugly" assembler-implementation is needed for systems before XP
// If you have a new PSDK and you only compile for XP and later, then you can use 
// the "RtlCaptureContext"
// Currently there is no define which determines the PSDK-Version... 
// So we just use the compiler-version (and assumes that the PSDK is 
// the one which was installed by the VS-IDE)

// INFO: If you want, you can use the RtlCaptureContext if you only target XP and later...
//       But I currently use it in x64/IA64 environments...
//#if defined(_M_IX86) && (_WIN32_WINNT <= 0x0500) && (_MSC_VER < 1400)

#if defined(_M_IX86)
#ifdef CURRENT_THREAD_VIA_EXCEPTION
// The following is not a "good" implementation, 
// because the callstack is only valid in the "__except" block...
#define GET_CURRENT_CONTEXT(c, contextFlags)            \
do                                                      \
{                                                       \
    memset(&c, 0, sizeof(CONTEXT));                     \
    EXCEPTION_POINTERS *pExp = NULL;                    \
    __try {                                             \
        throw 0;                                        \
} __except( ( (pExp = GetExceptionInformation()) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_EXECUTE_HANDLER)) {} \
    if (pExp != NULL)                                   \
    memcpy(&c, pExp->ContextRecord, sizeof(CONTEXT));   \
    c.ContextFlags = contextFlags;                      \
} while(0);
#else
// The following should be enough for walking the callstack...
#define GET_CURRENT_CONTEXT(c, contextFlags)    \
do                                              \
{                                               \
    memset(&c, 0, sizeof(CONTEXT));             \
    c.ContextFlags = contextFlags;              \
    __asm    call x                             \
    __asm x: pop eax                            \
    __asm    mov c.Eip, eax                     \
    __asm    mov c.Ebp, ebp                     \
    __asm    mov c.Esp, esp                     \
} while(0);
#endif

#else

// The following is defined for x86 (XP and higher), x64 and IA64:
#define GET_CURRENT_CONTEXT(c, contextFlags)    \
do                                              \
{                                               \
    memset(&c, 0, sizeof(CONTEXT));             \
    c.ContextFlags = contextFlags;              \
    RtlCaptureContext(&c);                      \
} while(0);
#endif

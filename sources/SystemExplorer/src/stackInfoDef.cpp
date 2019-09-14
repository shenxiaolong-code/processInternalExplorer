#include <SystemExplorer/stackInfoDef.h>

#ifdef MSVC
#include <MiniMPL\macroDef.h>

ModInfo::ModInfo()
{
    m_modHandle     = INVALID_HANDLE_VALUE;
    m_baseAddr      = 0;
    m_size          = 0;
    m_timeDateStamp = 0;
    m_verion        = 0;    
    m_checkSum      = 0;
    m_numSyms       = 0;;
    m_symType       = SymNone;
    m_entryPoint= 0;
}

FuncInfo::FuncInfo()
{
    m_addr      = 0;
    m_AsmOffset = 0;
}

AddressInfo::AddressInfo()
{
    m_addr          = 0;
    m_lineNumber    = 0;
}

CallStack::CallStack()
{
    m_lineNumber    = 0;
    m_funcAddr      = 0;
    m_AsmOffset     = 0;
}

CallStackS::CallStackS()
{
    m_processId = 0;
    m_threadId  = 0;
}

_MOD_SYMBOL_INFO& _MOD_SYMBOL_INFO::operator=( SYMBOL_INFO const& rsi )
{
    TypeIndex=rsi.TypeIndex;        // Type Index of symbol
    Index=rsi.Index;
    Size=rsi.Size;
    ModBase=rsi.ModBase;            // Base Address of module comtaining this symbol
    Flags=rsi.Flags;
    Value=rsi.Value;                // Value of symbol, ValuePresent should be 1
    Address=rsi.Address;            // Address of symbol including base address of module
    Register=rsi.Register;          // register holding value or pointer to value
    Scope=rsi.Scope;                // scope of the symbol
    Tag=rsi.Tag;                    // pdb classification
    name=rsi.Name;                  // pdb classification
    return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ExceptionItem const  exps[] = 
{   //comes MSDN EXCEPTION_RECORD Structure : ms-help://MS.MSDNQTR.v90.en/debug/base/exception_record_str.htm
    { EXCEPTION_ACCESS_VIOLATION,       TXT("The thread tried to read from or write to a virtual address for which it does not have the appropriate access.") },
    { EXCEPTION_ARRAY_BOUNDS_EXCEEDED,  TXT("The thread tried to access an array element that is out of bounds and the underlying hardware supports bounds checking.") },
    { EXCEPTION_BREAKPOINT,             TXT("A breakpoint was encountered.") },
    { EXCEPTION_DATATYPE_MISALIGNMENT,  TXT("The thread tried to read or write data that is misaligned on hardware that does not provide alignment. For example, 16-bit values must be aligned on 2-byte boundaries; 32-bit values on 4-byte boundaries, and so on.") },
    { EXCEPTION_FLT_DENORMAL_OPERAND,   TXT("One of the operands in a floating-point operation is denormal. A denormal value is one that is too small to represent as a standard floating-point value.") },
    { EXCEPTION_FLT_DIVIDE_BY_ZERO,     TXT("The thread tried to divide a floating-point value by a floating-point divisor of zero.") },
    { EXCEPTION_FLT_INEXACT_RESULT,     TXT("The result of a floating-point operation cannot be represented exactly as a decimal fraction.") },
    { EXCEPTION_FLT_INVALID_OPERATION,  TXT("This exception represents any floating-point exception not included in this list.") },
    { EXCEPTION_FLT_OVERFLOW,           TXT("The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.") },
    { EXCEPTION_FLT_STACK_CHECK,        TXT("The stack overflowed or underflowed as the result of a floating-point operation.") },
    { EXCEPTION_FLT_UNDERFLOW,          TXT("The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type.") },
    { EXCEPTION_ILLEGAL_INSTRUCTION,    TXT("The thread tried to execute an invalid instruction.") },
    { EXCEPTION_IN_PAGE_ERROR,          TXT("The thread tried to access a page that was not present, and the system was unable to load the page. For example, this exception might occur if a network connection is lost while running a program over the network.") },
    { EXCEPTION_INT_DIVIDE_BY_ZERO,     TXT("The thread tried to divide an integer value by an integer divisor of zero.") },
    { EXCEPTION_INT_OVERFLOW,           TXT("The result of an integer operation caused a carry out of the most significant bit of the result.") },
    { EXCEPTION_INVALID_DISPOSITION,    TXT("An exception handler returned an invalid disposition to the exception dispatcher. Programmers using a high-level language such as C should never encounter this exception.") },
    { EXCEPTION_NONCONTINUABLE_EXCEPTION, TXT("The thread tried to continue execution after a noncontinuable exception occurred.") },
    { EXCEPTION_PRIV_INSTRUCTION,       TXT("The thread tried to execute an instruction whose operation is not allowed in the current machine mode.") },
    { EXCEPTION_SINGLE_STEP,            TXT("A trace trap or other single-instruction mechanism signaled that one instruction has been executed.") },
    { EXCEPTION_STACK_OVERFLOW,         TXT("The thread used up its stack.") },
} ;

#endif

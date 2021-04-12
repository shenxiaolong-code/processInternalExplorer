#ifndef __CALLBACK_CPP2C_H__
#define __CALLBACK_CPP2C_H__
/***********************************************************************************************************************
* Description         : in some callback , its callback function is C interface (__stdcall), but in C++ app, we is used to provide C++ callback (thiscall).
                        here is way to transfer a C++ callback(thiscall) to a C callback(__stdcall).
  limitation          : this package is for x86 platform , but it is easy to port this way into other platforms.                       
* Author              : Shen.Xiaolong (at Fri 04/02/2021) , xlshen@126.com  xlshen2002@hotmail.com
* Copyright           : free to use / modify / sale in free and commercial software with those head comments.
***********************************************************************************************************************/
#include <MiniMPL/platformEnv.h>
#include <MiniMPL/any_cast.hpp>
#include <SystemExplorer/executablememoryreadwriter.h>
#include <MiniMPL/macro_assert.h>

#pragma pack(push)
#pragma pack(1)

// http://www.hackcraft.net/cpp/windowsThunk/
// This-call calling conventions add this value to ecx, and then jump to member function
// short jump to relative address                   , e.g. 00A0424B E9 00 B5 00 00      jmp  oldFunc (0A0F750h)  
// instead of jumping to absolute address (0xFF 25) , e.g. 768B6466 FF 25 80 0E 92 76   jmp  dword ptr ds:[76920E80h]

#if defined(ASM_X86) && defined(BIT32)
struct Instruction_x86_thiscall
{
    unsigned char	m_mov       = 0xB9;     //mov ecx, %pThis
    unsigned long	m_this      = 0;        //the pointer to the C++ object
    unsigned char	m_jmp       = 0xE9;     //jmp instruction
    unsigned long	m_relproc   = 0;        //relative jmp from here to C++ member function
};
Static_Assert(10==sizeof(Instruction_x86_thiscall));
#endif // ASM_X86 && BIT32

/*
//this structure only used by _stdcall calling conventions 
//_stdcall calling conventions is push this value to stack, and then jump to member function
struct Instruction_x86_stdcall
{
    unsigned char	m_mov       = 0xB8;         //mov eax, %pThis
    unsigned long	m_this      = 0;            //the pointer to the C++ object
    unsigned long   m_xchg_push = 0x50240487;   //xchg eax, [esp] : push eax
    unsigned char   m_jmp       = 0xE9;         //jmp func
    unsigned long   m_relproc   = 0;            //relative jmp
};
Static_Assert(14==sizeof(Instruction_x86_thiscall));
*/
#pragma pack(pop)

template<typename T>
class ThisCall2StdCallTransfer
{ //NOTE : one ThisCall2StdCallTransfer instance is only used for one transfer. the m_thunk is not reusable.
public: 
    template<typename TObj, typename TR , typename... TArgs>
    auto getStdCallBack(const TObj* pThis, TR (TObj::*pMemF)(TArgs...))
    {
        typedef  TR(__stdcall *pStdCall_T)(TArgs...);
        if (!pThis || !pMemF)
        {
            return (pStdCall_T)nullptr;
        }

        m_thunk.m_relproc   = MiniMPL::any_cast<ptrdiff_t>(pMemF) - ((ptrdiff_t)(&m_thunk + 1));
        m_thunk.m_this      = (DWORD)pThis;

        void* pAddr         = &m_thunk;
        Win_x86::InstructionRW writter(::GetCurrentProcessId(), pAddr, sizeof(T));
        if (writter.writeMemory(pAddr, pAddr, sizeof(T)))
        {
            return MiniMPL::any_cast<pStdCall_T>(pAddr);     // no virtual-table , same with this
        }
        return (pStdCall_T)nullptr;
    }

protected:
    T       m_thunk;
};
#if defined(ASM_X86) && defined(BIT32)

typedef ThisCall2StdCallTransfer<Instruction_x86_thiscall>      CInstructionThunk;

#endif // ASM_X86 && BIT32

#endif // __CALLBACK_CPP2C_H__

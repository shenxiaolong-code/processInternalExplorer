#ifndef __OLETYPETRAITS_HPP__
#define __OLETYPETRAITS_HPP__

/************************************************************************************
	author:		Shen Tony,2015-06-09
	purpose:	define excel ole data type traits
*************************************************************************************/
#include <excel/ExcelEnvDef.h>
#ifdef EXCEL_SUPPORT

#include <MiniMPL/productSpecialDef.h>
#include <MiniMPL/kitType.hpp>
#include <wtypes.h>
#include <oaidl.h>

template<typename T> struct CExcelCType;
template<> struct CExcelCType<void>:public MiniMPL::Int2Type<VT_EMPTY>{};  
template<> struct CExcelCType<BYTE>:public MiniMPL::Int2Type<VT_UI1>{};  
template<> struct CExcelCType<USHORT>:public MiniMPL::Int2Type<VT_UI2>{};  
template<> struct CExcelCType<ULONG>:public MiniMPL::Int2Type<VT_UI4>{};  
template<> struct CExcelCType<ULONGLONG>:public MiniMPL::Int2Type<VT_UI8>{};  
template<> struct CExcelCType<char>:public MiniMPL::Int2Type<VT_I1>{};
template<> struct CExcelCType<short>:public MiniMPL::Int2Type<VT_I2>{};  
template<> struct CExcelCType<long>:public MiniMPL::Int2Type<VT_I4>{}; 
template<> struct CExcelCType<unsigned>:public MiniMPL::Int2Type<VT_I4>{}; 
template<> struct CExcelCType<float>:public MiniMPL::Int2Type<VT_R4>{};  
template<> struct CExcelCType<double>:public MiniMPL::Int2Type<VT_R8>{};  
template<> struct CExcelCType<CY>:public MiniMPL::Int2Type<VT_CY>{};  
//template<> struct CExcelCType<DATE>:public MiniMPL::Int2Type<VT_DATE>{};  
template<> struct CExcelCType<BSTR>:public MiniMPL::Int2Type<VT_BSTR>{};
template<> struct CExcelCType<char*>:public CExcelCType<BSTR>{};
template<> struct CExcelCType<LPDISPATCH>:public MiniMPL::Int2Type<VT_DISPATCH>{};  
//template<> struct CExcelCType<SCODE>:public MiniMPL::Int2Type<VT_ERROR>{};  
template<> struct CExcelCType<BOOL>:public MiniMPL::Int2Type<VT_BOOL>{};  
//template<> struct CExcelCType<VARIANT>:public MiniMPL::Int2Type<VT_VARIANT>{};  
template<> struct CExcelCType<VARIANT*>:public MiniMPL::Int2Type<VT_VARIANT>{};
template<> struct CExcelCType<LPUNKNOWN>:public MiniMPL::Int2Type<VT_UNKNOWN>{}; 

// VT_VECTOR	= 0x1000,
// VT_ARRAY	= 0x2000,
// VT_BYREF	= 0x4000,

template<VARENUM eVal> struct CExcelOleType;
template<> struct CExcelOleType<VT_EMPTY>:public MiniMPL::Type2Type<void>{};
template<> struct CExcelOleType<VT_UI1>:public MiniMPL::Value2Type<BYTE VARIANT::*,&VARIANT::bVal>{};  
template<> struct CExcelOleType<VT_UI2>:public MiniMPL::Value2Type<USHORT VARIANT::*,&VARIANT::uiVal>{};  
template<> struct CExcelOleType<VT_UI4>:public MiniMPL::Value2Type<ULONG VARIANT::*,&VARIANT::ulVal>{};  
template<> struct CExcelOleType<VT_UI8>:public MiniMPL::Value2Type<ULONGLONG VARIANT::*,&VARIANT::ullVal>{};  
template<> struct CExcelOleType<VT_I1>:public MiniMPL::Value2Type<char VARIANT::*,&VARIANT::cVal>{};  
template<> struct CExcelOleType<VT_I2>:public MiniMPL::Value2Type<short VARIANT::*,&VARIANT::iVal>{};  
template<> struct CExcelOleType<VT_I4>:public MiniMPL::Value2Type<long VARIANT::*,&VARIANT::lVal>{};  
template<> struct CExcelOleType<VT_R4>:public MiniMPL::Value2Type<float VARIANT::*,&VARIANT::fltVal>{};  
template<> struct CExcelOleType<VT_R8>:public MiniMPL::Value2Type<double VARIANT::*,&VARIANT::dblVal>{};  
template<> struct CExcelOleType<VT_CY>:public MiniMPL::Value2Type<CY VARIANT::*,&VARIANT::cyVal>{};  
template<> struct CExcelOleType<VT_DATE>:public MiniMPL::Value2Type<DATE VARIANT::*,&VARIANT::date>{};  
template<> struct CExcelOleType<VT_BSTR>:public MiniMPL::Value2Type<BSTR VARIANT::*,&VARIANT::bstrVal>{};  
template<> struct CExcelOleType<VT_DISPATCH>:public MiniMPL::Value2Type<LPDISPATCH VARIANT::*,&VARIANT::pdispVal>{};  
template<> struct CExcelOleType<VT_ERROR>:public MiniMPL::Value2Type<SCODE VARIANT::*,&VARIANT::scode>{};  
template<> struct CExcelOleType<VT_BOOL>:public MiniMPL::Value2Type<VARIANT_BOOL VARIANT::*,&VARIANT::boolVal>{};  
template<> struct CExcelOleType<VT_VARIANT>:public MiniMPL::Value2Type<VARIANT* VARIANT::*,&VARIANT::pvarVal>{};  
template<> struct CExcelOleType<VT_UNKNOWN>:public MiniMPL::Value2Type<LPUNKNOWN VARIANT::*,&VARIANT::punkVal>{};  

#endif // EXCEL_SUPPORT


#endif // __OLETYPETRAITS_HPP__

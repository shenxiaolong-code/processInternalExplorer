#ifndef __EXCELDATATYPE_H__
#define __EXCELDATATYPE_H__
/************************************************************************************
	author:		Shen Tony,2015-06-04
	purpose:	provide Excel single data type packaging , refer to COleVariant
*************************************************************************************/
#include <Excel/ExcelEnvDef.h>
#ifdef EXCEL_SUPPORT

#include <Excel/oleTypeTraits.hpp>
#include <comutil.h>
#include <MiniMPL/fromToString.hpp>
//#include <MiniMPL/typeTraits.hpp>
#include <MiniMPL/typeLogic.hpp>
#include <limits>

struct CELL 
{
    unsigned int m_iRow;     //m_iRow begin from 1
    unsigned int m_iCol;     //m_iCol begin from 1

    CELL(unsigned int iRow=1,unsigned int iCol=1):m_iRow(iRow),m_iCol(iCol){};
    bool operator==(CELL const& rOther) {return m_iRow==rOther.m_iRow && m_iCol==rOther.m_iCol;}    
};
_bstr_t makeCellName(CELL const& rCell);
bool parselCellName(_bstr_t const& rCellName,CELL& rCell); //rCellName likes "A23" or "AC23"
bool isAvailCellName(_bstr_t const& rCellName);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct CExcelCell : public _variant_t
{      
    static stlSmartptr<CExcelCell> convert( VARIANT const& val );
    virtual stlString       getStringValue()=0;

    template<typename T>
    bool getValue(T& val)
    {  
        if (getType()==typeid(T))
        {
            val = static_cast<CExcelCData<T>&>(*this).getValue();
            return true;
        }        
        return false;
    }

protected:
    virtual type_info const&  getType() = 0;
};
typedef stlSmartptr<CExcelCell>    TpCExcelCell;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T> struct CIsExcelBasicRWType: public MiniMPL::BoolType<MiniMPL::Or_T<MiniMPL::And_T<MiniMPL::IsBuildInType<T>,MiniMPL::Not_T<MiniMPL::IsCPointer<T> > > , MiniMPL::IsString<T> >::value>{};
template<typename T> struct CExcelCData  : public CExcelCell
{
    enum {value=CExcelCType<T>::value};

    CExcelCData(){};
    template<typename U> CExcelCData(U& val)
    { 
        new((_variant_t*)this) _variant_t(val);
    };

    virtual type_info const&  getType()
    {
        return typeid(T);
    }

    virtual stlString   getStringValue()
    {
        return getStringValueImpl<T>();
    }

    T getValue()
    {
        return (*this).*CExcelOleType<VARENUM(value)>::value;
    }

protected:   
    template<typename U>
    stlString   getStringValueImpl(ENABLE_IF(CIsExcelBasicRWType<U>::value))
    { 
        return MiniMPL::toString((*this).*CExcelOleType<VARENUM(value)>::value);
    }
    template<typename U>
    stlString   getStringValueImpl(ENABLE_IF(!CIsExcelBasicRWType<U>::value))
    {
        return TXT(""); //non-basic type,e.g. void/tagCY/IDispatch*/tagVARIANT*/IUnknown *
    }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct CExcelCells : public _variant_t
{
    friend class COleExcel;

    CExcelCells();
    virtual ~CExcelCells();

    stlSmartptr<CExcelCells>    clone();
    bool                        import(VARIANT const& val);
    TpCExcelCell                get(unsigned iRow,unsigned iCol);       //iRow from 1, iCol from 1
    bool                        setCellsName(_bstr_t const& rCellsName); //rCellName like "A5:M13"
    void                        releaseBuffer();

    template<bool isRow,typename T,unsigned TCols>
    int exportAllValue(T (&binryArray)[TCols])
    {
        typedef typename ArrayDim1To2<T[TCols]>::template apply<isRow>::type TpArrDim2;  
        return exportAllValue(*reinterpret_cast<TpArrDim2>(&binryArray));
    }

    template<typename T,unsigned TRows,unsigned TCols>
    int exportAllValue(T (&binryArray)[TRows][TCols])
    { 
        if (m_rows!=TRows || m_cols!=TCols)
        {
            ASSERT_AND_LOG(!"unmatched binary array size");
            return 0;
        }

        int  iOkNum = 0;
        for (unsigned iCol=1;iCol<=m_cols;++iCol)
        {
            for(unsigned iRow=1;iRow<=m_rows;++iRow)
            {
                TpCExcelCell pCell = get(iRow,iCol);
                if (pCell)
                {
                    if (pCell->getValue(binryArray[iRow-1][iCol-1]))
                    {
                        iOkNum++;
                    }
                    else
                    {
                        setquiet_NaN(binryArray[iRow-1][iCol-1]);
                    }
                }
            }
        }
        return iOkNum;
    }

    template<typename T>
    bool fillCell(unsigned iRow,unsigned iCol,T val)    //iRow from 1, iCol from 1
    {
        if (iRow<1 || iRow>m_rows || iCol<1 || iCol>m_cols)
        {
            ASSERT_AND_LOG(!L"Out of range");
            return false;
        }

        CExcelCData<T> obj(val);
        long indices[] = { long(iRow),long(iCol)};
        HRESULT hr = SafeArrayPutElement(parray, indices, (void *)&obj.GetVARIANT());
        return SUCCEEDED(hr);
    }

protected:
    bool setRangeName(_bstr_t const& rangeName);
    template<typename T>
    void setquiet_NaN(T& val,ENABLE_IF(!std::numeric_limits<T>::has_quiet_NaN)){}
    template<typename T>
    void setquiet_NaN(T& val,ENABLE_IF(std::numeric_limits<T>::has_quiet_NaN))
    {
        val = std::numeric_limits<T>::quiet_NaN();
    }

    template<typename T>                struct ArrayDim1To2;
    template<typename T,unsigned TCols> struct ArrayDim1To2<T[TCols]>
    {
        template<bool isRow>
        struct apply:public MiniMPL::If_T<isRow,T(*)[1][TCols],T(*)[TCols][1]>{};
    };

public:
    bool        m_bCreated;
    unsigned    m_rows;
    unsigned    m_cols;
    _bstr_t     m_rangeName;
};
typedef stlSmartptr<CExcelCells>    TpCExcelCells; 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif // EXCEL_SUPPORT


#endif // __EXCELDATATYPE_H__

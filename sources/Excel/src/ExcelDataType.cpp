#include <Excel/ExcelDataType.h>

#ifdef EXCEL_SUPPORT
#include <MiniMPL/memberPtr.hpp>
#include <MiniMPL/fromToString.hpp>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
_bstr_t makeCellName(CELL const& rCell)
{
    _bstr_t cellName;
    if (rCell.m_iCol<1 || rCell.m_iRow<1)
    {
        return cellName;
    }

    int iExp = int(floor(log(double(rCell.m_iCol)) / log(double(26))));
    wchar_t   buf[32]={0};
    unsigned  vCol=rCell.m_iCol;
    for (int idx=iExp;idx >=0 ; --idx )
    {
        buf[idx] = 'A'+vCol%26-1;
        vCol    /= 26;
    }
    cellName = buf;
    cellName += MiniMPL::toStringW(rCell.m_iRow).c_str();
    return cellName;
}

bool parselCellName(_bstr_t const& rCellName,CELL& rCell)
{
    wchar_t   buf[32]={0};
    if (2==swscanf(rCellName,L"%[A-Za-z]%d",buf,&rCell.m_iRow))
    {
        rCell.m_iCol = 0;
        int iColNameSize=std::char_traits<wchar_t>::length(buf);
        for (int idx=0;idx<iColNameSize;++idx)
        {
            rCell.m_iCol *= 26;
            rCell.m_iCol += buf[idx]-'A'+1;
        }
        return rCell.m_iCol >= 1 && rCell.m_iRow >= 1;
    }
    return false;
}

bool isAvailCellName( _bstr_t const& rCellName )
{ 
    CELL rCell;
    return parselCellName(rCellName,rCell);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<VARENUM eVal> 
TpCExcelCell importExcelCData(VARIANT const& oleVal)
{
    typedef typename MiniMPL::GetMember<typename CExcelOleType<eVal>::type>::type   Value_T;
    return TpCExcelCell(new CExcelCData<Value_T>(oleVal));
}

TpCExcelCell CExcelCell::convert( VARIANT const& val )
{
    TpCExcelCell pCData;
    switch(val.vt)
    {
    case VT_EMPTY:
        {
            pCData = TpCExcelCell(new CExcelCData<void>());//void type
        }
        break;
    case VT_I2:
        {
            pCData = importExcelCData<VT_I2>(val);
        }
        break;
    case VT_I4:
        {
            pCData = importExcelCData<VT_I4>(val);
        }
        break;
    case VT_R4:
        {
            pCData = importExcelCData<VT_R4>(val);
        }
        break;
    case VT_R8:
        {
            pCData = importExcelCData<VT_R8>(val);
        }
        break;
    case VT_CY:
        {
            pCData = importExcelCData<VT_CY>(val);
        }
        break;
    case VT_DATE:
        {
            pCData = importExcelCData<VT_DATE>(val);
        }
        break;
    case VT_BSTR:
        {
            pCData = importExcelCData<VT_BSTR>(val);
        }
        break;
    case VT_DISPATCH:
        {
            pCData = importExcelCData<VT_DISPATCH>(val);
        }
        break;
    case VT_ERROR:
        {
            pCData = importExcelCData<VT_ERROR>(val);
        }
        break;
    case VT_BOOL:
        {
            pCData = importExcelCData<VT_BOOL>(val);
        }
        break;
    case VT_VARIANT:
        {
            pCData = importExcelCData<VT_VARIANT>(val);
        }
        break;
    case VT_UNKNOWN:
        {
            pCData = importExcelCData<VT_UNKNOWN>(val);
        }
        break;
    default:
        {
            ASSERT_AND_LOG(!TXT("Not supported data type"));
            if (VT_ARRAY & val.vt)
            { //it is safe array

            }
        }
        break;
    }
    return pCData;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CExcelCells::CExcelCells() :m_rows(0),m_cols(0),m_bCreated(false)
{
    vt = VT_ARRAY | VT_VARIANT;
}

CExcelCells::~CExcelCells()
{
    releaseBuffer();
}

stlSmartptr<CExcelCells> CExcelCells::clone()
{
    CExcelCells* pClone = new CExcelCells();
    pClone->setCellsName(m_rangeName);
    pClone->import(*this);  
    return stlSmartptr<CExcelCells>(pClone);
}

bool CExcelCells::import( VARIANT const& val )
{
    if (!(val.vt & VT_ARRAY))
    {
        ASSERT_AND_LOG(!"Not VT_ARRAY Array");
        return false;
    }
    vt = val.vt;
    HRESULT hr = SafeArrayCopy(val.parray,&parray); //rows/columns is decided by SafeArrayCreate
    ASSERT_AND_LOG(m_rows==val.parray->rgsabound[1].cElements); //or SafeArrayGetUBound(val.parray,1,&Dim1);
    ASSERT_AND_LOG(m_cols==val.parray->rgsabound[0].cElements); //or SafeArrayGetUBound(val.parray,2,&Dim2);
    return m_bCreated;
}

TpCExcelCell CExcelCells::get( unsigned iRow,unsigned iCol )
{
    if (iRow>m_rows || iCol>m_cols)
    {
        ASSERT_AND_LOG(!L"Out of range");
        return TpCExcelCell();
    }

    VARIANT val;
    long indices[] = { long(iRow),long(iCol)};
    HRESULT hr = SafeArrayGetElement(parray, indices, &val);
    if (SUCCEEDED(hr))
    {
        return CExcelCell::convert(val);
    }
    return TpCExcelCell();
}

bool CExcelCells::setCellsName( _bstr_t const& rCellsName )
{
    if (rCellsName==m_rangeName)
    {
        return true;
    }
    releaseBuffer();
    if (!setRangeName(rCellsName))
    {
        return false;
    }

    SAFEARRAYBOUND sab[2] = 
    {
        {m_rows ,1},   //cElements,lLbound
        {m_cols ,1}
    };
    parray = SafeArrayCreate(VT_VARIANT, 2, sab);
    m_bCreated = true;
    return true;
}

void CExcelCells::releaseBuffer()
{
    if (m_bCreated)
    {
        SafeArrayDestroy(parray);
        m_bCreated = false;
    }
}

bool CExcelCells::setRangeName( _bstr_t const& rangeName )
{
    wchar_t buf[2][32]={0};
    if (2==swscanf(rangeName,L"%[A-Za-z0-9]:%[A-Za-z0-9]",buf[0],buf[1]))    //like "A15:B23" or "A15:BC23"
    {
        CELL rCell[2];
        if (parselCellName(buf[0],rCell[0]) && parselCellName(buf[1],rCell[1]))
        { 
            m_rangeName = rangeName;
            m_rows = rCell[1].m_iRow-rCell[0].m_iRow+1;
            m_cols = rCell[1].m_iCol-rCell[0].m_iCol+1;
            return true;
        }
    }
    ASSERT_AND_LOG(!L"Wrong cells name");
    return false;
}

#endif // EXCEL_SUPPORT

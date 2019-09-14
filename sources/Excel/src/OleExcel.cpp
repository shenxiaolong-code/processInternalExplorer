#include <Excel/OleExcel.h>
#ifdef EXCEL_SUPPORT

#include <ole2.h> // OLE2 Definitions
#include <MiniMPL/macroDef.h>
#include <iosfwd>
#include <Excel/ExcelFileFormat.h>
#include <atlcomcli.h>

#pragma comment(lib, "comsuppw.lib")

extern HRESULT AutoWrap(int autoType, VARIANT *pvResult, IDispatch *pDisp, LPOLESTR ptName, int cArgs...) ;

CLSID COleExcel::excelSeverId = GUID_NULL;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OlePointer::OlePointer( IDispatch* impl/*=NULL*/ ) 
: m_impl(impl)
, m_counter(m_impl ? new TRefCounter(1) : NULL)
{
}

OlePointer::OlePointer( OlePointer const& rOther )
: m_impl(rOther.m_impl)
, m_counter(rOther.m_counter)
{
    if (m_counter)
    {
        ++*m_counter;
    }
}

HRESULT OlePointer::GetIDispatchMethods(/* _In_ IDispatch * m_impl,*/_Out_ std::map<long, std::wstring> & methodsMap )
{
    if (!m_impl)
    {
        return  S_FALSE;
    }

    HRESULT hr = S_OK;

    CComPtr<IDispatch> spDisp(m_impl);
    if(!spDisp)
        return E_INVALIDARG;

    CComPtr<ITypeInfo> spTypeInfo;
    hr = spDisp->GetTypeInfo(0, 0, &spTypeInfo);
    if(SUCCEEDED(hr) && spTypeInfo)
    {
        TYPEATTR *pTatt = NULL;
        hr = spTypeInfo->GetTypeAttr(&pTatt);
        if(SUCCEEDED(hr) && pTatt)
        {
            FUNCDESC * fd = NULL;
            for(int i = 0; i < pTatt->cFuncs; ++i)
            {
                hr = spTypeInfo->GetFuncDesc(i, &fd);
                if(SUCCEEDED(hr) && fd)
                {
                    CComBSTR funcName;
                    spTypeInfo->GetDocumentation(fd->memid, &funcName, NULL, NULL, NULL);
                    if(funcName.Length()>0)
                    {
                        methodsMap[fd->memid] = funcName;
                    }

                    spTypeInfo->ReleaseFuncDesc(fd);
                }
            }

            spTypeInfo->ReleaseTypeAttr(pTatt);
        }
    }

    return hr;
}

OlePointer::operator IDispatch*&()
{
    return m_impl;
}

OlePointer::~OlePointer()
{
    checkRelease();
}

void OlePointer::checkRelease()
{
    if (m_impl && 0==--*m_counter)
    {
        m_impl->Release();
        m_impl = NULL;
        delete m_counter;
        m_counter= NULL;
    }
}

OlePointer& OlePointer::operator=( IDispatch* impl )
{
    if (m_impl == impl)
    {
        return *this;
    }

    checkRelease();
    new(this) OlePointer(impl);

    return *this;
}

OlePointer& OlePointer::operator=( OlePointer const& rOther )
{
    if (m_impl==rOther.m_impl)
    {
        return *this;
    }

    checkRelease();
    new(this) OlePointer(rOther);

    return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LPOLESTR COleExcelSheet::getCellText( LPOLESTR pCellName )
{  
    if (!isAvailCellName(pCellName))
    {
        return NULL;
    }

    _variant_t result;
    VariantInit(&result);
    OlePointer pSheetRange = getSheetRange(pCellName);
    if (pSheetRange)
    {
        m_hr = AutoWrap(DISPATCH_PROPERTYGET, &result, pSheetRange, L"Text", 0);

    }
    return (_bstr_t)result;
}

TpCExcelCell COleExcelSheet::getValue_Cell( LPOLESTR pCellName )
{
    if (!isAvailCellName(pCellName))
    {
        return TpCExcelCell();
    }

    OlePointer pSheetRange = getSheetRange(pCellName);
    CHECK_NULL_ELSE_RETURN_VAL(pSheetRange,TpCExcelCell());

    _variant_t val;
    m_hr = AutoWrap(DISPATCH_PROPERTYGET, &val, pSheetRange, L"Value", 0);    
    if (FAILED(m_hr))
    {
        return TpCExcelCell();
    }

    return CExcelCell::convert(val);
}

TpCExcelCells COleExcelSheet::getValue_Cells( LPOLESTR pRangeName )
{
    OlePointer pSheetRange = getSheetRange(pRangeName);
    CHECK_NULL_ELSE_RETURN_VAL(pSheetRange,TpCExcelCells());

    TpCExcelCells pNewResult( new CExcelCells());
    CExcelCells& rObj = *pNewResult;
    m_hr = AutoWrap(DISPATCH_PROPERTYGET, &rObj, pSheetRange, L"Value", 0);
    if (FAILED(m_hr))
    {
        pNewResult.reset();
    }
    else
    {
        SAFEARRAY* pArr = rObj.parray;
        rObj.m_rows = pArr->rgsabound[1].cElements;
        rObj.m_cols = pArr->rgsabound[0].cElements;
        rObj.m_rangeName = pRangeName;
    }
    return pNewResult;
}

bool COleExcelSheet::setValue_Cells( CExcelCells const& rangeData )
{
    // Set range with our safearray...
    OlePointer pSheetRange = getSheetRange(rangeData.m_rangeName);
    if (pSheetRange)
    {
        return setValue(*pSheetRange,rangeData);
    }
    return false; 
}

bool COleExcelSheet::setValue( IDispatch& rSheetRange,VARIANT const& rVal )
{ 
    // Set range with our safearray...
    m_hr = AutoWrap(DISPATCH_PROPERTYPUT, NULL, &rSheetRange, L"Value", 1, rVal);
    return SUCCEEDED(m_hr);
}

OlePointer COleExcelSheet::getSheetRange( LPOLESTR pRangeName )
{
    _variant_t parm(pRangeName);

    VARIANT result;
    VariantInit(&result);

    m_hr = AutoWrap(DISPATCH_PROPERTYGET, &result, m_pWorksheet, L"Range", 1, parm.GetVARIANT());
    return result.pdispVal;
}

COleExcelSheet::COleExcelSheet( IDispatch* pWorksheet )
:m_hr(0),m_pWorksheet(pWorksheet)
{
    ASSERT_AND_LOG(NULL!=pWorksheet);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
COleExcel::COleExcel()
: m_hr(0),m_XlApp(NULL),m_pBooks(NULL),m_pActiveBook(NULL)
{
    initial();
}

COleExcel::~COleExcel()
{
    if (m_XlApp)
    {
        quit();
    }
    CoUninitialize();
}

OlePointer COleExcel::open( stlString fileName,bool bReadonly )
{
    if (!updateWorkbooks())
    {
        return NULL;
    }

    _variant_t  var(fileName.c_str());  //_bstr_t  sFname(fileName.c_str());
    //     _variant_t  vUpdateLinks = 3;//xlUpdateLinksAlways;
    //     _variant_t  vReadonly = bReadonly;

    VARIANT     result;
    VariantInit(&result);
    m_hr = AutoWrap(DISPATCH_PROPERTYGET, &result, m_pBooks, L"Open", 1,var); //var type : VARIANT
    m_pActiveBook = result.pdispVal;
    return m_pActiveBook;
}

bool COleExcel::forceOverwrite( bool bOverwrite )
{
    VARIANT vParam;
    vParam.vt = VT_BOOL;
    vParam.boolVal = bOverwrite ? VARIANT_FALSE : VARIANT_TRUE;
    m_hr = AutoWrap(DISPATCH_PROPERTYPUT, NULL, m_XlApp, L"DisplayAlerts", 1, vParam); //AlertBeforeOverwriting can't work

    return SUCCEEDED(m_hr);
}

bool COleExcel::save(bool bOverwritePrompt/*=false*/)
{
    CHECK_NULL_ELSE_RETURN_VAL(m_pActiveBook,false);
    forceOverwrite(!bOverwritePrompt);
    m_hr = AutoWrap(DISPATCH_METHOD, NULL, m_pActiveBook, L"Save", 0); //var type : VARIANT
    return SUCCEEDED(m_hr);
}

bool COleExcel::saveAs( stlString const& fileName ,bool bOverwritePrompt/*=false*/)
{       
    return saveAs(m_pActiveBook,fileName,bOverwritePrompt);
}

bool COleExcel::saveAs( IDispatch* pBook,stlString const& fileName ,bool bOverwritePrompt/*=false*/)
{
    CHECK_NULL_ELSE_RETURN_VAL(pBook,false);
    forceOverwrite(!bOverwritePrompt);

    _variant_t  var(fileName.c_str());  //_bstr_t  sFname(fileName.c_str());
    VARIANT varAs;
    varAs.vt=VT_I4;
    varAs.intVal=xlWorkbookNormal;   //.xls file format
    m_hr = AutoWrap(DISPATCH_METHOD, NULL, pBook, L"SaveAs", 2,varAs,var); //var type : VARIANT
    return SUCCEEDED(m_hr);
}

bool COleExcel::makeVisible()
{
    CHECK_NULL_ELSE_RETURN_VAL(m_XlApp,false);

    // Make it visible (i.e. app.visible = 1)
    VARIANT x;
    x.vt = VT_I4;
    x.lVal = 1;
    m_hr = AutoWrap(DISPATCH_PROPERTYPUT, NULL, m_XlApp, L"Visible", 1, x);
    return SUCCEEDED(m_hr);
}

OlePointer COleExcel::addNewWorkbook()
{
    CHECK_NULL_ELSE_RETURN_VAL(m_XlApp,m_pActiveBook);
    if (!updateWorkbooks())
    {
        return m_pActiveBook;
    }

    VARIANT result;
    VariantInit(&result);
    m_hr = AutoWrap(DISPATCH_PROPERTYGET, &result, m_pBooks, L"Add", 0);
    m_pActiveBook = result.pdispVal;
    return m_pActiveBook;
}

bool COleExcel::close( IDispatch* pWorkbook,bool bSaveChanges )
{  
    CHECK_NULL_ELSE_RETURN_VAL(pWorkbook,false);

    _variant_t param = bSaveChanges;
    m_hr = AutoWrap(DISPATCH_METHOD, NULL, pWorkbook, L"Close", 1, param.GetVARIANT());
    return SUCCEEDED(m_hr);
}

bool COleExcel::SetSaveFlag(IDispatch* pWorkbook)
{
    CHECK_NULL_ELSE_RETURN_VAL(pWorkbook,false);

    // Set .Saved property of workbook to TRUE so we aren't prompted
    // to save when we tell Excel to quit...
    VARIANT x;
    x.vt = VT_I4;
    x.lVal = 1;
    m_hr = AutoWrap(DISPATCH_PROPERTYPUT, NULL, pWorkbook, L"Saved", 1, x);
    return SUCCEEDED(m_hr);
}

TpExcelSheet COleExcel::getActiveSheet()
{
    CHECK_NULL_ELSE_RETURN_VAL(m_XlApp,TpExcelSheet());

    VARIANT result;
    VariantInit(&result);
    m_hr = AutoWrap(DISPATCH_PROPERTYGET, &result, m_XlApp, L"ActiveSheet", 0);
    return TpExcelSheet(new COleExcelSheet(result.pdispVal));
}

TpExcelSheet COleExcel::getSheet( int nSheetNo )
{
    TpExcelSheet pExcelSheet;
    CHECK_NULL_ELSE_RETURN_VAL(m_XlApp,pExcelSheet);
    CHECK_NULL_ELSE_RETURN_VAL(m_pActiveBook,pExcelSheet);

    OlePointer pSheets;
    {
        VARIANT result;
        VariantInit(&result);

        m_hr = AutoWrap(DISPATCH_PROPERTYGET, &result, m_pActiveBook, L"Sheets", 0);
        pSheets = result.pdispVal;
    }
    CHECK_TRUE_ELSE_RETURN_VAL(SUCCEEDED(m_hr),pExcelSheet);

    {
        VARIANT result;
        VariantInit(&result);
        VARIANT itemn;
        itemn.vt = VT_I4;
        itemn.lVal = nSheetNo;
        m_hr = AutoWrap(DISPATCH_PROPERTYGET, &result, pSheets, L"Item", 1, itemn);
        if (SUCCEEDED(m_hr))
        {
            pExcelSheet.reset(new COleExcelSheet(result.pdispVal));
        }
    }

    return pExcelSheet;
}

bool COleExcel::initial()
{
    // Initialize COM for this thread...
    CoInitialize(NULL);    
    if (IsEqualCLSID(excelSeverId,GUID_NULL))
    {
        // Get CLSID for our server...
        m_hr = CLSIDFromProgID(L"Excel.Application", &excelSeverId);
        if (FAILED(m_hr))
        {
            reportError(TXT("CLSIDFromProgID() failed"));
            return false;
        }
    }

    // Start server and get IDispatch... 
    m_hr = CoCreateInstance(excelSeverId, NULL, CLSCTX_LOCAL_SERVER, IID_IDispatch, (void **)&m_XlApp);
    if(FAILED(m_hr)) 
    {
        reportError(TXT("Excel not registered properly"));
        return false;
    }
    return true;
}

bool COleExcel::updateWorkbooks()
{ 
    CHECK_NULL_ELSE_RETURN_VAL(m_XlApp,false);
    VARIANT result;
    VariantInit(&result);
    m_hr = AutoWrap(DISPATCH_PROPERTYGET, &result, m_XlApp, L"Workbooks", 0);
    m_pBooks = result.pdispVal;
    return SUCCEEDED(m_hr);
}

bool COleExcel::quit()
{
    // Tell Excel to quit (i.e. App.Quit)
    m_hr = AutoWrap(DISPATCH_METHOD, NULL, m_XlApp, L"Quit", 0);
    m_XlApp->Release();
    m_XlApp = NULL;

    m_pActiveBook = NULL;
    m_pBooks = NULL;
    return SUCCEEDED(m_hr);
}

bool COleExcel::reportError( stlString sMsg )
{
    outputTxt(sMsg.c_str());
    return true;
}
#endif // EXCEL_SUPPORT

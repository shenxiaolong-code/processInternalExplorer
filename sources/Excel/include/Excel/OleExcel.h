#ifndef __OLEEXCEL_H__
#define __OLEEXCEL_H__

/***********************************************************************************************************************
* Description         : access excel by OLE.                        
* Author              : Shen.Xiaolong (Shen Tony) (2009-2015)
* Mail                : xlshen2002@hotmail.com,  xlshen@126.com
* verified platform   : VS2008 (2015/06/03 15:35:10.15)
* copyright:          : free to use / modify / sale in free and commercial software.
*                       Unique limit: MUST keep those copyright comments in all copies and in supporting documentation.
*                       latest Version of The Code Project Open License (CPOL : http://www.codeproject.com/)
***********************************************************************************************************************/
#include <Excel/ExcelDataType.h>
#ifdef EXCEL_SUPPORT

#include <MiniMPL/libConfig.h>
#include <MiniMPL/productSpecialDef.h>
#include <oaidl.h>
#include <MiniMPL/atomic_counter.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//to maintain IDispatch release
typedef MiniMPL::atomic_counter     TRefCounter;
struct OlePointer 
{
    OlePointer(IDispatch* impl=NULL);
    OlePointer(OlePointer const& rOther);

    OlePointer& operator=(OlePointer const& rOther);
    OlePointer& operator=(IDispatch* impl);

    HRESULT GetIDispatchMethods(_Out_ std::map<long, std::wstring> & methodsMap);

    operator IDispatch*&();
    ~OlePointer();

protected:
    void checkRelease();

protected:
    IDispatch*          m_impl;
    TRefCounter*        m_counter;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//sheet : cell access unit
class COleExcelSheet
{
public:
    COleExcelSheet(IDispatch* pWorksheet);

    LPOLESTR        getCellText(LPOLESTR pCellName);

    TpCExcelCell    getValue_Cell(LPOLESTR pCellName);        //pCellName like L"B10"
    TpCExcelCells   getValue_Cells(LPOLESTR pRangeName);      //pRangeName like L"A1:O15"    

    template<typename T>    bool setValue_cell(LPOLESTR pCellName,T val)  //pCellName like "A10" 
    {
        OlePointer pSheetRange = getSheetRange(pCellName);
        if (pSheetRange)
        {
            _variant_t _val(val);
            return setValue(*pSheetRange,_val);
        }
        return true;
    }
    bool setValue_Cells(CExcelCells const& rangeData);

protected:
    bool setValue(IDispatch& rSheetRange,VARIANT const& rVal);  //basic implement interface
    // Get Range object for the Range, e.g. pRangeName = A1:O15... 
    OlePointer      getSheetRange(LPOLESTR pRangeName);       //pRangeName like L"A1:O15" or L"B10"

private:
    HRESULT     m_hr;
    OlePointer  m_pWorksheet;
};

typedef stlSmartptr<COleExcelSheet>     TpExcelSheet;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class COleExcel
{
public:
    static CLSID excelSeverId;

    COleExcel();
    virtual ~COleExcel();

    //open excel bool, return ActiveBook
    OlePointer  open(stlString fileName,bool bReadonly);
    bool close(IDispatch* pWorkbook,bool bSaveChanges);

    OlePointer  addNewWorkbook();   // Call Workbooks.Add() to generate a new workbook...

    bool SetSaveFlag(IDispatch* pWorkbook); //set save flag to avoid prompt window when quit
    bool save(bool bOverwritePrompt=false);
    bool saveAs(stlString const& fileName,bool bOverwritePrompt=false);
    bool saveAs(IDispatch* pBook,stlString const& fileName,bool bOverwritePrompt=false);

    //make app visible
    bool makeVisible();

    TpExcelSheet  getActiveSheet();
    TpExcelSheet  getSheet(int nSheetNo);
protected:
    bool forceOverwrite(bool bOverwrite);
    bool initial();
    bool updateWorkbooks();
    bool quit();
    virtual bool reportError(stlString sMsg);

private: 
    //Excel structure:application->Workbooks->WorkBook->WorkSheets->Worksheet->range
    //WorkBook is storage unit      : save single file(Save)
    //application is storage unit   : save all files(SaveAll)
    //Workbooks is container of Workbook,WorkSheets is container of WorkSheet
    HRESULT     m_hr;
    IDispatch*  m_XlApp;            //all other pointer MUST be released before m_XlApp is released
    OlePointer  m_pBooks;
    OlePointer	m_pActiveBook;
};
#endif // EXCEL_SUPPORT


#endif // __OLEEXCEL_H__

/***********************************************************************************************************************
* Description         : test file for <Excel\OleExcel.h>
* Author              : Shen.Xiaolong (Shen Tony,2009-2016)
* Mail                : xlshen2002@gmail.com,  xlshen@126.com
* verified platform   : VS2008 (2016/04/20 11:23:18.07)
* copyright:          : free to use / modify / sale in free and commercial software.
*                       Unique limit: MUST keep those copyright comments in all copies and in supporting documentation.
* usage demo          : #define RUN_EXAMPLE_OLEEXCEL to run this demo
***********************************************************************************************************************/
#define RUN_EXAMPLE_OLEEXCEL

#ifdef COMPILE_EXAMPLE_ALL
    #define COMPILE_EXAMPLE_OLEEXCEL
#endif

#ifdef RUN_EXAMPLE_ALL
    #define RUN_EXAMPLE_OLEEXCEL
#endif

#if defined(RUN_EXAMPLE_OLEEXCEL) && !defined(COMPILE_EXAMPLE_OLEEXCEL)
    #define COMPILE_EXAMPLE_OLEEXCEL
#endif

//#undef RUN_EXAMPLE_OLEEXCEL
//#undef COMPILE_EXAMPLE_OLEEXCEL

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//best reference article
//MS Office OLE Automation Using C++
//http://www.codeproject.com/Articles/34998/MS-Office-OLE-Automation-Using-C
//How to automate Excel from C++ without using MFC or #import
//https://support.microsoft.com/en-us/kb/216686
//Locating Resources to Study Automation
//https://support.microsoft.com/en-us/kb/152023/en-us
//Office Automation Using Visual C++
//https://support.microsoft.com/en-us/kb/196776
////////////////////////////////////////////usage & test demo code//////////////////////////////////////////////////////////
#ifdef COMPILE_EXAMPLE_OLEEXCEL
#include <Excel/OleExcel.h>
#include <UnitTestKit/tc_tracer.h>
#include "../testconfig_Excel.h"
#include <shlwapi.h>
#include <MiniMPL/macro_init.h>

#pragma comment(lib,"shlwapi.lib")

namespace UnitTest
{
    #define Excel_Example_Name L"excel_example.xls"

    void TestCase_OleExcelGeneral()
    {
        PrintTestcase();

        CELL rCell(555,555);
        _bstr_t  name = makeCellName(rCell);

        CELL rNewCell;
        ASSERT_AND_LOG(parselCellName(name,rNewCell));
        ASSERT_AND_LOG(rCell==rNewCell);

        ASSERT_AND_LOG(parselCellName(L"AC4",rNewCell));
        ASSERT_AND_LOG(rNewCell.m_iRow==4);
        ASSERT_AND_LOG(rNewCell.m_iCol==29);
        ASSERT_AND_LOG(makeCellName(CELL(4,29))==_bstr_t(L"AC4"));
    }

    void TestCase_OleExcelClass_Write()
    {
        PrintTestcase();

        COleExcel   rExcel;
        rExcel.makeVisible();
        OlePointer pBook = rExcel.addNewWorkbook();
        ASSERT_AND_LOG(pBook!=NULL);

        CExcelCells cells;
        cells.setCellsName("A1:M13");
        for (unsigned i=1;i<=cells.m_rows;++i)
        {
            for (unsigned j=1;j<=cells.m_cols;++j)
            {
                if ((i+j)==(cells.m_rows+cells.m_cols)/2)
                {
                    char buf[32]={0};
                    sprintf_s(buf,32,"str_%d_%d",i,j);
                    //                     wchar_t buf[32]={0};
                    //                     wsprintf(buf,L"str_%d_%d",i,j);
                    cells.fillCell(i,j,buf);
                }
                else
                {
                    cells.fillCell(i,j,i*j);
                }
            }
        }

        TpExcelSheet pActiveSheet = rExcel.getActiveSheet();
        pActiveSheet->setValue_Cells(cells);
        rExcel.SetSaveFlag(pBook);
        LPOLESTR pCellText=pActiveSheet->getCellText(L"D5");
        ASSERT_AND_LOG(0==wmemcmp(pCellText,L"20",2));;

        TpCExcelCells pCells = pActiveSheet->getValue_Cells(L"A1:M13");        
        if (pCells)
        {
            TpCExcelCells pCloneCells = pCells->clone();
            CExcelCells& rCells = *pCells;
            for (unsigned i=1;i<=rCells.m_rows;++i)
            {
                for (unsigned j=1;j<=rCells.m_cols;++j)
                {
                    TpCExcelCell pCell = rCells.get(i,j);
                    ASSERT_AND_LOG(pCell);
                    outputTxtV(L"[%d:%d]%s\n",i,j,pCell->getStringValue().c_str());
                }
            }
        }

        TpCExcelCell pCellValue = pActiveSheet->getValue_Cell(L"A2");
        if (pCellValue)
        {
            stlString strValue = pCellValue->getStringValue();
            outputTxtV(L"[A2:B3]%s\n",strValue.c_str());

            double dVal=0.0;
            pCellValue->getValue(dVal);
        }

        wchar_t path[256]={0};
        GetModuleFileName(NULL,path,arraySize(path));
        PathRemoveFileSpecW(path);
        wchar_t excelName[]=L"excelTest_Write.xls";
        wchar_t excelPath[256]={0};
        printBufW(excelPath,L"%s\\%s",path,excelName);
        rExcel.save();
        rExcel.saveAs(MiniMPL::toString(excelPath));
    }

    void TestCase_OleExcelClass_Read()
    {
        PrintTestcase();

        //prepare read excel meterial
        wchar_t path[256]={0};
        GetModuleFileName(NULL,path,arraySize(path));
        PathRemoveFileSpecW(path);
        wchar_t excelName_dst[]=L"excelTest_Read.xls";
        wchar_t excelPath_dst[256]={0};
        wsprintf(excelPath_dst,L"%s\\%s",path,excelName_dst);
        wchar_t excelDir_src[256]={0};
        GetCurrentDirectory(arraySize(excelDir_src),excelDir_src);

        wchar_t excelPath_src[256]={0};
        wsprintf(excelPath_src,L"%s\\%s",excelDir_src,Excel_Example_Name);

        //copy read excel file to module folder
        wchar_t dosCmd[256]={0};
        wsprintf(dosCmd,L"copy/y %s %s",excelPath_src,excelPath_dst);
        system(MiniMPL::toStringA(dosCmd).c_str());

        COleExcel   rExcel;
        rExcel.makeVisible(); 
        OlePointer pBook = rExcel.open(MiniMPL::toString(excelPath_dst),true);
        ASSERT_AND_LOG(pBook!=NULL);
        TpExcelSheet pActiveSheet = rExcel.getActiveSheet();
        TpCExcelCells pCells = pActiveSheet->getValue_Cells(L"A1:E13");

        stlString strValue;
        TpCExcelCell pVal= pActiveSheet->getValue_Cell(L"D4");
        strValue = pVal->getStringValue();
        outputTxtV(TXT("%s\n"),strValue.c_str());
        pVal = pActiveSheet->getValue_Cell(makeCellName(CELL(8,3)));
        strValue = pVal->getStringValue();
        outputTxtV(TXT("%s\n"),strValue.c_str());
        for (int i=1;i<7;++i)
        {
            for (int j=1;j<13;++j)
            {
                pVal = pActiveSheet->getValue_Cell(makeCellName(CELL(i,j)));
                strValue = pVal->getStringValue();
                outputTxtV(TXT("%s\n"),strValue.c_str());
            }
        }

        //std::numeric_limits
        pCells = pActiveSheet->getValue_Cells(L"D3:H6");
        ASSERT_AND_LOG(pCells);
        double buffers2Dim[4][5] = {0};
        int iOkNum = pCells->exportAllValue(buffers2Dim);
        ASSERT_AND_LOG(pCells->m_rows==4);
        ASSERT_AND_LOG(pCells->m_cols==5);
        ASSERT_AND_LOG(iOkNum==(pCells->m_rows*pCells->m_cols-2)); // two string

        pCells = pActiveSheet->getValue_Cells(L"D8:H8");
        ASSERT_AND_LOG(pCells);
        double buffers1Dim[5] = {0};
        iOkNum = pCells->exportAllValue<true>(buffers1Dim);
        ASSERT_AND_LOG(pCells->m_rows==1);
        ASSERT_AND_LOG(pCells->m_cols==5);
        ASSERT_AND_LOG(iOkNum==(pCells->m_cols-1)); // two string

        TpCExcelCells tmp = pCells->clone();
        ASSERT_AND_LOG(tmp->m_rows==pCells->m_rows);
        ASSERT_AND_LOG(tmp->m_cols==pCells->m_cols);

        rExcel.close(pBook,false);
    }

#ifdef RUN_EXAMPLE_OLEEXCEL
    InitRunFunc(TestCase_OleExcelGeneral);
    InitRunFunc(TestCase_OleExcelClass_Read);
    InitRunFunc(TestCase_OleExcelClass_Write);
#else //else of RUN_EXAMPLE_OLEEXCEL

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    #if defined(RUN_WARNING_NO_TESTCASE_RUN)
    GLOBALVAR(RUN_OLEEXCEL)=(outputTxtV(TXT("[Unit test run disabled] OleExcel.h\n%s(%d)\n"),TXT(__FILE__),__LINE__),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_RUN)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is runing for \"OleExcel.h\".")
    #endif
#endif  	// RUN_EXAMPLE_OLEEXCEL
}

#else //else of COMPILE_EXAMPLE_OLEEXCEL
    #if defined(RUN_WARNING_NO_TESTCASE_COMPILE)
    GLOBALVAR(COMPILE_OLEEXCEL)=(outputTxtV(TXT("[Unit test compile disabled] OleExcel.h\n%s(%d)\n"),TXT(__FILE__),__LINE__),1);
    #endif

    #if defined(BUILD_WARNING_NO_TESTCASE_COMPILE)
    #pragma message (__FILE__ "(" MAKESTRA(__LINE__) "):No test code is compiled for \"OleExcel.h\".")
    #endif
#endif // COMPILE_EXAMPLE_OLEEXCEL


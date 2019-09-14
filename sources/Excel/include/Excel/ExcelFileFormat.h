#ifndef __EXCELFILEFORMAT_H__
#define __EXCELFILEFORMAT_H__
/************************************************************************************
	author:		Shen Tony,2015-09-01
	purpose:	excel file format, xlWorkbookNormal is normal .xls format
*************************************************************************************/

//reference :ms-help://MS.MSDNQTR.v90.en/offioxl11ref/html/T_Microsoft_Office_Interop_Excel_XlFileFormat.htm
enum XlFileFormat
{
    xlAddIn = 18,                       //Microsoft Office Excel Add-In. 
    xlCSV = 6,                          //Comma separated value. 
    xlCSVMac = 22,                      //Comma separated value. 
    xlCSVMSDOS = 24,                    //Comma separated value. 
    xlCSVWindows = 23,                  //Comma separated value. 
    xlDBF2 = 7,                         //Dbase 2 format. 
    xlDBF3 = 8,                         //Dbase 3 format. 
    xlDBF4 = 11,                        //Dbase 4 format. 
    xlDIF = 9,                          //Data Interchange format. 
    xlExcel2 = 16,                      //Excel version 2.0. 
    xlExcel2FarEast = 27,               //Excel version 2.0 far east. 
    xlExcel3 = 29,                      //Excel version 3.0. 
    xlExcel4 = 33,                      //Excel version 4.0. 
    xlExcel5 = 39,                      //Excel version 5.0. 
    xlExcel7 = 39,                      //Excel 95. 
    xlExcel9795 = 43,                   //Excel version 95 and 97. 
    xlExcel4Workbook = 35,              //Excel version 4.0. Workbook format. 
    xlIntlAddIn = 26,                   //Microsoft Office Excel Add-In international format. 
    xlIntlMacro = 25,                   //Deprecated format. 
    xlWorkbookNormal = -4143,           //Excel workbook format. (.xls)
    xlSYLK = 2,                         //Symbolic link format. 
    xlTemplate = 17,                    //Excel template format. 
    xlCurrentPlatformText = -4158,      //Specifies a type of text format 
    xlTextMac = 19,                     //Specifies a type of text format. 
    xlTextMSDOS = 21,                   //Specifies a type of text format. 
    xlTextPrinter = 36,                 //Specifies a type of text format. 
    xlTextWindows = 20,                 //Specifies a type of text format. 
    xlWJ2WD1 = 14,                      //Deprecated format.
    xlWK1 = 5,                          //Lotus 1-2-3 format. 
    xlWK1ALL = 31,                      //Lotus 1-2-3 format.
    xlWK1FMT = 30,                      //Lotus 1-2-3 format.
    xlWK3 = 15,                         //Lotus 1-2-3 format.
    xlWK4 = 38,                         //Lotus 1-2-3 format.
    xlWK3FM3 = 32,                      //Lotus 1-2-3 format. 
    xlWKS = 4,                          //Lotus 1-2-3 format.
    xlWorks2FarEast = 28,               //Microsoft Works 2.0 format
    xlWQ1 = 34,                         //Quattro Pro format.
    xlWJ3 = 40,                         //Deprecated format.
    xlWJ3FJ3 = 41,                      //Deprecated format.
    xlUnicodeText = 42,                 //Specifies a type of text format. 
    xlHtml = 44,                        //Web page format.
    xlWebArchive = 45,                  //MHT format.
    xlXMLSpreadsheet = 46,              //Excel Spreadsheet format.
    xlExcel12 = 50,
    xlOpenXMLWorkbook = 51,
    xlOpenXMLWorkbookMacroEnabled = 52,
    xlOpenXMLTemplateMacroEnabled = 53,
    xlTemplate8 = 17,
    xlOpenXMLTemplate = 54,
    xlAddIn8 = 18,
    xlOpenXMLAddIn = 55,
    xlExcel8 = 56,
    xlOpenDocumentSpreadsheet = 60,
    xlWorkbookDefault = 51
};

#endif // __EXCELFILEFORMAT_H__

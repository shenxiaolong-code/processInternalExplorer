#ifndef __EXCELENVDEF_H__
#define __EXCELENVDEF_H__

/************************************************************************************
	author:		Shen Tony,2015-08-18
	purpose:	Ole Excel needs windows PC environment
*************************************************************************************/
#if defined(_MSC_VER) && (defined(_X86_) || defined(_M_IX86))

#define EXCEL_SUPPORT                   1
#define _CRT_SECURE_NO_WARNINGS

#endif   //define windows x86 platform


#endif // __EXCELENVDEF_H__

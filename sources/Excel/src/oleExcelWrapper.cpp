#include <Excel/ExcelDataType.h>
#ifdef EXCEL_SUPPORT

#include <ole2.h> // OLE2 Definitions
#pragma warning(disable:4996)

// AutoWrap() - Automation helper function...
/*
The AutoWrap() function simplifies most of the low-level details involved with using IDispatch directly. 
Feel free to use it in your own implementations.
reference : ..\VC\atlmfc\src\mfc\oledisp2.cpp
One caveat is that if you pass multiple parameters, they need to be passed in reverse-order. For example:
VARIANT parm[3];
parm[0].vt = VT_I4; parm[0].lVal = 1;
parm[1].vt = VT_I4; parm[1].lVal = 2;
parm[2].vt = VT_I4; parm[2].lVal = 3;
AutoWrap(DISPATCH_METHOD, NULL, pDisp, L"call", 3, parm[2], parm[1], parm[0]);
*/
HRESULT AutoWrap(int autoType, VARIANT *pvResult, IDispatch *pDisp, LPOLESTR ptName, int cArgs...) 
{
    //verify params
    if(!pDisp) 
    {
        MessageBoxA(NULL, "NULL IDispatch passed to AutoWrap()", "Error", MB_SETFOREGROUND);
        _exit(0);
    }

    // Begin variable-argument list...
    //cArgs : number of various arguments
    va_list marker;
    va_start(marker, cArgs);

    // Convert wchar down to ANSI to be used in display info
    //char szName[200];
    WCHAR* szName = ptName;
    // WideCharToMultiByte(CP_ACP, 0, ptName, -1, szName, 256, NULL, NULL);

    HRESULT hr;
    DISPID dispID;
    WCHAR buf[200]={0};
    // Get DISPID for name passed...
    hr = pDisp->GetIDsOfNames(IID_NULL, &ptName, 1, LOCALE_USER_DEFAULT, &dispID);
    if(FAILED(hr)) 
    {
        wsprintf(buf, L"IDispatch::GetIDsOfNames(\"%s\") failed w/err 0x%08lx", szName, hr);
        MessageBoxW(NULL, buf, L"AutoWrap()", MB_SETFOREGROUND);
        //_exit(0);
        return hr;
    }

    // Allocate memory for arguments...
    VARIANT *pArgs = new VARIANT[cArgs+1];
    // Extract arguments...
    for(int i=0; i<cArgs; i++) {
        pArgs[i] = va_arg(marker, VARIANT);
    }

    // Build DISPPARAMS
    DISPPARAMS dp = { NULL, NULL, 0, 0 };
    dp.cArgs = cArgs;
    dp.rgvarg = pArgs;

    // Handle special-case for property-puts!
    DISPID dispidNamed = DISPID_PROPERTYPUT;
    if(autoType & DISPATCH_PROPERTYPUT) {
        dp.cNamedArgs = 1;
        dp.rgdispidNamedArgs = &dispidNamed;
    }

    // Make the call!
    hr = pDisp->Invoke(dispID, IID_NULL, LOCALE_SYSTEM_DEFAULT, autoType, &dp, pvResult, NULL, NULL);
    if(FAILED(hr)) {
        wsprintf(buf, L"IDispatch::Invoke(\"%s\"=%08lx) failed w/err 0x%08lx", szName, dispID, hr);
        MessageBoxW(NULL, buf, L"AutoWrap()", MB_SETFOREGROUND);
        //_exit(0);
        return hr;
    }
    // End variable-argument section...
    va_end(marker);

    delete [] pArgs;

    return hr;
}
#endif // EXCEL_SUPPORT

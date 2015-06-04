// test_setuper.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

void ShowHelp();

int _tmain(int argc, _TCHAR* argv[])
{
    if(argc < 2)
    {
        ShowHelp();
        return 0;
    }
    const TCHAR szParam[] = _T("/callwnd:");
    LPCTSTR szCmdline = ::GetCommandLine();
    LPCTSTR hwndParam = _tcsstr(szCmdline, szParam);
    if(hwndParam == NULL)
    {
        ShowHelp();
        return 0;
    }
    HWND hWnd = (HWND)_ttoi(hwndParam + _countof(szParam) - 1);
    if(hWnd == NULL || !::IsWindow(hWnd))
    {
        ShowHelp();
        return 0;
    }
    TCHAR szText[100];
    int nProgress = 0;
    COPYDATASTRUCT cds = {0};
    for(int i=0; i<100; ++ i)
    {
        _sntprintf(szText, _countof(szText), _T("Current Progress is %d%%"), i);
        // Send text
        cds.dwData = 0;
        cds.cbData = sizeof(TCHAR) * (_tcslen(szText) + 1);
        cds.lpData = szText;
        wprintf(L"Sending Text: %s\r\n", szText);
        ::SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM)&cds);
        // Send progress
        cds.dwData = 1;
        cds.cbData = sizeof(int);
        nProgress = 300 * i;
        cds.lpData = &nProgress;
        wprintf(L"Sending Progress: %d\r\n", nProgress);
        ::SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM)&cds);
        if(i % 8 == 0)
            ::Sleep(500);
    }
	return 0;
}

void ShowHelp()
{
    printf("test_setuper /callwnd:<hwnd>\r\n");
}

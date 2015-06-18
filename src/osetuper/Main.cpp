// Main.cpp : Defines the entry point for the application.
//

#include "stdafx.h"


#ifdef ID_FILE_SAVE
#undef ID_FILE_SAVE
#endif

#ifdef IDC_STATIC
#undef IDC_STATIC
#endif


#include "Util.h"
#include "resource.h"
#include "LinkStatic.h"
#include "BaseDlg.h"
#include "MainDlg.h"
#include "CrashDaemon.h"


CAppModule _Module;


int Run(LPTSTR /*lpCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
    CMessageLoop theLoop;
    _Module.AddMessageLoop(&theLoop);

    CMainDlg dlgMain;

    if(dlgMain.Create(NULL) == NULL)
    {
        ATLTRACE(_T("Main dialog creation failed!\n"));
        return 0;
    }

    dlgMain.ShowWindow(nCmdShow);

    int nRet = theLoop.Run();

    if(dlgMain.IsWindow())
        dlgMain.DestroyWindow();

    _Module.RemoveMessageLoop();
    return nRet;
}


void WeAreCrashed(IN LPCTSTR szFilePath)
{
    CString strMsg;
    strMsg.Format(_T("Woops, I'm crashed, send file %s to developer please"), szFilePath);
    ::MessageBox(NULL, strMsg, _T("Crash Tip"), MB_OK | MB_ICONERROR);
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int nCmdShow)
{
#if (_WIN32_IE >= 0x0300)
    INITCOMMONCONTROLSEX iccx;
    iccx.dwSize = sizeof(iccx);
    iccx.dwICC = ICC_BAR_CLASSES;	// change to support other controls
    ::InitCommonControlsEx(&iccx);
#else
    ::InitCommonControls();
#endif

    CrashDaemon::StartCrashClient(NULL, WeAreCrashed);

    ULONG_PTR uToken = Util::InitGdiplus();

    _Module.Init(NULL, hInstance);

    int nRet = Run(lpCmdLine, nCmdShow);

    _Module.Term();
    Util::CleanGdiplus(uToken);
    return nRet;
}

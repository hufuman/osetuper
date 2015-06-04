#include "StdAfx.h"
#include "SetupWorker.h"


#include "Util.h"
#include "StringBundle.h"


/*
    wParam:
        0 - error
        1 - finish
    lParam:
        error: 0 - release file error
        error: 1 - create process error
*/
const UINT g_uNotifyMsg = ::RegisterWindowMessage(_T("OSETUPER_NOTIFY_MSG"));

CSetupWorker::CSetupWorker(void)
{
    m_pListener = NULL;
    m_hThread = NULL;
    m_bStop = FALSE;
    m_hMsgWnd = NULL;
}

CSetupWorker::~CSetupWorker(void)
{
}

void CSetupWorker::Start(ISetupStatusListener* listener, LPCTSTR szSetupPath)
{
    m_pListener = listener;
    m_bStop = FALSE;
    m_strSetupPath = szSetupPath;

    InitMsgWnd();

    m_pListener->OnProgressChanged(0);
    m_pListener->OnProgressTextChanged(_T("正在释放安装文件..."));
    m_hThread = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, &CSetupWorker::ThreadProc, reinterpret_cast<void*>(this), 0, 0));
}

UINT CSetupWorker::ThreadProc(void* pData)
{
    CSetupWorker* pThis = reinterpret_cast<CSetupWorker*>(pData);
    pThis->OnThread();
    return 0;
}

LRESULT CSetupWorker::MsgWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message == WM_COPYDATA)
    {
        CSetupWorker* pThis = reinterpret_cast<CSetupWorker*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if(pThis == NULL)
            return 0;
        COPYDATASTRUCT* pCopyDataStruct = reinterpret_cast<COPYDATASTRUCT*>(lParam);
        if(pCopyDataStruct->dwData == 0)
        {
            // Text
            TCHAR szText[MAX_PATH * 2];
            int nSize = min(MAX_PATH * 2, pCopyDataStruct->cbData / 2);
            if(nSize > 0)
            {
                _tcsncpy(szText, (LPCTSTR)pCopyDataStruct->lpData, nSize);
                szText[nSize - 1] = 0;
                CString strText(szText);
                int pos = strText.Find(_T("..."));
                if(pos > 0)
                    strText = strText.Mid(0, pos);
                pThis->m_pListener->OnProgressTextChanged(strText);
            }
        }
        else if(pCopyDataStruct->dwData == 1)
        {
            int n = 0;
            if(pCopyDataStruct->cbData == sizeof(int))
            {
                n = *((int*)pCopyDataStruct->lpData);
                int rate = n * 100 / 30000;
                if(rate > 100)
                    rate = 100;
                if(rate < 0)
                    rate = 0;
                pThis->m_pListener->OnProgressChanged(rate);
            }
        }
    }
    else if(g_uNotifyMsg == message)
    {
        CSetupWorker* pThis = reinterpret_cast<CSetupWorker*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if(pThis == NULL)
            return 0;
        switch(wParam)
        {
        case 0:
            {
                // Error
                if(lParam == 0)
                {
                    pThis->m_pListener->OnError(CStringBundle::GetInst().Get(_T("ERR_REALSE_FILE")));
                }
                else if(lParam == 1)
                {
                    pThis->m_pListener->OnError(CStringBundle::GetInst().Get(_T("ERR_CREATE_PROCESS")));
                }
                break;
            }
        case 1:
            {
                pThis->m_pListener->OnFinish();
                break;
            }
        }
    }

    return ::DefWindowProc(hWnd, message, wParam, lParam);
}

void CSetupWorker::OnThread()
{
    CString strExePath = ReleaseFile();
    if(strExePath.IsEmpty())
    {
        ::PostMessage(m_hMsgWnd, g_uNotifyMsg, 0, 0);
        return;
    }
    if(m_bStop)
        return;
    CString strCommand;
    strCommand.Format(_T("\"%s\" /callwnd:%d /S /D=%s"), (LPCTSTR)strExePath, m_hMsgWnd, (LPCTSTR)m_strSetupPath);
    PROCESS_INFORMATION pi;
    STARTUPINFO si = {sizeof(STARTUPINFO)};
    if(!::CreateProcess(NULL, strCommand.GetBuffer(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        ::PostMessage(m_hMsgWnd, g_uNotifyMsg, 0, 1);
        return;
    }
    for(;!m_bStop;)
    {
        DWORD dwWaitResult = ::WaitForSingleObject(pi.hProcess, 200);
        if(dwWaitResult == WAIT_OBJECT_0 || dwWaitResult != WAIT_TIMEOUT)
            break;
    }
    ::DeleteFile(strExePath);
    ::PostMessage(m_hMsgWnd, g_uNotifyMsg, 1, 0);
}

void CSetupWorker::InitMsgWnd()
{
    WNDCLASSEX wc = {sizeof(WNDCLASSEX)};
    wc.hInstance = ::GetModuleHandle(NULL);
    wc.lpfnWndProc = &CSetupWorker::MsgWndProc;
    wc.lpszClassName = _T("osetuper_msg_wnd_class");
    ::RegisterClassEx(&wc);

    m_hMsgWnd = ::CreateWindow(_T("osetuper_msg_wnd_class"), _T(""), WS_OVERLAPPED, 0, 0, 0, 0, HWND_MESSAGE, NULL, ::GetModuleHandle(NULL), 0);
    ::SetWindowLongPtr(m_hMsgWnd, GWLP_USERDATA, reinterpret_cast<LONG>(this));
}

CString CSetupWorker::ReleaseFile()
{
    CString strResult;

    // release file to userdata
    TCHAR szPath[1024];
    ::GetModuleFileName(NULL, szPath, 1024);
    CString strOutputPath = GetTempExePath();

    HANDLE hInFile = ::CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if(hInFile != INVALID_HANDLE_VALUE)
    {
        HANDLE hOutFile = ::CreateFile(strOutputPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
        if(hOutFile != INVALID_HANDLE_VALUE)
        {
            ::SetFilePointer(hInFile, Util::GetSelfSize(), NULL, FILE_BEGIN);
            BYTE byData[4096];
            DWORD dwRead;
            DWORD dwWritten;
            BOOL bResult = TRUE;
            while(bResult && ::ReadFile(hInFile, (LPVOID)byData, 4096, &dwRead, NULL) && dwRead > 0)
            {
                bResult = ::WriteFile(hOutFile, (LPCVOID)byData, dwRead, &dwWritten, NULL) && dwRead == dwWritten;
            }
            ::CloseHandle(hOutFile);
        }
        ::CloseHandle(hInFile);
    }
    return strOutputPath;
}

CString CSetupWorker::GetTempExePath()
{
    CString strResult;
    CString strAppName = CStringBundle::GetInst().Get(_T("APPNAME"));
    TCHAR szTemp[MAX_PATH + 1];
    ::SHGetSpecialFolderPath(m_hMsgWnd, szTemp, CSIDL_APPDATA, FALSE);
    strResult = Util::AppendPath(szTemp, _T("osetuper"));
    Util::MakeDirs(strResult);
    strResult = Util::AppendPath(szTemp, strAppName);
    Util::MakeDirs(strResult);
    srand((unsigned int)time(NULL));
    CString strTemp;
    strTemp.Format(_T("setup%d.exe"), rand());
    strResult = Util::AppendPath(strResult, strTemp);
    return strResult;
}

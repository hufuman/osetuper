#pragma once


class ISetupStatusListener
{
public:
    virtual ~ISetupStatusListener(){}

    virtual void OnProgressChanged(int progress) = 0;
    virtual void OnProgressTextChanged(LPCTSTR szText) = 0;
    virtual void OnError(CString strError) = 0;
    virtual void OnFinish() = 0;
};

class CSetupWorker
{
public:
    CSetupWorker(void);
    ~CSetupWorker(void);

    void Start(ISetupStatusListener* listener, LPCTSTR szSetupPath);

private:
    static UINT WINAPI ThreadProc(void* pData);
    static LRESULT WINAPI MsgWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void OnThread();
    void InitMsgWnd();
    CString ReleaseFile();
    CString GetTempExePath();

private:
    ISetupStatusListener* m_pListener;
    HANDLE m_hThread;
    volatile BOOL m_bStop;
    HWND m_hMsgWnd;
    CString m_strSetupPath;
};

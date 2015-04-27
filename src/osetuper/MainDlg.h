// maindlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////


#pragma once

#include "OControlManager.h"
#include "PageManager.h"
#include "StringBundle.h"


class CMainDlg : public BaseDlg<CMainDlg>, public CMessageFilter
{
public:
	enum { IDD = IDD_DIALOG_MAIN };


    CMainDlg() : m_PageManager(&m_ControlManager)
	{
    }

    ~CMainDlg()
    {
        m_hWnd = NULL;
    }

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return ::IsDialogMessage(m_hWnd, pMsg);
	}

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
        MESSAGE_HANDLER(WM_NCACTIVATE, OnNcActivate)
        MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
        MESSAGE_HANDLER(WM_NCCALCSIZE, OnNcCalcSize)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)

        COMMAND_ID_HANDLER(IDC_BTN_SYS_MIN, OnBtnSysMin)
        COMMAND_ID_HANDLER(IDC_BTN_SYS_CLOSE, OnBtnSysClose)

        COMMAND_ID_HANDLER(IDC_BTN_PREV_PAGE, OnBtnPrevPage)
        COMMAND_ID_HANDLER(IDC_BTN_NEXT_PAGE, OnBtnNextPage)
        COMMAND_ID_HANDLER(IDC_BTN_CANCEL, OnBtnCancel)
        COMMAND_ID_HANDLER(IDC_BTN_FINISH, OnBtnFinish)

        if(uMsg == 0xAE // WM_NCUAHDRAWCAPTION
            || uMsg == 0xAF) // WM_NCUAHDRAWFRAME
        {
            bHandled = TRUE;
            lResult = 0;
            return TRUE;
        }

        if(m_ControlManager.HandleMsg(uMsg, wParam, lParam, lResult))
        {
            return TRUE;
        }

        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    LRESULT OnBtnSysMin(UINT /*uCode*/, UINT /*uCommandId*/, HWND /*hWndControl*/, BOOL& /*bHandled*/)
    {
        ::ShowWindow(m_hWnd, SW_MINIMIZE);
        return 0;
    }

    LRESULT OnBtnPrevPage(UINT /*uCode*/, UINT /*uCommandId*/, HWND /*hWndControl*/, BOOL& /*bHandled*/)
    {
        m_PageManager.PrevPage();
        return 0;
    }


    LRESULT OnBtnNextPage(UINT /*uCode*/, UINT /*uCommandId*/, HWND /*hWndControl*/, BOOL& /*bHandled*/)
    {
        m_PageManager.NextPage();
        return 0;
    }


    LRESULT OnBtnCancel(UINT /*uCode*/, UINT /*uCommandId*/, HWND /*hWndControl*/, BOOL& /*bHandled*/)
    {
        DestroyWindow();
        return 0;
    }

    LRESULT OnBtnFinish(UINT /*uCode*/, UINT /*uCommandId*/, HWND /*hWndControl*/, BOOL& /*bHandled*/)
    {
        DestroyWindow();
        return 0;
    }
    LRESULT OnBtnSysClose(UINT /*uCode*/, UINT /*uCommandId*/, HWND /*hWndControl*/, BOOL& /*bHandled*/)
    {
        ::DestroyWindow(m_hWnd);
        return 0;
    }

    LRESULT OnNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = !::IsIconic(m_hWnd);
        return (wParam == 0) ? TRUE : FALSE;
    }

    LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 1;
    }

    LRESULT OnNcCalcSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 0;
    }

    LRESULT OnNcPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 0;
    }

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        PAINTSTRUCT ps;
        ::BeginPaint(m_hWnd, &ps);

        m_ControlManager.Draw(ps.hdc, ps.rcPaint);

        ::EndPaint(m_hWnd, &ps);
        return 0;
    }

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// center the dialog on the screen
		CenterWindow();

		// set icons
		HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
		SetIcon(hIcon, TRUE);
		HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
		SetIcon(hIconSmall, FALSE);

		// register object for message filtering
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		pLoop->AddMessageFilter(this);

        m_strAppName.LoadString(IDS_APP_NAME);

        CStringBundle::GetInst().Init();
        m_ControlManager.Init(m_hWnd);
        SetWindowPos(NULL, 0, 0, 520, 340, SWP_NOMOVE | SWP_NOZORDER);

        m_PageManager.ShowPage(CPageManager::PageWelcome);

        InitLayout();

        CenterWindow();

		return TRUE;
    }

    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        ::PostQuitMessage(0);
        bHandled = FALSE;
        return 0;
    }

    // Layout
    void InitLayout()
    {
        CRect rcMargin;

        rcMargin.SetRect(2, 0, 0, 2);
        m_ControlManager.CreateButton(_T("sysclosebutton"), IDC_BTN_SYS_CLOSE, ManagerLayout::Top | ManagerLayout::Right, 4, rcMargin);
    }

private:
    CString         m_strAppName;

    CPageManager    m_PageManager;
    OControlManager m_ControlManager;
};

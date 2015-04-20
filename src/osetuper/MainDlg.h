// maindlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////


#pragma once

#include "OControlManager.h"


class CMainDlg : public BaseDlg<CMainDlg>, public CMessageFilter
{
public:
	enum { IDD = IDD_DIALOG_MAIN };


	CMainDlg()
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
        m_ControlManager.Init(m_hWnd);

        CRect rcTemp(0, 0, 0, 0);

        OShape* pShape = m_ControlManager.CreateShape(RGB(255, 255, 0), ManagerLayout::VFill | ManagerLayout::HFill, rcTemp);
        pShape = pShape;

        OButton* pBtnClose = m_ControlManager.CreateButton(_T("sysclosebutton"), IDC_BTN_SYS_CLOSE, ManagerLayout::Top | ManagerLayout::Right, 4, rcTemp);
        rcTemp = pBtnClose->GetRect();
        rcTemp.top = 0;
        rcTemp.right = rcTemp.Width();
        m_ControlManager.CreateButton(_T("sysminbutton"), IDC_BTN_SYS_MIN, ManagerLayout::Top | ManagerLayout::Right, 3, rcTemp);

        rcTemp.SetRect(0, 0, 0, 0);
        OCheckBox* pChkBox = m_ControlManager.CreateCheckBox(_T("checkbutton"), 0, ManagerLayout::Bottom | ManagerLayout::Left, rcTemp);
        pChkBox->SetText(_T("asdfadsfdsaf"));

        rcTemp.SetRect(20, 20, 20, 20);
        m_ControlManager.CreateEdit(_T("E:\\Test\\Text"), ManagerLayout::Bottom | ManagerLayout::HFill, rcTemp);

        rcTemp.SetRect(20, 20, 20, 20);
        m_ControlManager.CreateLink(_T("E:\\Test\\Text"), ManagerLayout::Top | ManagerLayout::HFill, rcTemp);
    }

private:
    CString         m_strAppName;

    OControlManager m_ControlManager;
};

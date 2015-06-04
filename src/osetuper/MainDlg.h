// maindlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////


#pragma once

#include "OControlManager.h"
#include "PageManager.h"
#include "StringBundle.h"
#include "SetupData.h"

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

        if(m_PageManager.HandleMsg(uMsg, wParam, lParam, lResult))
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
        FinishSetup(CSetupData::GetInst().GetStart());

        DestroyWindow();
        return 0;
    }
    LRESULT OnBtnSysClose(UINT /*uCode*/, UINT /*uCommandId*/, HWND /*hWndControl*/, BOOL& /*bHandled*/)
    {
        if(m_PageManager.GetCurPageIndex() == CPageManager::PageFinish)
        {
            FinishSetup(FALSE);
        }
        else
        {
            CString strMsg = CStringBundle::GetInst().Get(_T("TEXT_CONFIRM_QUIT"));
            if(MsgBox(strMsg, MB_OKCANCEL | MB_ICONINFORMATION) != IDOK)
                return 0;
        }
        ::DestroyWindow(m_hWnd);
        return 0;
    }

    void FinishSetup(BOOL bStart)
    {
        CString strCmd = CStringBundle::GetInst().Get(bStart ? _T("COMMAND_START") : _T("COMMAND_NOT_START"));
        if(!strCmd.IsEmpty())
        {
            CString strCommand = strCmd;
            strCommand.Replace(_T("$INSTDIR"), CSetupData::GetInst().GetSetupDir());
            ::ShellExecute(m_hWnd, _T("open"), strCommand, NULL, NULL, SW_SHOW);
        }
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

        CStringBundle::GetInst().Init();
        m_ControlManager.Init(m_hWnd);
        SetWindowPos(NULL, 0, 0, 450, 640, SWP_NOMOVE | SWP_NOZORDER);

        CString strTitle = CStringBundle::GetInst().Get(_T("TITLE"));
        SetWindowText(strTitle);

        InitLayout();

        m_PageManager.ShowPage(CPageManager::PageWelcome);

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
        CRect rcTemp(0, 0, 0, 0);
        m_ControlManager.CreateShape(RGB(45, 45, 45), ManagerLayout::HFill | ManagerLayout::VFill, rcTemp);

        OShape* tmpShape = m_ControlManager.CreateShape(RGB(34, 36, 33), ManagerLayout::HFill | ManagerLayout::Bottom, rcTemp);
        rcTemp.bottom = 115;
        tmpShape->SetRect(rcTemp);
        tmpShape->AutoSize();

        rcTemp.SetRect(CONTROL_LEFT, 100, 0, 0);
        m_ControlManager.CreateImage(_T("Logo"), ManagerLayout::Top | ManagerLayout::Left, rcTemp);

        rcTemp.SetRect(2, 0, 0, 2);
        m_ControlManager.CreateButton(_T("sysclosebutton"), IDC_BTN_SYS_CLOSE, ManagerLayout::Top | ManagerLayout::Right, 4, rcTemp);

        rcTemp.SetRect(0, 0, 0, 0);
        m_ControlManager.CreateButton(_T("sysclosebutton"), IDC_BTN_SYS_CLOSE, ManagerLayout::Top | ManagerLayout::Right, 4, rcTemp);
    }

private:
    CPageManager    m_PageManager;
    OControlManager m_ControlManager;
};

#include "stdafx.h"
#include "InstallPage.h"

#include "resource.h"
#include "StringBundle.h"
#include "SetupData.h"


CInstallPage::CInstallPage(OControlManager* pManager)
: CBasePage(pManager)
{
    m_dwProgress = 0;
}

CInstallPage::~CInstallPage(void)
{
    ;
}

void CInstallPage::Create()
{
    CRect rcTemp(CONTROL_LEFT, 200, CONTROL_LEFT, 0);
    m_pText = m_pControlManager->CreateLabel(_T("TEXT_PREINST"), ManagerLayout::HFill | ManagerLayout::Top, rcTemp);
    m_pText->SetTextAlign(TextAlign::AlignVCenter);
    AddControl(m_pText);
    m_pText->SetAutoSize(FALSE);
    m_strText = m_pText->GetText();

    rcTemp.SetRect(CONTROL_LEFT, 225, CONTROL_LEFT, 0);
    m_pProgressBack = m_pControlManager->CreateShape(RGB(19, 76, 112), ManagerLayout::HFill | ManagerLayout::Top, rcTemp);
    AddControl(m_pProgressBack);
    rcTemp = m_pProgressBack->GetRect();
    rcTemp.bottom = rcTemp.top + 10;
    m_pProgressBack->SetRect(rcTemp);

    rcTemp.SetRect(CONTROL_LEFT, 225, CONTROL_LEFT, 0);
    m_pProgressFore = m_pControlManager->CreateShape(RGB(0, 149, 249), ManagerLayout::Left | ManagerLayout::Top, rcTemp);
    AddControl(m_pProgressFore);
    rcTemp = m_pProgressFore->GetRect();
    rcTemp.right = rcTemp.left + 2;
    rcTemp.bottom = rcTemp.top + 10;
    m_pProgressFore->SetRect(rcTemp);

    m_Worker.Start(this, CSetupData::GetInst().GetSetupDir());
}

// ISetupStatusListener
void CInstallPage::OnProgressChanged(int progress)
{
    m_dwProgress = progress;
    CString strText;
    strText.Format(_T("%d\r\n"), progress);
    ::OutputDebugString(strText);
    UpdateProgressUI();
}

void CInstallPage::OnProgressTextChanged(LPCTSTR szText)
{
    m_strText = szText;
    UpdateProgressUI();
}

void CInstallPage::OnError(CString strError)
{
    ::MessageBox(m_pControlManager->GetWindow(), strError, CStringBundle::GetInst().Get(_T("TITLE")), MB_OK | MB_ICONWARNING);
    ::PostQuitMessage(0);
}

void CInstallPage::OnFinish()
{
    ::PostMessage(m_pControlManager->GetWindow(), WM_COMMAND, MAKEWPARAM(IDC_BTN_NEXT_PAGE, BN_CLICKED), 0);
}

void CInstallPage::UpdateProgressUI()
{
    CString strText;
    strText.Format(_T("[%d%%] %s"), m_dwProgress, (LPCTSTR)m_strText);
    m_pText->SetText(strText);

    CRect rc = m_pText->GetRect();
    CString str;
    str.Format(_T("%d, %d, %d, %d\r\n"), rc.left, rc.top, rc.right, rc.bottom);
    ::OutputDebugString(str);

    CRect rcTemp = m_pProgressBack->GetRect();
    int width = rcTemp.Width() * m_dwProgress / 100;
    rcTemp = m_pProgressFore->GetRect();
    rcTemp.right = rcTemp.left + width;
    m_pProgressFore->SetRect(rcTemp);
}

#include "stdafx.h"
#include "WelcomePage.h"

#include "resource.h"
#include "StringBundle.h"


CWelcomePage::CWelcomePage(OControlManager* pManager)
: CBasePage(pManager)
{
}

CWelcomePage::~CWelcomePage(void)
{
    ;
}

void CWelcomePage::Create()
{
    CRect rcTemp(0, 0, 0, 0);
    m_pControlManager->CreateShape(RGB(255, 0, 0), ManagerLayout::HFill | ManagerLayout::VFill, rcTemp);

    CRect rcMargin;

    // Title
    rcMargin.SetRect(11, 14, 0, 0);
    AddControl(m_pControlManager->CreateLabel(CStringBundle::GetInst().Get(_T("TITLE")), ManagerLayout::Top | ManagerLayout::Left, rcMargin));

    // Install Now
    rcMargin.SetRect(0, 230, 0, 0);
    OShape* pInstallNow = m_pControlManager->CreateShape(RGB(255, 255, 255), ManagerLayout::Top | ManagerLayout::HCenter, rcMargin);
    pInstallNow->SetMinSize(214, 40);
    pInstallNow->SetBorder(1);
    pInstallNow->SetTextAttr(CStringBundle::GetInst().Get(_T("WELCOME_INSTALL")), TRUE);
    AddControl(pInstallNow);

    // License
    rcMargin.SetRect(17, 312, 0, 0);
    OCheckBox* pChkLicense = m_pControlManager->CreateCheckBox(_T("checkbutton"), IDC_CHK_AGREE, ManagerLayout::Top | ManagerLayout::Left, rcMargin);
    pChkLicense->SetTextAttr(CStringBundle::GetInst().Get(_T("WELCOME_LICENSE_CHECK")), TRUE);
    AddControl(pChkLicense);

    // Link to License
    rcMargin.SetRect(17 + pChkLicense->GetRect().Width() + 2, 317, 0, 0);
    OLink* pLinkLicense = m_pControlManager->CreateLink(CStringBundle::GetInst().Get(_T("WELCOME_LICENSE_LINK")), ManagerLayout::Top | ManagerLayout::Left, rcMargin);
    AddControl(pLinkLicense);

    // Options
    rcMargin.SetRect(0, 317, 30, 0);
    OLink* pOption = m_pControlManager->CreateLink(CStringBundle::GetInst().Get(_T("OPTION")), ManagerLayout::Top | ManagerLayout::Right, rcMargin);
    AddControl(pOption);
}

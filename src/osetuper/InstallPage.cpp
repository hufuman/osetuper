#include "stdafx.h"
#include "InstallPage.h"



CInstallPage::CInstallPage(OControlManager* pManager)
: CBasePage(pManager)
{
}

CInstallPage::~CInstallPage(void)
{
    ;
}

void CInstallPage::Create()
{
    CRect rcTemp(0, 0, 0, 0);
    OButton* pButton = m_pControlManager->CreateButton(_T("sysclosebutton"), 0, ManagerLayout::Top | ManagerLayout::Right, 4, rcTemp);
    pButton->SetText(_T("Install Page"));
    AddControl(pButton);
}

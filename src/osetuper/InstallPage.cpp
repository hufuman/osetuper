#include "stdafx.h"
#include "InstallPage.h"

#include "resource.h"


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
    m_pControlManager->CreateShape(RGB(0, 255, 0), ManagerLayout::HFill | ManagerLayout::VFill, rcTemp);

    OButton* pButton = m_pControlManager->CreateButton(_T("pushbutton"), IDC_BTN_NEXT_PAGE, ManagerLayout::Bottom | ManagerLayout::Right, 4, rcTemp);
    pButton->SetText(_T("Install Page"));
    AddControl(pButton);
}

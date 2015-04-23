#include "stdafx.h"
#include "WelcomePage.h"



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
    OButton* pButton = m_pControlManager->CreateButton(_T("sysclosebutton"), 0, ManagerLayout::Top | ManagerLayout::Right, 4, rcTemp);
    pButton->SetText(_T("Create Page"));
    AddControl(pButton);
}

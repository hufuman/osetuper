#include "stdafx.h"
#include "FinishPage.h"



CFinishPage::CFinishPage(OControlManager* pManager)
: CBasePage(pManager)
{
}

CFinishPage::~CFinishPage(void)
{
    ;
}

void CFinishPage::Create()
{
    CRect rcTemp(0, 0, 0, 0);
    OButton* pButton = m_pControlManager->CreateButton(_T("sysclosebutton"), 0, ManagerLayout::Top | ManagerLayout::Right, 4, rcTemp);
    pButton->SetText(_T("Finish Page"));
    AddControl(pButton);
}

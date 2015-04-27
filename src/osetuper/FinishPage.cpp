#include "stdafx.h"
#include "FinishPage.h"


#include "resource.h"

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
    m_pControlManager->CreateShape(RGB(0, 0, 255), ManagerLayout::HFill | ManagerLayout::VFill, rcTemp);

    OButton* pButton = m_pControlManager->CreateButton(_T("pushbutton"), IDC_BTN_FINISH, ManagerLayout::Bottom | ManagerLayout::Right, 4, rcTemp);
    pButton->SetText(_T("Finish Page"));
    AddControl(pButton);
}

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
    CRect rcTemp(0, 0, 35, 20);
    OButton* pBtn = m_pControlManager->CreateButton(_T("pushbutton"), IDC_BTN_NEXT_PAGE, ManagerLayout::Right | ManagerLayout::Bottom, 4, rcTemp);
    pBtn->SetTextAttr(_T("BTN_NEXT"), FALSE);
    AddControl(pBtn);
}

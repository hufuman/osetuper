#include "stdafx.h"
#include "FinishPage.h"


#include "resource.h"
#include "SetupData.h"


CFinishPage::CFinishPage(OControlManager* pManager)
: CBasePage(pManager)
{
    m_pChkStart = NULL;
}

CFinishPage::~CFinishPage(void)
{
    ;
}

void CFinishPage::Create()
{
    // 
    CRect rcTemp(CONTROL_LEFT, 250, CONTROL_LEFT, 0);
    OLabel* pText = m_pControlManager->CreateLabel(_T("TEXT_FINISH_MSG"), ManagerLayout::HFill | ManagerLayout::Top, rcTemp);
    pText->SetTextAlign(TextAlign::AlignVCenter);
    AddControl(pText);

    // Start program
    rcTemp.SetRect(CONTROL_LEFT - 2, 280, 0, 0);
    m_pChkStart = m_pControlManager->CreateCheckBox(_T("checkbox"), IDC_BTN_START, ManagerLayout::Left | ManagerLayout::Top, rcTemp);
    m_pChkStart->SetTextAttr(_T("CHK_START_PROG"), TRUE);
    AddControl(m_pChkStart);

    // Finish
    rcTemp.SetRect(0, 0, 45, 40);
    OButton* pBtn = m_pControlManager->CreateButton(_T("pushbutton"), IDC_BTN_FINISH, ManagerLayout::Right | ManagerLayout::Bottom, 4, rcTemp);
    pBtn->SetTextAttr(_T("BTN_FINISH"), FALSE);
    AddControl(pBtn);
}

void CFinishPage::Show(BOOL bShow)
{
    __super::Show(bShow);
    CSetupData::GetInst().SetStart(m_pChkStart->IsChecked());
}

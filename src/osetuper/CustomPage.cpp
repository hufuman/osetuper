#include "stdafx.h"
#include "CustomPage.h"

#include "resource.h"
#include "StringBundle.h"
#include "Util.h"
#include "OGdiObjManager.h"
#include "SetupData.h"


CCustomPage::CCustomPage(OControlManager* pManager)
: CBasePage(pManager)
{
}

CCustomPage::~CCustomPage(void)
{
    ;
}

void CCustomPage::Create()
{
    CRect rcTemp;

    // Title
    rcTemp.SetRect(CONTROL_LEFT, 200, 0, 0);
    m_pTitle = m_pControlManager->CreateLabel(_T("CUSTOM_TITLE"), ManagerLayout::Top | ManagerLayout::Left, rcTemp);
    AddControl(m_pTitle);

    // Path
    TCHAR szTemp[MAX_PATH + 1];
    ::SHGetSpecialFolderPath(m_pControlManager->GetWindow(), szTemp, CSIDL_PROGRAM_FILES, FALSE);
    CString strInstDir = Util::AppendPath(szTemp, _T("Soya"));

    // Edit
    rcTemp.SetRect(CONTROL_LEFT, 225, CONTROL_LEFT, 0);
    m_pPath = m_pControlManager->CreateEdit(strInstDir, ManagerLayout::Top | ManagerLayout::HFill, rcTemp);
    m_pPath->SetRect(rcTemp);
    m_pPath->SetBackColor(RGB(53, 52, 70));
    m_pPath->SetReadOnly(TRUE);
    AddControl(m_pPath);
    CSetupData::GetInst().SetSetupDir(strInstDir);

    // Browse
    rcTemp.SetRect(0, 225, 0, 0);
    OButton* pBtnBrowse = m_pControlManager->CreateButton(_T("pushbutton"), IDC_BTN_BROWSE, ManagerLayout::Right | ManagerLayout::Top, 4, rcTemp);
    pBtnBrowse->SetTextAttr(_T("BTN_BROWSE"), FALSE);
    rcTemp = pBtnBrowse->GetRect();
    rcTemp.left = rcTemp.right - 30;
    rcTemp.bottom = rcTemp.top + Util::GetTextHeight(m_pControlManager->GetWindow(), OGdiObjManager::GetInst().GetFont(14, false));
    rcTemp.OffsetRect(-20, 0);
    pBtnBrowse->SetRect(rcTemp);
    AddControl(pBtnBrowse);

    // Agree
    rcTemp.SetRect(CONTROL_LEFT, 300, 0, 0);
    OCheckBox* pChkAgree = m_pControlManager->CreateCheckBox(_T("checkbox"), IDC_BTN_AGREE, ManagerLayout::Left | ManagerLayout::Top, rcTemp);
    pChkAgree->SetTextAttr(_T("BTN_AGREE"), TRUE);
    AddControl(pChkAgree);

    // License
    rcTemp.SetRect(CONTROL_LEFT + pChkAgree->GetRect().Width() + 2, 300 + 4, 0, 0);
    OLink* pLinkAgree = m_pControlManager->CreateLink(IDC_LINK_LICENSE, _T("LINK_LICENSE"), ManagerLayout::Left | ManagerLayout::Top, rcTemp);
    AddControl(pLinkAgree);

    // Next
    rcTemp.SetRect(0, 0, 35, 20);
    OButton* pBtn = m_pControlManager->CreateButton(_T("pushbutton"), IDC_BTN_NEXT_PAGE, ManagerLayout::Right | ManagerLayout::Bottom, 4, rcTemp);
    pBtn->SetTextAttr(_T("BTN_NEXT"), FALSE);
    AddControl(pBtn);
}

BOOL CCustomPage::OnCommand(WORD command)
{
    if(command == IDC_LINK_LICENSE)
    {
        ::ShellExecute(m_pControlManager->GetWindow(), _T("open"), CStringBundle::GetInst().Get(_T("URL_LICENSE")), NULL, NULL, SW_SHOW);
    }
    else if(command == IDC_BTN_BROWSE)
    {
        CString strInitDir = m_pPath->GetText();
        if(!Util::IsDirExists(strInitDir))
            strInitDir = Util::RemovePath(strInitDir);
        CString strPath = Util::BrowseForFolder(m_pControlManager->GetWindow(), _T("Select Title"), strInitDir);
        if(!strPath.IsEmpty())
        {
            strPath = Util::MakeSureEndWith(strPath, _T("Soya"));
            m_pPath->SetText(strPath);
            CSetupData::GetInst().SetSetupDir(strPath);
        }
    }
    return true;
}

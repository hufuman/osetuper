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

    // Next
    rcTemp.SetRect(0, 260, 0, 0);
    OButton* pBtn = m_pControlManager->CreateButton(_T("BtnInstall"), IDC_BTN_NEXT_PAGE, ManagerLayout::HCenter | ManagerLayout::Top, 3, rcTemp);
    pBtn->SetMinSize(132, 46);
    pBtn->SetTextAttr(_T("BTN_INSTALL"), FALSE);
    AddControl(pBtn);

    // bottom rect
    rcTemp.SetRect(1, 310, 1, 1);
    AddControl(m_pControlManager->CreateShape(RGB(248, 248, 248), ManagerLayout::HFill | ManagerLayout::VFill, rcTemp));

    rcTemp.SetRect(1, 312, 1, 67);
    AddControl(m_pControlManager->CreateShape(RGB(237, 237, 237), ManagerLayout::HFill | ManagerLayout::VFill, rcTemp));

    // Browse
    rcTemp.SetRect(6, 320, 0, 0);
    OButton* pBtnBrowse = m_pControlManager->CreateButton(_T("FOLDER"), IDC_BTN_BROWSE, ManagerLayout::Left | ManagerLayout::Top, 3, rcTemp);
    AddControl(pBtnBrowse);

    // Path
    TCHAR szTemp[MAX_PATH + 1];
    ::SHGetSpecialFolderPath(m_pControlManager->GetWindow(), szTemp, CSIDL_PROGRAM_FILES, FALSE);
    CString strInstDir = Util::AppendPath(szTemp, _T("Soya"));

    // Path Edit
    rcTemp.SetRect(36, 324, 40, 0);
    m_pPath = m_pControlManager->CreateEdit(strInstDir, ManagerLayout::Top | ManagerLayout::HFill, rcTemp);
    m_pPath->SetRect(rcTemp);
    m_pPath->SetBackColor(RGB(255, 255, 255));
    m_pPath->SetReadOnly(TRUE);
    AddControl(m_pPath);
    CSetupData::GetInst().SetSetupDir(strInstDir);

    // Agree
    rcTemp.SetRect(8, 347, 0, 0);
    m_pChkAgree = m_pControlManager->CreateCheckBox(_T("checkbox"), IDC_BTN_AGREE, ManagerLayout::Left | ManagerLayout::Top, rcTemp);
    m_pChkAgree->SetTextAttr(_T("BTN_AGREE"), TRUE);
    AddControl(m_pChkAgree);

    // License
    rcTemp.SetRect(8 + m_pChkAgree->GetRect().Width() + 2, 347 + 4, 0, 0);
    OLink* pLinkAgree = m_pControlManager->CreateLink(IDC_LINK_LICENSE, _T("LINK_LICENSE"), ManagerLayout::Left | ManagerLayout::Top, rcTemp);
    AddControl(pLinkAgree);
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

bool CCustomPage::CanShowNext() const
{
    if(m_pChkAgree->IsChecked())
        return true;
    CString strText = CStringBundle::GetInst().Get(_T("ERR_NOT_AGREE"));
    ::MessageBox(m_pControlManager->GetWindow(), strText, CStringBundle::GetInst().Get(_T("TITLE")), MB_OK | MB_ICONWARNING);
    return false;
}

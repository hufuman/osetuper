#include "stdafx.h"
#include "WelcomePage.h"

#include "resource.h"
#include "StringBundle.h"
#include "SysInfo.h"


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
    CRect rcTemp;

    // Sys Info
    CString strMsg = GetSysInfo();
    rcTemp.SetRect(0, 0, 0, 0);
    OLabel* pText = m_pControlManager->CreateLabel(_T("TEXT_DEFAULT_FONT"), ManagerLayout::Left | ManagerLayout::Top, rcTemp);
    pText->SetAutoSize(false);
    rcTemp.SetRect(106, 180, 605, 240);
    pText->SetRect(rcTemp);
    pText->SetMultiLine(true);
    pText->SetText(strMsg);
    AddControl(pText);

    // Next
    rcTemp.SetRect(0, 260, 0, 0);
    OButton* pBtn = m_pControlManager->CreateButton(_T("BtnInstall"), IDC_BTN_NEXT_PAGE, ManagerLayout::HCenter | ManagerLayout::Top, 3, rcTemp);
    pBtn->SetMinSize(132, 46);
    pBtn->SetTextAttr(_T("BTN_NEXT"), FALSE);
    AddControl(pBtn);
}

CString CWelcomePage::GetSysInfo() const
{
    CString strMsg;
    CSysInfo sysInfo;
    TCHAR szTemp[1024];

    strMsg += _T("CPU：\t");
    strMsg += sysInfo.GetCpuInfo().strName.c_str();
    strMsg += _T("\r\n");

    strMsg += _T("内存：\t");
    _sntprintf(szTemp, 1024, _T("%d G"), (int)((float)sysInfo.GetMemSize() / 1024));
    strMsg += szTemp;
    strMsg += _T("\r\n");
    /*
    strMsg += _T("分辨率：\t");
    DWORD dwWidth, dwHeight;
    sysInfo.GetScreenSize(dwWidth, dwHeight);
    _sntprintf(szTemp, 1024, _T("%d * %d"), dwWidth, dwHeight);
    strMsg += szTemp;
    strMsg += _T("\r\n");
    */
    strMsg += _T("系统：\t");
    strMsg += sysInfo.GetOsVersion().c_str();
    strMsg += sysInfo.IsSys64() ? _T(" （64位）") : _T(" （32位）");
    strMsg += _T("\r\n");

    const std::vector<CVideoCardInfo>& vctVideoCards = sysInfo.GetVideoCardInfos();
    if(vctVideoCards.size() > 0)
    {
        strMsg += _T("显卡：");
        for(size_t i=0; i<vctVideoCards.size(); ++ i)
        {
            strMsg += _T("\t");
            strMsg += vctVideoCards[i].strName.c_str();
            if(vctVideoCards[i].dwMemSize > 100)
            {
                _sntprintf(szTemp, 1024, _T(" - %d M"), vctVideoCards[i].dwMemSize);
                strMsg += szTemp;
            }
            strMsg += _T("\r\n");
        }
        strMsg += _T("\r\n");
    }

    return strMsg;
}

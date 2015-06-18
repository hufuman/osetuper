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
    CRect rcTemp(0, 0, 35, 20);
    OButton* pBtn = m_pControlManager->CreateButton(_T("pushbutton"), IDC_BTN_NEXT_PAGE, ManagerLayout::Right | ManagerLayout::Bottom, 4, rcTemp);
    pBtn->SetTextAttr(_T("BTN_NEXT"), FALSE);
    AddControl(pBtn);

    rcTemp.SetRect(0, 250, 0, 200);
    OShape* pRect = m_pControlManager->CreateShape(RGB(34, 36, 33), ManagerLayout::HFill | ManagerLayout::VFill, rcTemp);
    // pRect->SetColor(RGB(255, 0, 0));
    pRect->SetBorder(0);
    AddControl(pRect);

    rcTemp.SetRect(30, 220, 0, 0);
    OLabel* pTitle = m_pControlManager->CreateLabel(_T("TEXT_SYS_INFO"), ManagerLayout::Left | ManagerLayout::Top, rcTemp);
    AddControl(pTitle);

    CSysInfo sysInfo;
    std::wstring strMsg;
    TCHAR szTemp[1024];

    strMsg += _T("CPU：\r\n\t");
    strMsg += sysInfo.GetCpuInfo().strName;
    strMsg += _T("\r\n");

    strMsg += _T("内存：\r\n\t");
    _sntprintf(szTemp, 1024, _T("%d G"), (int)((float)sysInfo.GetMemSize() / 1024));
    strMsg += szTemp;
    strMsg += _T("\r\n");
    /*
    strMsg += _T("分辨率：\r\n\t");
    DWORD dwWidth, dwHeight;
    sysInfo.GetScreenSize(dwWidth, dwHeight);
    _sntprintf(szTemp, 1024, _T("%d * %d"), dwWidth, dwHeight);
    strMsg += szTemp;
    strMsg += _T("\r\n");
    */
    strMsg += _T("系统：\r\n\t");
    strMsg += sysInfo.GetOsVersion();
    strMsg += sysInfo.IsSys64() ? _T(" （64位）") : _T(" （32位）");
    strMsg += _T("\r\n");

    const std::vector<CVideoCardInfo>& vctVideoCards = sysInfo.GetVideoCardInfos();
    if(vctVideoCards.size() > 0)
    {
        strMsg += _T("显卡：\r\n");
        for(size_t i=0; i<vctVideoCards.size(); ++ i)
        {
            strMsg += _T("\t");
            strMsg += vctVideoCards[i].strName;
            if(vctVideoCards[i].dwMemSize > 100)
            {
                _sntprintf(szTemp, 1024, _T(" - %d M"), vctVideoCards[i].dwMemSize);
                strMsg += szTemp;
            }
            strMsg += _T("\r\n");
        }
        strMsg += _T("\r\n");
    }

    rcTemp.SetRect(0, 0, 0, 0);
    OLabel* pText = m_pControlManager->CreateLabel(_T("TEXT_DEFAULT_FONT"), ManagerLayout::Left | ManagerLayout::Top, rcTemp);
    pText->SetAutoSize(false);
    pText->SetTextColor(RGB(156, 151, 140));
    rcTemp.SetRect(35, 275, 605, 525);
    pText->SetRect(rcTemp);
    pText->SetMultiLine(true);
    pText->SetText(strMsg.c_str());
    AddControl(pText);
}

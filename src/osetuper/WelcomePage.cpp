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

    CSysInfo sysInfo;
    std::wstring strMsg;
    TCHAR szTemp[1024];

    strMsg += _T("CPU：\r\n\t");
    strMsg += sysInfo.GetCpuInfo().strName;
    strMsg += _T("\r\n");

    strMsg += _T("内存：\r\n\t");
    _sntprintf(szTemp, 1024, _T("%d G"), sysInfo.GetMemSize() / 1024);
    strMsg += szTemp;
    strMsg += _T("\r\n");

    strMsg += _T("分辨率：\r\n\t");
    DWORD dwWidth, dwHeight;
    sysInfo.GetScreenSize(dwWidth, dwHeight);
    _sntprintf(szTemp, 1024, _T("%d * %d"), dwWidth, dwHeight);
    strMsg += szTemp;
    strMsg += _T("\r\n");

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
                _sntprintf(szTemp, 1024, _T("%d M"), vctVideoCards[i].dwMemSize);
                strMsg += szTemp;
            }
            strMsg += _T("\r\n");
        }
        strMsg += _T("\r\n");
    }

    rcTemp.SetRect(0, 0, 0, 0);
    OLabel* pText = m_pControlManager->CreateLabel(_T(""), ManagerLayout::Left | ManagerLayout::Top, rcTemp);
    pText->SetAutoSize(false);
    pText->SetTextColor(RGB(240, 240, 240));
    rcTemp.SetRect(35, 250, 605, 500);
    pText->SetRect(rcTemp);
    pText->SetMultiLine(true);
    pText->SetText(strMsg.c_str());
    AddControl(pText);
}

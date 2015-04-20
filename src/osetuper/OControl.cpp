#include "stdafx.h"
#include "OControl.h"


#include "Util.h"
#include "OControlManager.h"


OControl::OControl()
{
    m_hImage = NULL;
    m_nImageCount = 4;
    m_uLayout = 0;

    m_uCommandId = 0;
    m_pManager = NULL;
    m_Status = ControlStatus::StatusNormal;

    m_SizeOfImage.cx = m_SizeOfImage.cy = 0;
    m_Rect.SetRect(0, 0, 0, 0);
    m_Margin.SetRect(0, 0, 0, 0);
}

OControl::~OControl()
{
    if(m_hImage != NULL)
        ::DeleteObject(m_hImage);
}

BOOL OControl::Create(OControlManager* manager, LPCTSTR szResName, UINT uCommandId, UINT uLayout, int nImageCount)
{
    CRect rcMargin(0, 0, 0, 0);
    return Create(manager, szResName, uCommandId, uLayout, nImageCount, rcMargin);
}

BOOL OControl::Create(OControlManager* manager, LPCTSTR szResName, UINT uCommandId, UINT uLayout, int nImageCount, const CRect& rcMargin)
{
    m_pManager = manager;
    m_Margin = rcMargin;
    m_uLayout = uLayout;
    m_uCommandId = uCommandId;
    m_nImageCount = nImageCount;

    m_hImage = Util::LoadImageFromExeRes(szResName);
    if(m_hImage != NULL)
    {
        BITMAP bmp;
        ::GetObject(m_hImage, sizeof(bmp), &bmp);
        m_SizeOfImage.cx = bmp.bmWidth;
        m_SizeOfImage.cy = bmp.bmHeight;
        return TRUE;
    }
    return FALSE;
}

UINT OControl::GetLayout() const
{
    return m_uLayout;
}

CRect OControl::GetRect() const
{
    return m_Rect;
}

CRect OControl::GetMargin() const
{
    return m_Margin;
}

BOOL OControl::GetDown() const
{
    return (m_Status == ControlStatus::StatusDown);
}

ControlStatus::Status OControl::GetStatus() const
{
    return m_Status;
}

void OControl::SetHover(BOOL bHover, const CPoint& pt)
{
    if(Util::IsKeyPressed(VK_LBUTTON) && m_Rect.PtInRect(pt))
        m_Status = ControlStatus::StatusDown;
    else if(bHover)
        m_Status = ControlStatus::StatusHover;
    else
        m_Status = ControlStatus::StatusNormal;
    Invalidate();
}

void OControl::SetDown(BOOL bDown, const CPoint& pt)
{
    if(bDown)
    {
        m_Status = ControlStatus::StatusDown;
        ::SetCapture(m_pManager->GetWindow());
    }
    else
    {
        if(m_pManager->GetWindow() == ::GetCapture())
        {
            ::ReleaseCapture();
        }
        if(m_Rect.PtInRect(pt))
        {
            m_Status = ControlStatus::StatusHover;
            FireCommand(BN_CLICKED);
        }
        else
        {
            m_Status = ControlStatus::StatusNormal;
        }
    }
    Invalidate();
}

void OControl::SetRect(const CRect& rect)
{
    m_Rect = rect;
    Invalidate();
}

void OControl::Draw(HDC hDc) const
{
    UNREFERENCED_PARAMETER(hDc);
}

void OControl::Invalidate() const
{
    m_pManager->Invalidate(m_Rect);
}

CSize OControl::GetAutoSize()
{
    CSize size(m_SizeOfImage);
    size.cx /= m_nImageCount;
    return size;
}

void OControl::FireCommand(UINT uCommandCode)
{
    if(m_uCommandId == 0)
        return;
    ::PostMessage(m_pManager->GetWindow(), WM_COMMAND, MAKEWPARAM(m_uCommandId, uCommandCode), 0);
}

//////////////////////////////////////////////////////////////////////////

void OButton::Draw(HDC hDc) const
{
    CRect rcImage(0, 0, m_SizeOfImage.cx, m_SizeOfImage.cy);

    switch(m_Status)
    {
    case ControlStatus::StatusNormal:
        rcImage.TopLeft().SetPoint(0, 0);
        break;
    case ControlStatus::StatusDown:
        rcImage.TopLeft().SetPoint(m_SizeOfImage.cx / m_nImageCount * 2, 0);
        break;
    case ControlStatus::StatusHover:
        rcImage.TopLeft().SetPoint(m_SizeOfImage.cx / m_nImageCount, 0);
        break;
    case ControlStatus::StatusDisabled:
        rcImage.TopLeft().SetPoint(m_SizeOfImage.cx / m_nImageCount * 3, 0);
        break;
    }
    rcImage.BottomRight().SetPoint(rcImage.left + m_SizeOfImage.cx / m_nImageCount, rcImage.top + m_SizeOfImage.cy);
    Util::AlphaDraw(m_hImage, rcImage.left, rcImage.top, rcImage.Width(), rcImage.Height(), hDc, m_Rect.left, m_Rect.top, m_Rect.Width(), m_Rect.Height());
}

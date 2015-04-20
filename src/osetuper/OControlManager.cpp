#include "stdafx.h"
#include "OControlManager.h"

#include "Util.h"

OControlManager::OControlManager()
{
    m_hWnd = NULL;
    m_pHoverControl = NULL;
}

OControlManager::~OControlManager()
{
    ;
}

void OControlManager::Init(HWND hWnd)
{
    m_hWnd = hWnd;

    UpdateCorner();

    CRect rcClient;
    ::GetClientRect(m_hWnd, &rcClient);
    RelayoutControls(rcClient.Width(), rcClient.Height());
}

BOOL OControlManager::HandleMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
    lResult = 0;
    BOOL bResult = FALSE;
    switch(message)
    {
    case WM_SIZE:
        {
            if(wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
            {
                RelayoutControls(LOWORD(lParam), HIWORD(lParam));
                UpdateCorner();
                return true;
            }
            break;
        }
    case WM_MOUSELEAVE:
        {
            if(m_pHoverControl != NULL)
            {
                CPoint pt(LOWORD(lParam), HIWORD(lParam));
                m_pHoverControl->SetHover(FALSE, pt);
                m_pHoverControl = NULL;
            }
            break;
        }
    case WM_LBUTTONDOWN:
        {
            if(m_pHoverControl != NULL)
            {
                CPoint pt(LOWORD(lParam), HIWORD(lParam));
                m_pHoverControl->SetDown(TRUE, pt);
            }
            break;
        }
    case WM_LBUTTONUP:
        {
            if(m_pHoverControl != NULL)
            {
                CPoint pt(LOWORD(lParam), HIWORD(lParam));
                m_pHoverControl->SetDown(FALSE, pt);
            }
            break;
        }
    case WM_CAPTURECHANGED:
        {
            if(::GetCapture() == m_hWnd)
                break;
        }
    case WM_NCACTIVATE:
    case WM_ACTIVATE:
    case WM_ACTIVATEAPP:
        {
            if(m_pHoverControl != NULL)
            {
                CPoint pt(LOWORD(lParam), HIWORD(lParam));
                m_pHoverControl->SetHover(FALSE, pt);
            }
            break;
        }
    case WM_MOUSEMOVE:
        {
            bResult = TRUE;
            CPoint pt(LOWORD(lParam), HIWORD(lParam));
            UpdateHover(pt);
            break;
        }
    case WM_NCHITTEST:
        {
            bResult = TRUE;
            CPoint pt(LOWORD(lParam), HIWORD(lParam));
            ::ScreenToClient(m_hWnd, &pt);
            UpdateHover(pt);
            lResult = (m_pHoverControl == NULL) ? HTCAPTION : HTCLIENT;
            break;
        }
    }
    return bResult;
}

void OControlManager::Draw(HDC hDc, const CRect& rcClip)
{
    CRect rcControl;
    OControlVector::const_iterator ite = m_vctControls.begin();
    for(; ite != m_vctControls.end(); ++ ite)
    {
        OControl* control = *ite;
        rcControl = control->GetRect();
        if(!Util::IsRectsOverlapped(&rcClip, &rcControl))
            continue;
        control->Draw(hDc);
    }
}

HWND OControlManager::GetWindow() const
{
    return m_hWnd;
}

OButton* OControlManager::CreateButton(LPCTSTR szResName, UINT uButtonCommandId, UINT uLayout, int nImageCount)
{
    CRect rcMargin;
    rcMargin.SetRect(0, 0, 0, 0);
    return CreateButton(szResName, uButtonCommandId, uLayout, nImageCount, rcMargin);
}

OButton* OControlManager::CreateButton(LPCTSTR szResName, UINT uButtonCommandId, UINT uLayout, int nImageCount, const CRect& rcMargin)
{
    OButton* control = new OButton;
    control->Create(this, szResName, uButtonCommandId, uLayout, nImageCount, rcMargin);

    CRect rcClient;
    ::GetClientRect(m_hWnd, &rcClient);

    CRect rcControl;
    GetControlRect(rcClient.Width(), rcClient.Height(), control, rcControl);
    control->SetRect(rcControl);

    m_vctControls.push_back(control);
    return control;
}

void OControlManager::Invalidate(const CRect& rect) const
{
    ::InvalidateRect(m_hWnd, rect, FALSE);
}

void OControlManager::GetControlRect(int nWidth, int nHeight, OControl* control, CRect& rcControl) const
{
    UINT uLayout = control->GetLayout();
    CRect rcMargin = control->GetMargin();

    CSize size = control->GetAutoSize();
    rcControl.SetRect(0, 0, size.cx, size.cy);
    if(uLayout & ManagerLayout::Top)
    {
        rcControl.top = rcMargin.top;
        rcControl.bottom = rcControl.top + size.cy;
    }
    else if(uLayout & ManagerLayout::Bottom)
    {
        rcControl.bottom = nHeight - rcMargin.bottom;
        rcControl.top = rcControl.bottom - size.cy;
    }

    if(uLayout & ManagerLayout::Left)
    {
        rcControl.left = rcMargin.left;
        rcControl.right = rcControl.left + size.cx;
    }
    else if(uLayout & ManagerLayout::Right)
    {
        rcControl.right = nWidth - rcMargin.right;
        rcControl.left = rcControl.right - size.cx;
    }
}

void OControlManager::RelayoutControls(int nWidth, int nHeight) const
{
    CRect rcControl;
    OControlVector::const_iterator ite = m_vctControls.begin();
    for(; ite != m_vctControls.end(); ++ ite)
    {
        OControl* control = *ite;
        GetControlRect(nWidth, nHeight, control, rcControl);
        control->SetRect(rcControl);
    }
}

OControl* OControlManager::GetControlByPt(const CPoint& pt) const
{
    CRect rcControl;
    OControlVector::const_iterator ite = m_vctControls.begin();
    for(; ite != m_vctControls.end(); ++ ite)
    {
        OControl* control = *ite;
        rcControl = control->GetRect();
        if(rcControl.PtInRect(pt))
            return control;
    }
    return NULL;
}

void OControlManager::UpdateHover(const CPoint& pt)
{
    OControl* pNewHover = GetControlByPt(pt);
    if(pNewHover != m_pHoverControl)
    {
        if(m_pHoverControl == NULL && pNewHover != NULL)
        {
            TRACKMOUSEEVENT _Event = {0};
            _Event.cbSize = sizeof(_Event);
            _Event.dwFlags = TME_LEAVE;
            _Event.hwndTrack = m_hWnd;
            TrackMouseEvent(&_Event);
        }

        if(m_pHoverControl != NULL)
            m_pHoverControl->SetHover(FALSE, pt);
        if(pNewHover)
            pNewHover->SetHover(TRUE, pt);
        m_pHoverControl = pNewHover;
    }
}

void OControlManager::UpdateCorner()
{
    CRect rcWnd;
    ::GetClientRect(m_hWnd, &rcWnd);
    HRGN hRgn = ::CreateRectRgn(0, 0, rcWnd.Width(), rcWnd.Height());
    ::SetWindowRgn(m_hWnd, hRgn, FALSE);
}
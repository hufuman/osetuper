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
    OControlVector::iterator ite = m_vctControls.begin();
    for(; ite != m_vctControls.end(); ++ ite)
    {
        OControl* control = *ite;
        delete control;
    }
    m_vctControls.clear();
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
    case WM_SETCURSOR:
        {
            if(m_pHoverControl == NULL)
            {
                bResult = FALSE;
            }
            else
            {
                HCURSOR hCursor = m_pHoverControl->GetCursor();
                if(hCursor != NULL)
                {
                    bResult = TRUE;
                    lResult = TRUE;
                    ::SetCursor(hCursor);
                }
            }
            break;
        }
    }
    return bResult;
}

void OControlManager::Draw(HDC hDc, const CRect& rcClip)
{
    CRect rcClient;
    ::GetClientRect(m_hWnd, &rcClient);

    HDC hMemDc = ::CreateCompatibleDC(hDc);
    HBITMAP hMemBitmap = ::CreateCompatibleBitmap(hDc, rcClient.Width(), rcClient.Height());
    HGDIOBJ hOldBmp = ::SelectObject(hMemDc, hMemBitmap);
    ::SetBkMode(hMemDc, TRANSPARENT);
    ::SetTextColor(hMemDc, RGB(255, 255, 255));

    HRGN hClipRgn = (HRGN)::GetCurrentObject(hDc, OBJ_REGION);
    HFONT hFont = (HFONT)::SendMessage(m_hWnd, WM_GETFONT, 0, 0);
    ::SelectClipRgn(hMemDc, hClipRgn);
    HGDIOBJ hOldFont = ::SelectObject(hMemDc, hFont);

    CRect rcControl;
    OControlVector::const_iterator ite = m_vctControls.begin();
    for(; ite != m_vctControls.end(); ++ ite)
    {
        OControl* control = *ite;
        if(!control->IsVisible())
            continue;
        rcControl = control->GetRect();
        if(!Util::IsRectsOverlapped(&rcClip, &rcControl))
            continue;
        control->Draw(hMemDc);
    }

    ::BitBlt(hDc, 0, 0, rcClient.Width(), rcClient.Height(), hMemDc, 0, 0, SRCCOPY);

    ::SelectObject(hMemDc, hOldBmp);

    ::SelectObject(hMemDc, NULL);
    ::SelectObject(hMemDc, hOldFont);

    ::DeleteObject(hMemBitmap);
    ::DeleteDC(hMemDc);
}

HWND OControlManager::GetWindow() const
{
    return m_hWnd;
}

OButton* OControlManager::CreateButton(LPCTSTR szResName, UINT uButtonCommandId, UINT uLayout, int nImageCount, const CRect& rcMargin)
{
    OButton* control = new OButton(this);

    m_vctControls.push_back(control);
    control->Create(szResName, uButtonCommandId, uLayout, nImageCount, rcMargin);

    return control;
}

OShape* OControlManager::CreateShape(COLORREF color, UINT uLayout, const CRect& rcMargin)
{
    OShape* control = new OShape(this);
    m_vctControls.push_back(control);

    control->Create(NULL, 0, uLayout, 0, rcMargin);
    control->SetColor(color);

    return control;
}

OEdit* OControlManager::CreateEdit(LPCTSTR szTextAttr, UINT uLayout, const CRect& rcMargin)
{
    OEdit* control = new OEdit(this);
    m_vctControls.push_back(control);

    control->Create(NULL, 0, uLayout, 0, rcMargin);
    control->SetTextAttr(szTextAttr, FALSE);

    return control;
}

OLink* OControlManager::CreateLink(LPCTSTR szTextAttr, UINT uLayout, const CRect& rcMargin)
{
    OLink* control = new OLink(this);
    m_vctControls.push_back(control);

    control->Create(NULL, 0, uLayout, 0, rcMargin);
    control->SetTextAttr(szTextAttr, TRUE);

    return control;
}

OLabel* OControlManager::CreateLabel(LPCTSTR szTextAttr, UINT uLayout, const CRect& rcMargin)
{
    OLabel* control = new OLabel(this);
    m_vctControls.push_back(control);

    control->Create(NULL, 0, uLayout, 0, rcMargin);
    control->SetTextAttr(szTextAttr, TRUE);

    return control;
}

OCheckBox* OControlManager::CreateCheckBox(LPCTSTR szResName, UINT uButtonCommandId, UINT uLayout, const CRect& rcMargin)
{
    OCheckBox* control = new OCheckBox(this);
    m_vctControls.push_back(control);

    control->Create(szResName, uButtonCommandId, uLayout, 4, rcMargin);

    return control;
}

void OControlManager::Invalidate(const CRect& rect) const
{
    ::InvalidateRect(m_hWnd, rect, FALSE);
}

void OControlManager::RelayoutControls(int nWidth, int nHeight) const
{
    OControlVector::const_iterator ite = m_vctControls.begin();
    for(; ite != m_vctControls.end(); ++ ite)
    {
        OControl* control = *ite;
        control->AutoSize(nWidth, nHeight);
    }
}

OControl* OControlManager::GetControlByPt(const CPoint& pt) const
{
    CRect rcControl;
    OControlVector::const_iterator ite = m_vctControls.begin();
    for(; ite != m_vctControls.end(); ++ ite)
    {
        OControl* control = *ite;
        if(!control->NeedHover() || !control->IsVisible())
            continue;
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


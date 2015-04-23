#include "stdafx.h"
#include "OControl.h"


#include "Util.h"
#include "OControlManager.h"

class OGdiObjSelector
{
    OGdiObjSelector();
    OGdiObjSelector(const OGdiObjSelector&);
    OGdiObjSelector& operator = (const OGdiObjSelector&);
public:
    OGdiObjSelector(HDC hDc, HGDIOBJ hNewObj)
    {
        m_hDc = hDc;
        if(hNewObj == NULL)
            m_hOldObj = NULL;
        else
            m_hOldObj = ::SelectObject(hDc, hNewObj);
    }
    ~OGdiObjSelector()
    {
        if(m_hOldObj != NULL)
            ::SelectObject(m_hDc, m_hOldObj);
    }
private:
    HDC     m_hDc;
    HGDIOBJ m_hOldObj;
};

class OTextColor
{
    OTextColor();
    OTextColor(const OTextColor&);
    OTextColor& operator = (const OTextColor&);
public:
    OTextColor(HDC hDc, COLORREF clrText)
    {
        m_hDc = hDc;
        m_clrOld = ::SetTextColor(hDc, clrText);
    }
    ~OTextColor()
    {
        ::SetTextColor(m_hDc, m_clrOld);
    }
private:
    HDC     m_hDc;
    COLORREF m_clrOld;
};

OControl::OControl(OControlManager* manager)
{
    m_hImage = NULL;
    m_nHorzImageCount = 4;
    m_nVertImageCount = 1;
    m_uLayout = 0;

    m_uCommandId = 0;
    m_pManager = NULL;
    m_Status = ControlStatus::StatusNormal;

    m_SizeOfImage.cx = m_SizeOfImage.cy = 0;
    m_Rect.SetRect(0, 0, 0, 0);
    m_Margin.SetRect(0, 0, 0, 0);

    m_pManager = manager;
    m_hCursor = NULL;
    m_bVisible = TRUE;

    m_clrText = RGB(255, 255, 255);
}

OControl::~OControl()
{
    if(m_hImage != NULL)
        ::DeleteObject(m_hImage);
}

BOOL OControl::Create(LPCTSTR szResName, UINT uCommandId, UINT uLayout, int nImageCount)
{
    CRect rcMargin(0, 0, 0, 0);
    return Create(szResName, uCommandId, uLayout, nImageCount, rcMargin);
}

BOOL OControl::Create(LPCTSTR szResName, UINT uCommandId, UINT uLayout, int nImageCount, const CRect& rcMargin)
{
    m_Margin = rcMargin;
    m_uLayout = uLayout;
    m_uCommandId = uCommandId;
    m_nHorzImageCount = nImageCount;

    if(szResName != NULL)
    {
        m_hImage = Util::LoadImageFromExeRes(szResName);
        if(m_hImage != NULL)
        {
            BITMAP bmp;
            ::GetObject(m_hImage, sizeof(bmp), &bmp);
            m_SizeOfImage.cx = bmp.bmWidth;
            m_SizeOfImage.cy = bmp.bmHeight;
        }
    }
    AutoSize();
    OnCreate();
    return TRUE;
}

BOOL OControl::IsVisible() const
{
    return m_bVisible;
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

HFONT OControl::GetFont() const
{
    return NULL;
}

void OControl::SetVisible(BOOL bVisible)
{
    m_bVisible = bVisible;
    Invalidate();
}

void OControl::SetHover(BOOL bHover, const CPoint& pt)
{
    ControlStatus::Status lastStatus = m_Status;
    if(Util::IsKeyPressed(VK_LBUTTON) && m_Rect.PtInRect(pt))
        m_Status = ControlStatus::StatusDown;
    else if(bHover)
        m_Status = ControlStatus::StatusHover;
    else
        m_Status = ControlStatus::StatusNormal;
    if(lastStatus == ControlStatus::StatusDown
        && m_Status != ControlStatus::StatusDown
        && m_pManager->GetWindow() == ::GetCapture())
    {
        ::ReleaseCapture();
    }
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

void OControl::SetTextColor(COLORREF clrText)
{
    m_clrText = clrText;
}

void OControl::SetText(LPCTSTR szText)
{
    m_strText = szText;
    Invalidate();
}

CString OControl::GetText() const
{
    return m_strText;
}

void OControl::Draw(HDC hDc) const
{
    if(m_hImage != NULL)
    {
        CRect rcImage = GetStatusImageRect();
        Util::AlphaDraw(m_hImage, rcImage.left, rcImage.top, rcImage.Width(), rcImage.Height(), hDc, m_Rect.left, m_Rect.top, m_Rect.Width(), m_Rect.Height());
    }

    if(!m_strText.IsEmpty())
    {
        OGdiObjSelector selector(hDc, GetFont());
        OTextColor textColor(hDc, m_clrText);
        ::DrawText(hDc, m_strText, m_strText.GetLength(), (LPRECT)&m_Rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    }
}

void OControl::Invalidate() const
{
    m_pManager->Invalidate(m_Rect);
}

BOOL OControl::NeedHover() const
{
    return TRUE;
}

HCURSOR OControl::GetCursor() const
{
    return m_hCursor;
}

CSize OControl::GetAutoSize() const
{
    if(m_hImage == NULL)
    {
        if(m_Rect.IsRectEmpty())
        {
            CString strText = m_strText;
            if(strText.IsEmpty())
            {
                strText = _T("for good osetuper");
            }
            CRect rcText(0, 0, 65535, 65535);
            HDC hClientDc = ::GetDC(m_pManager->GetWindow());
            OGdiObjSelector selector(hClientDc, GetFont());
            ::DrawText(hClientDc, strText, strText.GetLength(), &rcText, DT_SINGLELINE | DT_LEFT | DT_TOP | DT_CALCRECT);
            ::ReleaseDC(m_pManager->GetWindow(), hClientDc);

            CSize size(rcText.Width(), rcText.Height());
            return size;
        }
        else
        {
            CSize size(m_Rect.Width(), m_Rect.Height());
            return size;
        }
    }
    else
    {
        CSize size(m_SizeOfImage);
        size.cx = m_SizeOfImage.cx / m_nHorzImageCount;
        size.cy = m_SizeOfImage.cy / m_nVertImageCount;
        return size;
    }
}

void OControl::AutoSize()
{
    CRect rcClient;
    ::GetClientRect(m_pManager->GetWindow(), &rcClient);
    AutoSize(rcClient.Width(), rcClient.Height());
}

void OControl::AutoSize(int nWidth, int nHeight)
{
    CRect rcControl;
    GetControlRect(nWidth, nHeight, rcControl);
    SetRect(rcControl);
}

void OControl::FireCommand(UINT uCommandCode)
{
    if(m_uCommandId == 0)
        return;
    ::PostMessage(m_pManager->GetWindow(), WM_COMMAND, MAKEWPARAM(m_uCommandId, uCommandCode), 0);
}

void OControl::GetControlRect(int nWidth, int nHeight, CRect& rcControl) const
{
    UINT uLayout = GetLayout();
    CRect rcMargin = GetMargin();

    CSize size = GetAutoSize();
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
    else if(uLayout & ManagerLayout::VFill)
    {
        rcControl.bottom = nHeight - rcMargin.bottom;
        rcControl.top = rcMargin.top;
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
    else if(uLayout & ManagerLayout::HFill)
    {
        rcControl.left = rcMargin.left;
        rcControl.right = nWidth - rcMargin.right;
    }
}

CRect OControl::GetStatusImageRect() const
{
    CRect rcImage(0, 0, m_SizeOfImage.cx, m_SizeOfImage.cy);

    switch(m_Status)
    {
    case ControlStatus::StatusNormal:
        rcImage.TopLeft().SetPoint(0, 0);
        break;
    case ControlStatus::StatusDown:
        rcImage.TopLeft().SetPoint(m_SizeOfImage.cx / m_nHorzImageCount * 2, 0);
        break;
    case ControlStatus::StatusHover:
        rcImage.TopLeft().SetPoint(m_SizeOfImage.cx / m_nHorzImageCount, 0);
        break;
    case ControlStatus::StatusDisabled:
        rcImage.TopLeft().SetPoint(m_SizeOfImage.cx / m_nHorzImageCount * 3, 0);
        break;
    }
    rcImage.BottomRight().SetPoint(rcImage.left + m_SizeOfImage.cx / m_nHorzImageCount, rcImage.top + m_SizeOfImage.cy / m_nVertImageCount);

    return rcImage;
}

void OControl::OnCreate()
{
    ;
}

//////////////////////////////////////////////////////////////////////////
OButton::OButton(OControlManager* manager)
    : OControl(manager)
{
    ;
}

//////////////////////////////////////////////////////////////////////////

OShape::OShape(OControlManager* manager)
    : OControl(manager)
{
    m_hBrush = NULL;
}

OShape::~OShape()
{
    ::DeleteObject(m_hBrush);
    m_hBrush = NULL;
}

void OShape::SetColor(COLORREF color)
{
    if(m_hBrush != NULL)
        ::DeleteObject(m_hBrush);
    m_hBrush = ::CreateSolidBrush(color);
    Invalidate();
}

void OShape::Draw(HDC hDc) const
{
    ::FillRect(hDc, &m_Rect, m_hBrush);
}

BOOL OShape::NeedHover() const
{
    return FALSE;
}

//////////////////////////////////////////////////////////////////////////
OEdit::OEdit(OControlManager* manager)
    : OControl(manager)
{
    m_hWndEdit = NULL;
}

OEdit::~OEdit()
{
    if(m_hWndEdit != NULL)
        ::DestroyWindow(m_hWndEdit);
}

void OEdit::SetText(LPCTSTR szText)
{
    m_strText = szText;
    if(m_hWndEdit != NULL)
        ::SetWindowText(m_hWndEdit, m_strText);
}

CString OEdit::GetText() const
{
    CString strText = m_strText;
    if(m_hWndEdit != NULL)
    {
        TCHAR szText[1024];
        ::GetWindowText(m_hWndEdit, szText, 1024);
        strText = szText;
    }
    return strText;
}

BOOL OEdit::NeedHover() const
{
    return FALSE;
}

void OEdit::OnCreate()
{
    m_hWndEdit = ::CreateWindowEx(0,
        WC_EDIT,
        m_strText,
        WS_TABSTOP | ES_AUTOHSCROLL | ES_AUTOVSCROLL | WS_VISIBLE | WS_CHILD,
        m_Rect.left, m_Rect.top,
        m_Rect.Width(), m_Rect.Height(),
        m_pManager->GetWindow(),
        NULL, ::GetModuleHandle(NULL), 0);
    HFONT hFont = (HFONT)::SendMessage(m_pManager->GetWindow(), WM_GETFONT, 0, 0);
    ::SendMessage(m_hWndEdit, WM_SETFONT, (WPARAM)hFont, 0);
}

//////////////////////////////////////////////////////////////////////////

OCheckBox::OCheckBox(OControlManager* manager)
    : OControl(manager)
{
    m_bChecked = TRUE;
    m_nVertImageCount = 2;
}

void OCheckBox::SetChecked(BOOL bChecked)
{
    m_bChecked = bChecked;
    Invalidate();
}

BOOL OCheckBox::IsChecked() const
{
    return m_bChecked;
}

void OCheckBox::SetText(LPCTSTR szText)
{
    __super::SetText(szText);
    AutoSize();
}

void OCheckBox::Draw(HDC hDc) const
{
    CRect rcImage = GetStatusImageRect();
    Util::AlphaDraw(m_hImage,
        rcImage.left, m_bChecked ? rcImage.top + m_SizeOfImage.cy / m_nVertImageCount : rcImage.top,
        rcImage.Width(), rcImage.Height(),
        hDc,
        m_Rect.left, m_Rect.top - 2,
        rcImage.Width(), rcImage.Height());

    CRect rcText(m_Rect);
    rcText.left += 2 + rcImage.Width();
    OGdiObjSelector selector(hDc, GetFont());
    OTextColor textColor(hDc, m_clrText);
    ::DrawText(hDc, m_strText, m_strText.GetLength(), &rcText, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS);
}

CSize OCheckBox::GetAutoSize() const
{
    CRect rcText(0, 0, 65530, 65530);
    if(m_strText.IsEmpty())
    {
        rcText.SetRect(0, 0, 0, 0);
    }
    else
    {
        HDC hClientDc = ::GetDC(m_pManager->GetWindow());
        OGdiObjSelector selector(hClientDc, GetFont());
        ::DrawText(hClientDc, m_strText, m_strText.GetLength(), &rcText, DT_SINGLELINE | DT_LEFT | DT_TOP | DT_CALCRECT);
        ::ReleaseDC(m_pManager->GetWindow(), hClientDc);
    }

    CSize size;
    size.cx = m_SizeOfImage.cx / m_nHorzImageCount + 2 + rcText.Width();
    size.cy = (m_SizeOfImage.cy >= rcText.Height()) ? m_SizeOfImage.cy / m_nVertImageCount : rcText.Height();
    return size;
}

void OCheckBox::FireCommand(UINT uCommandCode)
{
    m_bChecked = !m_bChecked;
    Invalidate();
    __super::FireCommand(uCommandCode);
}

//////////////////////////////////////////////////////////////////////////

OLink::OLink(OControlManager* manager)
    : OControl(manager)
{
    m_hCursor = ::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND));

    HFONT hFont = (HFONT)::SendMessage(m_pManager->GetWindow(), WM_GETFONT, 0, 0);
    LOGFONT logFont;
    ::GetObject(hFont, sizeof(logFont), &logFont);
    logFont.lfUnderline = TRUE;
    m_hLinkFont = ::CreateFontIndirect(&logFont);
}

void OLink::Draw(HDC hDc) const
{
    DWORD dwColor = ::SetTextColor(hDc, RGB(0, 0, 255));

    __super::Draw(hDc);

    ::SetTextColor(hDc, dwColor);
}

HFONT OLink::GetFont() const
{
    return m_hLinkFont;
}

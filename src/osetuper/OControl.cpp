#include "stdafx.h"
#include "OControl.h"


#include "Util.h"
#include "OControlManager.h"
#include "OGdiObjManager.h"
#include "StringBundle.h"


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
    OTextColor(HDC hDc)
    {
        m_hDc = hDc;
        m_clrOld = RGB(255, 255, 0);
    }
    void SetColor(COLORREF clrText)
    {
        DWORD clrOld = ::SetTextColor(m_hDc, clrText);
        if(m_clrOld == RGB(255, 255, 0))
            m_clrOld = clrOld;
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
    m_minSize.SetSize(-1, -1);

    m_pManager = manager;
    m_hCursor = NULL;
    m_bVisible = TRUE;
    m_hFont = OGdiObjManager::GetInst().GetFont(14, false);

    m_clrText = RGB(255, 255, 255);
    m_clrTextHover = m_clrText;
    m_uTextAlign = TextAlign::AlignCenter | TextAlign::AlignVCenter;
    m_bAutoSize = TRUE;
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
    return m_hFont;
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

void OControl::SetMinSize(int nMinWidth, int nMinHeight)
{
    m_minSize.SetSize(nMinWidth, nMinHeight);
    AutoSize();
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
    AutoSize();
}

void OControl::SetTextAlign(UINT align)
{
    m_uTextAlign = align;
}

// 14,FFFFFF,通用安装包
void OControl::SetTextAttr(LPCTSTR szTextAttrTitle, BOOL bAutoSize)
{
    if(szTextAttrTitle != NULL && szTextAttrTitle[0] != 0)
    {
        CString strTextAttr(CStringBundle::GetInst().Get(szTextAttrTitle));
        int start = 0;
        CString fontSize = strTextAttr.Tokenize(_T(","), start);
        bool bBold = fontSize[fontSize.GetLength() - 1] == _T('b');
        if(bBold)
            fontSize = fontSize.Mid(0, fontSize.GetLength() - 1);
        m_hFont = OGdiObjManager::GetInst().GetFont(_ttoi(fontSize), bBold);
        CString textColor = strTextAttr.Tokenize(_T(","), start);
        int byRed, byGreen, byBlue;
        _sntscanf(textColor, 123, _T("%02x%02x%02x"), &byRed, &byGreen, &byBlue);
        m_clrText = RGB(byRed, byGreen, byBlue);
        m_strText = strTextAttr.Tokenize(_T(","), start);
        m_clrTextHover = m_clrText;
        if(start > 0)
        {
            CString hoverTextColor = strTextAttr.Tokenize(_T(","), start);
            if(!hoverTextColor.IsEmpty())
            {
                _sntscanf(hoverTextColor, 123, _T("%02x%02x%02x"), &byRed, &byGreen, &byBlue);
                m_clrTextHover = RGB(byRed, byGreen, byBlue);
            }
        }
    }

    if(bAutoSize)
        AutoSize();
    else
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
        OTextColor textColor(hDc);
        if(ControlStatus::StatusHover == m_Status)
            textColor.SetColor(m_clrTextHover);
        else
            textColor.SetColor(m_clrText);
        ::DrawText(hDc, m_strText, m_strText.GetLength(), (LPRECT)&m_Rect, GetDrawTextFlag());
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
    CSize size;
    if(m_hImage == NULL)
    {
        if(m_strText.IsEmpty())
        {
            size.SetSize(m_Rect.Width(), m_Rect.Height());
        }
        else
        {
            CRect rcText(0, 0, 65535, 65535);
            HDC hClientDc = ::GetDC(m_pManager->GetWindow());
            OGdiObjSelector selector(hClientDc, GetFont());
            ::DrawText(hClientDc, m_strText, m_strText.GetLength(), &rcText, DT_SINGLELINE | DT_LEFT | DT_TOP | DT_CALCRECT);
            ::ReleaseDC(m_pManager->GetWindow(), hClientDc);

            size.SetSize(rcText.Width(), rcText.Height());
        }
    }
    else
    {
        size.cx = m_SizeOfImage.cx / m_nHorzImageCount;
        size.cy = m_SizeOfImage.cy / m_nVertImageCount;
    }
    if(m_minSize.cx > -1 && m_minSize.cx > size.cx)
        size.cx = m_minSize.cx;
    if(m_minSize.cy > -1 && m_minSize.cy > size.cy)
        size.cy = m_minSize.cy;
    return size;
}

void OControl::AutoSize()
{
    if(!m_bAutoSize)
    {
        Invalidate();
        return;
    }
    CRect rcClient;
    ::GetClientRect(m_pManager->GetWindow(), &rcClient);
    AutoSize(rcClient.Width(), rcClient.Height());
}

void OControl::AutoSize(int nWidth, int nHeight)
{
    if(!m_bAutoSize)
    {
        Invalidate();
        return;
    }
    CRect rcControl;
    GetControlRect(nWidth, nHeight, rcControl);
    SetRect(rcControl);
}

void OControl::SetAutoSize(BOOL bAutoSize)
{
    m_bAutoSize = bAutoSize;
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
    else if(uLayout & ManagerLayout::VCenter)
    {
        rcControl.top = (nHeight - rcMargin.bottom - rcMargin.top - size.cy) / 2;
        rcControl.bottom = rcControl.top + size.cy;
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
    else if(uLayout & ManagerLayout::HCenter)
    {
        rcControl.left = (nWidth - rcMargin.left - rcMargin.right - size.cx) / 2;
        rcControl.right = rcControl.left + size.cx;
    }
    if(rcControl.top == rcControl.bottom)
    {
        rcControl.bottom = rcControl.top + Util::GetTextHeight(m_pManager->GetWindow(), m_hFont);
    }
}

CRect OControl::GetStatusImageRect() const
{
    CRect rcImage(0, 0, m_SizeOfImage.cx, m_SizeOfImage.cy);
    if(m_nHorzImageCount == 1)
        return rcImage;

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

DWORD OControl::GetDrawTextFlag() const
{
    DWORD dwResult = DT_SINGLELINE | DT_END_ELLIPSIS;
    if(m_uTextAlign & TextAlign::AlignCenter)
        dwResult |= DT_CENTER;
    if(m_uTextAlign & TextAlign::AlignVCenter)
        dwResult |= DT_VCENTER;
    return dwResult;
}

//////////////////////////////////////////////////////////////////////////
OButton::OButton(OControlManager* manager)
: OControl(manager)
{
    ;
}

//////////////////////////////////////////////////////////////////////////
OImage::OImage(OControlManager* manager)
: OControl(manager)
{
    ;
}

//////////////////////////////////////////////////////////////////////////
OLabel::OLabel(OControlManager* manager)
: OControl(manager)
{
    ;
}

//////////////////////////////////////////////////////////////////////////

OShape::OShape(OControlManager* manager)
    : OControl(manager)
{
    m_hBrush = NULL;
    m_hPen = NULL;
    m_Color = RGB(255, 255, 255);
    m_nBorderWidth = 0;
}

OShape::~OShape()
{
    Destroy();
}

void OShape::SetColor(COLORREF color)
{
    m_Color = color;
    Destroy();
    Invalidate();
}

void OShape::SetBorder(int nBorderWidth)
{
    m_nBorderWidth = nBorderWidth;
    Invalidate();
}

void OShape::Draw(HDC hDc) const
{
    if(m_nBorderWidth == 0)
    {
        if(m_hBrush == NULL)
            m_hBrush = ::CreateSolidBrush(m_Color);
        ::FillRect(hDc, &m_Rect, m_hBrush);
    }
    else
    {
        if(m_hPen == NULL)
            m_hPen = ::CreatePen(PS_SOLID, m_nBorderWidth, m_Color);
        OGdiObjSelector penSelector(hDc, m_hPen);
        OGdiObjSelector brushSelector(hDc, GetStockObject(NULL_BRUSH));
        ::Rectangle(hDc, m_Rect.left, m_Rect.top, m_Rect.right, m_Rect.bottom);
    }
    __super::Draw(hDc);
}

BOOL OShape::NeedHover() const
{
    return FALSE;
}

void OShape::Destroy()
{
    if(m_hBrush != NULL)
    {
        ::DeleteObject(m_hBrush);
        m_hBrush = NULL;
    }
    if(m_hPen != NULL)
    {
        ::DeleteObject(m_hPen);
        m_hPen = NULL;
    }
}

//////////////////////////////////////////////////////////////////////////
ORealControl::ORealControl(OControlManager* manager)
: OControl(manager)
{
    m_hBackBrush = NULL;
}

void ORealControl::SetBackColor(COLORREF clrBack)
{
    if(m_hBackBrush != NULL)
        ::DeleteObject(m_hBackBrush);
    m_hBackBrush = ::CreateSolidBrush(clrBack);
}

HBRUSH ORealControl::OnCtlColor(HDC hDc)
{
    ::SetTextColor(hDc, m_clrText);
    ::SetBkMode(hDc, TRANSPARENT);
    return m_hBackBrush;
}

//////////////////////////////////////////////////////////////////////////
OEdit::OEdit(OControlManager* manager)
    : ORealControl(manager)
{
    m_hWndEdit = NULL;
}

OEdit::~OEdit()
{
    if(m_hWndEdit != NULL)
        ::DestroyWindow(m_hWndEdit);
}

void OEdit::SetReadOnly(BOOL bReadOnly)
{
    ::SendMessage(m_hWndEdit, EM_SETREADONLY, bReadOnly, 0L);
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

void OEdit::SetVisible(BOOL bVisible)
{
    __super::SetVisible(bVisible);
    if(m_hWndEdit != NULL)
        ::ShowWindow(m_hWndEdit, bVisible ? SW_SHOW : SW_HIDE);
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
    ::SetWindowLongPtr(m_hWndEdit, GWLP_USERDATA, (LONG)this);

    // text VCenter
    CRect rcTemp;
    ::GetClientRect(m_hWndEdit, &rcTemp);
    HDC hDc = GetDC(m_hWndEdit);
    TEXTMETRIC tm;
    ::GetTextMetrics(hDc, &tm);
    int nFontHeight = tm.tmHeight + tm.tmExternalLeading;
    int nMargin = (rcTemp.Height() - nFontHeight) / 2;
    rcTemp.DeflateRect(0,nMargin);
    ::SendMessage(m_hWndEdit, EM_SETRECTNP, 0, (LPARAM)&rcTemp);
    ReleaseDC(m_hWndEdit, hDc) ;
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
    ::DrawText(hDc, m_strText, m_strText.GetLength(), &rcText, GetDrawTextFlag());
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
    m_hLinkFont = NULL;
}

void OLink::Draw(HDC hDc) const
{
    DWORD dwColor = ::SetTextColor(hDc, RGB(0, 0, 255));

    if(m_hLinkFont == NULL)
        m_hLinkFont = OGdiObjManager::GetInst().GetLinkFont(__super::GetFont());

    __super::Draw(hDc);

    ::SetTextColor(hDc, dwColor);
}

void OLink::SetTextAttr(LPCTSTR szTextAttrTitle, BOOL bAutoSize)
{
    __super::SetTextAttr(szTextAttrTitle, bAutoSize);
    m_hLinkFont = OGdiObjManager::GetInst().GetLinkFont(__super::GetFont());
}

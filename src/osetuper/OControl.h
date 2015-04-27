#pragma once


class OControlManager;

class ControlStatus
{
public:
    enum Status
    {
        StatusNormal    = 0x00,
        StatusHover     = 0x01,
        StatusDown      = 0x02,
        StatusDisabled  = 0x04,
    };
};

//////////////////////////////////////////////////////////////////////////
class OControl
{
public:
    OControl(OControlManager* manager);
    ~OControl();

    BOOL Create(LPCTSTR szResName, UINT uCommandId, UINT uLayout, int nImageCount);
    BOOL Create(LPCTSTR szResName, UINT uCommandId, UINT uLayout, int nImageCount, const CRect& rcMargin);

    BOOL IsVisible() const;
    UINT GetLayout() const;
    CRect GetRect() const;
    CRect GetMargin() const;
    BOOL GetDown() const;
    ControlStatus::Status GetStatus() const;
    virtual HFONT GetFont() const;

    void SetVisible(BOOL bVisible);
    void SetHover(BOOL bHover, const CPoint& pt);
    void SetDown(BOOL bDown, const CPoint& pt);
    void SetRect(const CRect& rect);
    void SetMinSize(int nMinWidth, int nMinHeight);
    void SetTextColor(COLORREF clrText);
    virtual void SetText(LPCTSTR szText);
    // 14,FFFFFF,通用安装包
    virtual void SetTextAttr(LPCTSTR szTextAttr, BOOL bAutoSize);
    virtual CString GetText() const;

    virtual void Draw(HDC hDc) const;
    virtual CSize GetAutoSize() const;
    virtual void AutoSize();
    virtual void AutoSize(int nWidth, int nHeight);

    void Invalidate() const;

    virtual BOOL NeedHover() const;
    virtual HCURSOR GetCursor() const;
    virtual void FireCommand(UINT uCommandCode);

protected:
    void GetControlRect(int nWidth, int nHeight, CRect& rcControl) const;
    CRect GetStatusImageRect() const;
    virtual void OnCreate();

protected:
    HBITMAP m_hImage;
    SIZE    m_SizeOfImage;
    int     m_nHorzImageCount;
    int     m_nVertImageCount;

    UINT    m_uLayout;
    CRect   m_Rect;
    CRect   m_Margin;
    CSize   m_minSize;

    CString m_strText;
    HCURSOR m_hCursor;
    COLORREF m_clrText;
    BOOL    m_bVisible;
    HFONT   m_hFont;

    UINT    m_uCommandId;
    OControlManager* m_pManager;

    ControlStatus::Status m_Status;
};


//////////////////////////////////////////////////////////////////////////
class OButton : public OControl
{
public:
    OButton(OControlManager* manager);
};


//////////////////////////////////////////////////////////////////////////
class OLabel : public OControl
{
public:
    OLabel(OControlManager* manager);
};

//////////////////////////////////////////////////////////////////////////
class OShape : public OControl
{
public:
    OShape(OControlManager* manager);
    ~OShape();

    void SetColor(COLORREF color);
    void SetBorder(int nBorderWidth);

public:
    virtual void Draw(HDC hDc) const;
    virtual BOOL NeedHover() const;

private:
    void Destroy();

private:
    mutable HBRUSH m_hBrush;
    mutable HPEN m_hPen;
    COLORREF m_Color;
    int m_nBorderWidth;
};

//////////////////////////////////////////////////////////////////////////
class OEdit : public OControl
{
public:
    OEdit(OControlManager* manager);
    ~OEdit();

    virtual void SetText(LPCTSTR szText);
    virtual CString GetText() const;

    virtual BOOL NeedHover() const;

protected:
    virtual void OnCreate();

private:
    HWND m_hWndEdit;
};

//////////////////////////////////////////////////////////////////////////
class OCheckBox : public OControl
{
public:
    OCheckBox(OControlManager* manager);

    void SetChecked(BOOL bChecked);
    BOOL IsChecked() const;

    virtual void SetText(LPCTSTR szText);

    virtual void Draw(HDC hDc) const;
    virtual CSize GetAutoSize() const;

    virtual void FireCommand(UINT uCommandCode);

private:
    BOOL m_bChecked;
};

//////////////////////////////////////////////////////////////////////////
class OLink : public OControl
{
public:
    OLink(OControlManager* manager);
    virtual void Draw(HDC hDc) const;

protected:
    virtual HFONT GetFont() const;

protected:
    HFONT m_hLinkFont;
};

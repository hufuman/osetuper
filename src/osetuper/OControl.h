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

    UINT GetLayout() const;
    CRect GetRect() const;
    CRect GetMargin() const;
    BOOL GetDown() const;
    ControlStatus::Status GetStatus() const;
    virtual HFONT GetFont() const;

    void SetHover(BOOL bHover, const CPoint& pt);
    void SetDown(BOOL bDown, const CPoint& pt);
    void SetRect(const CRect& rect);
    virtual void SetText(LPCTSTR szText);
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

    CString m_strText;
    HCURSOR m_hCursor;

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
class OShape : public OControl
{
public:
    OShape(OControlManager* manager);
    ~OShape();
    void SetColor(COLORREF color);

public:
    virtual void Draw(HDC hDc) const;
    virtual BOOL NeedHover() const;
private:
    HBRUSH m_hBrush;
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

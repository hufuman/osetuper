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
    OControl();
    ~OControl();

    BOOL Create(OControlManager* manager, LPCTSTR szResName, UINT uCommandId, UINT uLayout, int nImageCount);
    BOOL Create(OControlManager* manager, LPCTSTR szResName, UINT uCommandId, UINT uLayout, int nImageCount, const CRect& rcMargin);

    UINT GetLayout() const;
    CRect GetRect() const;
    CRect GetMargin() const;
    BOOL GetDown() const;
    ControlStatus::Status GetStatus() const;

    void SetHover(BOOL bHover, const CPoint& pt);
    void SetDown(BOOL bDown, const CPoint& pt);
    void SetRect(const CRect& rect);

    virtual void Draw(HDC hDc) const;
    void Invalidate() const;
    virtual CSize GetAutoSize();

    void FireCommand(UINT uCommandCode);

protected:
    HBITMAP m_hImage;
    SIZE    m_SizeOfImage;
    int     m_nImageCount;

    UINT    m_uLayout;
    CRect   m_Rect;
    CRect   m_Margin;

    UINT    m_uCommandId;
    OControlManager* m_pManager;

    ControlStatus::Status m_Status;
};


//////////////////////////////////////////////////////////////////////////
class OButton : public OControl
{
public:
    virtual void Draw(HDC hDc) const;
};

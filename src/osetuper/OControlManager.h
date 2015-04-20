#pragma once

#include "OControl.h"

class ManagerLayout
{
public:
    enum
    {
        Top     = 0x01,
        Left    = 0x02,
        Right   = 0x04,
        Bottom  = 0x08,
    };
};


class OControlManager
{
public:
    OControlManager();
    ~OControlManager();

    void Init(HWND hWnd);
    BOOL HandleMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult);
    void Draw(HDC hDc, const CRect& rcWindow);

    HWND GetWindow() const;

    OButton* CreateButton(LPCTSTR szResName, UINT uButtonCommandId, UINT uLayout, int nImageCount);
    OButton* CreateButton(LPCTSTR szResName, UINT uButtonCommandId, UINT uLayout, int nImageCount, const CRect& rcMargin);

public:
    void Invalidate(const CRect& rect) const;
    void GetControlRect(int nWidth, int nHeight, OControl* control, CRect& rcControl) const;
    void RelayoutControls(int nWidth, int nHeight) const;
    OControl* GetControlByPt(const CPoint& pt) const;

protected:
    void UpdateHover(const CPoint& pt);
    void UpdateCorner();

private:
    HWND m_hWnd;

    OControl* m_pHoverControl;

    typedef std::vector<OControl*> OControlVector;
    OControlVector  m_vctControls;
};

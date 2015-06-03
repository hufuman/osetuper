#pragma once

#include "OControl.h"

class ManagerLayout
{
public:
    enum
    {
        Top     = 0x001,
        Left    = 0x002,
        Right   = 0x004,
        Bottom  = 0x008,
        HFill   = 0x010,
        VFill   = 0x020,
        HCenter = 0x040,
        VCenter = 0x080
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

    OControl*   CreateImage(LPCTSTR szResName, UINT uLayout, const CRect& rcMargin);
    OButton*    CreateButton(LPCTSTR szResName, UINT uButtonCommandId, UINT uLayout, int nImageCount, const CRect& rcMargin);

    OShape*     CreateShape(COLORREF color, UINT uLayout, const CRect& rcMargin);
    OEdit*      CreateEdit(LPCTSTR szTextAttrTitle, UINT uLayout, const CRect& rcMargin);
    OLink*      CreateLink(UINT uButtonCommandId, LPCTSTR szTextAttrTitle, UINT uLayout, const CRect& rcMargin);
    OLabel*     CreateLabel(LPCTSTR szTextAttrTitle, UINT uLayout, const CRect& rcMargin);
    OCheckBox*  CreateCheckBox(LPCTSTR szResName, UINT uButtonCommandId, UINT uLayout, const CRect& rcMargin);

public:
    void Invalidate(const CRect& rect) const;
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

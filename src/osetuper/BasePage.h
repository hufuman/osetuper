#pragma once


#include "OControlManager.h"


class CBasePage
{
public:
    CBasePage(OControlManager* pManager)
    {
        m_pControlManager = pManager;
    }

    ~CBasePage(void)
    {
        ;
    }

public:
    virtual void Create() = 0;


    virtual void Destroy()
    {
        size_t count = m_arrControls.size();
        for(size_t i=0; i<count; ++ i)
        {
            OControl* page = m_arrControls[i];
            delete page;
        }
        m_arrControls.clear();
    }

    virtual void Show(BOOL bShow)
    {
        size_t count = m_arrControls.size();
        for(size_t i=0; i<count; ++ i)
        {
            OControl* page = m_arrControls[i];
            page->SetVisible(bShow);
        }
    }

protected:
    void AddControl(OControl* control)
    {
        m_arrControls.push_back(control);
    }

protected:
    OControlManager* m_pControlManager;
    typedef std::vector<OControl*> ControlArray;
    ControlArray    m_arrControls;
};

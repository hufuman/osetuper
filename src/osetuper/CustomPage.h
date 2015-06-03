#pragma once


#include "BasePage.h"


class CCustomPage : public CBasePage
{
public:
    CCustomPage(OControlManager* pManager);
    ~CCustomPage(void);

public:
    virtual void Create();
    virtual BOOL OnCommand(WORD command);

protected:
    OLabel* m_pTitle;
    OEdit*  m_pPath;
};

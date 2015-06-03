#pragma once


#include "BasePage.h"


class CFinishPage : public CBasePage
{
public:
    CFinishPage(OControlManager* pManager);
    ~CFinishPage(void);

public:
    virtual void Create();
    virtual void Show(BOOL bShow);

private:
    OCheckBox* m_pChkStart;
};

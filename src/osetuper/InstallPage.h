#pragma once


#include "BasePage.h"


class CInstallPage : public CBasePage
{
public:
    CInstallPage(OControlManager* pManager);
    ~CInstallPage(void);

public:
    virtual void Create();

};

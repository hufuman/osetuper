#pragma once


#include "BasePage.h"


class CWelcomePage : public CBasePage
{
public:
    CWelcomePage(OControlManager* pManager);
    ~CWelcomePage(void);

public:
    virtual void Create();

};

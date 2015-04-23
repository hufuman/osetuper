#pragma once


#include "BasePage.h"


class CPageManager
{
public:
    enum PageIndex
    {
        PageWelcome,
        PageInstall,
        PageFinish,
    };

protected:
    struct PageInfo
    {
        CBasePage* pPage;
        PageIndex index;
    };

public:
    CPageManager(OControlManager* pManager);
    ~CPageManager(void);

public:
    void ShowPage(PageIndex index);

private:
    PageInfo* GetPageByIndex(PageIndex index);
    PageInfo* CreatePage(PageIndex index);

private:
    OControlManager* m_pControlManager;

    typedef std::vector<PageInfo> PageArray;
    PageArray   m_arrPages;

    PageInfo*   m_pLastPage;
};

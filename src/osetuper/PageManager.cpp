#include "stdafx.h"
#include "PageManager.h"


#include "WelcomePage.h"
#include "InstallPage.h"
#include "FinishPage.h"


CPageManager::CPageManager(OControlManager* pManager)
{
    m_pLastPage = NULL;
    m_pControlManager = pManager;
}

CPageManager::~CPageManager(void)
{
    size_t count = m_arrPages.size();
    for(size_t i=0; i<count; ++ i)
    {
        PageInfo& page = m_arrPages[i];
        if(page.pPage)
        {
            page.pPage->Destroy();
            delete page.pPage;
        }
    }
    m_arrPages.clear();
}

void CPageManager::ShowPage(PageIndex index)
{
    PageInfo* page = GetPageByIndex(index);

    if(page == NULL)
        page = CreatePage(index);

    if(m_pLastPage != NULL)
        m_pLastPage->pPage->Show(FALSE);
    page->pPage->Show(TRUE);

    m_pLastPage = page;
}

CPageManager::PageInfo* CPageManager::GetPageByIndex(PageIndex index)
{
    size_t count = m_arrPages.size();
    for(size_t i=0; i<count; ++ i)
    {
        PageInfo& page = m_arrPages[i];
        if(page.index == index)
        {
            return &page;
        }
    }
    return NULL;
}

CPageManager::PageInfo* CPageManager::CreatePage(PageIndex index)
{
    PageInfo page;
    page.index = index;

    switch(index)
    {
    case PageWelcome:
        page.pPage = new CWelcomePage(m_pControlManager);
        break;
    case PageInstall:
        page.pPage = new CInstallPage(m_pControlManager);
        break;
    case PageFinish:
        page.pPage = new CFinishPage(m_pControlManager);
        break;
    default:
        page.pPage = NULL;
        break;
    }
    if(page.pPage == NULL)
        return NULL;
    page.pPage->Create();
    m_arrPages.push_back(page);
    return &m_arrPages[m_arrPages.size() - 1];
}



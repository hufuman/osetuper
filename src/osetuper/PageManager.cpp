#include "stdafx.h"
#include "PageManager.h"


#include "WelcomePage.h"
#include "CustomPage.h"
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
        PageInfo* page = m_arrPages[i];
        if(page->pPage)
        {
            page->pPage->Destroy();
            delete page->pPage;
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

void CPageManager::PrevPage()
{
    if(m_pLastPage->index == PageWelcome)
        return;
    ShowPage((CPageManager::PageIndex)(m_pLastPage->index - 1));
}

void CPageManager::NextPage()
{
    if(m_pLastPage->index == PageFinish)
        return;
    ShowPage((CPageManager::PageIndex)(m_pLastPage->index + 1));
}

BOOL CPageManager::HandleMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
    UNREFERENCED_PARAMETER(lResult);
    UNREFERENCED_PARAMETER(lParam);
    if(m_pLastPage == NULL)
        return FALSE;
    if(message != WM_COMMAND || HIWORD(wParam) != BN_CLICKED)
        return FALSE;
    return m_pLastPage->pPage->OnCommand(LOWORD(wParam));
}

CPageManager::PageIndex CPageManager::GetCurPageIndex() const
{
    if(m_pLastPage == NULL)
        return PageWelcome;
    return m_pLastPage->index;
}

CPageManager::PageInfo* CPageManager::GetPageByIndex(PageIndex index)
{
    size_t count = m_arrPages.size();
    for(size_t i=0; i<count; ++ i)
    {
        PageInfo* page = m_arrPages[i];
        if(page->index == index)
        {
            return page;
        }
    }
    return NULL;
}

CPageManager::PageInfo* CPageManager::CreatePage(PageIndex index)
{
    PageInfo* page = new PageInfo;
    page->index = index;

    switch(index)
    {
    case PageWelcome:
        page->pPage = new CWelcomePage(m_pControlManager);
        break;
    case PageCustom:
        page->pPage = new CCustomPage(m_pControlManager);
        break;
    case PageInstall:
        // page->pPage = new CInstallPage(m_pControlManager);
        // break;
    case PageFinish:
        page->pPage = new CFinishPage(m_pControlManager);
        break;
    default:
        page->pPage = NULL;
        break;
    }
    if(page->pPage == NULL)
        return NULL;

    page->pPage->Create();
    m_arrPages.push_back(page);
    return page;
}



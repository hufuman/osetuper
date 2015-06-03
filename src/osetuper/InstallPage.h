#pragma once


#include "BasePage.h"
#include "SetupWorker.h"


class CInstallPage : public CBasePage, public ISetupStatusListener
{
public:
    CInstallPage(OControlManager* pManager);
    ~CInstallPage(void);

public:
    virtual void Create();

public:
    // ISetupStatusListener
    virtual void OnProgressChanged(int progress);
    virtual void OnProgressTextChanged(LPCTSTR szText);
    virtual void OnError(CString strError);
    virtual void OnFinish();

private:
    void UpdateProgressUI();

private:
    CSetupWorker m_Worker;

    DWORD m_dwProgress;
    CString m_strText;

    OShape* m_pProgressFore;
    OShape* m_pProgressBack;
    OLabel* m_pText;
};

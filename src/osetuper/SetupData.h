#pragma once



class CSetupData
{
    CSetupData(void);
    CSetupData(const CSetupData&);
    CSetupData& operator = (const CSetupData&);
public:
    ~CSetupData(void);

    static CSetupData& GetInst();

    void SetSetupDir(LPCTSTR szDir);
    CString GetSetupDir() const;

    void SetStart(BOOL bStart);
    BOOL GetStart() const;

private:
    BOOL m_bStart;
    CString m_strSetupDir;
};

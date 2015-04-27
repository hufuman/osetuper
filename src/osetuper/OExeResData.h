#pragma once

class OExeResData
{
public:
    OExeResData(void);
    ~OExeResData(void);

    BOOL LoadData(LPCTSTR szResName, LPCTSTR szType);

    LPBYTE GetData() const;
    DWORD GetSize() const;

private:
    void Release();

private:
    HRSRC   m_hRsrc;
    HGLOBAL m_hGlobal;
    void*   m_pData;
    DWORD   m_dwSize;
};

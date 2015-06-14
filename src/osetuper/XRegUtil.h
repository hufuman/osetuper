#pragma once


enum RegMode
{
    RM_ReadOnly,
    RM_WriteOnly,
    RM_ReadAndWrite,
};

class CRegUtil
{
    CRegUtil(const CRegUtil&);
    CRegUtil& operator = (const CRegUtil&);
public:
    CRegUtil();
    ~CRegUtil();

public:
    bool Open(HKEY hParentKey, const wchar_t * const path, RegMode mode);
    void Close();

    bool GetValue(const wchar_t* const name, std::wstring& strValue);
    bool SetValue(const wchar_t* const name, const wchar_t* const value);

    static bool GetStringValue(HKEY hKey, LPCTSTR szSubPath, LPCTSTR szValueName, std::wstring& value);
private:
    HKEY    m_hKey;
};




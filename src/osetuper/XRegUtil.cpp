#include "stdafx.h"
#include "XRegUtil.h"

CRegUtil::CRegUtil()
{
    m_hKey = NULL;
}

CRegUtil::~CRegUtil()
{
    Close();
}

bool CRegUtil::Open(HKEY hParentKey, const wchar_t * const path, RegMode mode)
{
    Close();

    REGSAM sam = KEY_READ;
    switch(mode)
    {
    default:
    case RM_ReadOnly:
        sam = KEY_READ;
        break;
    case RM_WriteOnly:
        sam = KEY_WRITE;
        break;
    case RM_ReadAndWrite:
        sam = KEY_READ | KEY_WRITE;
        break;
    }
    LONG lResult = ::RegCreateKeyEx(hParentKey, path, 0, 0, 0, sam, 0, &m_hKey, 0);
    bool bResult = (lResult == ERROR_SUCCESS && m_hKey != NULL);
    return bResult;
}

void CRegUtil::Close()
{
    if(m_hKey != NULL)
    {
        ::RegCloseKey(m_hKey);
        m_hKey = NULL;
    }
}

bool CRegUtil::GetValue(const wchar_t* const name, std::wstring& strValue)
{
    const int nSize = 1024;
    std::vector<BYTE> buffer;
    DWORD dwLength = 0;
    DWORD dwType = REG_NONE;

    buffer.resize(nSize);
    void* pBuffer = &buffer[0];
    LONG lResult = ::RegQueryValueEx(m_hKey, name, 0, &dwType, (LPBYTE)pBuffer, &dwLength);
    if((lResult != ERROR_MORE_DATA && lResult != ERROR_SUCCESS)
        || (dwType != REG_SZ)
        || dwLength == 0)
    {
        return false;
    }

    if(lResult == ERROR_MORE_DATA)
    {
        buffer.resize(dwLength);
        pBuffer = &buffer[0];
        lResult = ::RegQueryValueEx(m_hKey, name, 0, &dwType, (LPBYTE)pBuffer, &dwLength);
    }

    if(lResult != ERROR_SUCCESS)
        return false;

    strValue.assign((const wchar_t*)pBuffer, dwLength / sizeof(wchar_t));
    if(strValue.at(strValue.size() - 1) == 0)
        strValue.resize(strValue.size() - 1);
    return true;
}


bool CRegUtil::GetStringValue(HKEY hKey, LPCTSTR szSubPath, LPCTSTR szValueName, std::wstring& value)
{
    CRegUtil reg;
    if (!reg.Open(hKey, szSubPath, RM_ReadOnly))
        return false;
    return reg.GetValue(szValueName, value);
}

bool CRegUtil::SetValue(const wchar_t* const name, const wchar_t* const value)
{
    return ::RegSetValueEx(m_hKey, name, 0, REG_SZ, (const BYTE*)value, sizeof(TCHAR) * (wcslen(value) + 1)) == ERROR_SUCCESS;
}

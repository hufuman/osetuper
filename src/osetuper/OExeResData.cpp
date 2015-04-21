#include "StdAfx.h"
#include "OExeResData.h"

OExeResData::OExeResData(void)
{
    m_hRsrc = NULL;
    m_hGlobal = NULL;
    m_pData = NULL;
    m_dwSize = NULL;
}

OExeResData::~OExeResData(void)
{
    Release();
}

BOOL OExeResData::LoadData(LPCTSTR szResName, LPCTSTR szType)
{
    Release();

    HMODULE hModule = ::GetModuleHandle(NULL);
    m_hRsrc = ::FindResource(hModule, szResName, szType);
    if(m_hRsrc == NULL)
        return FALSE;
    m_hGlobal = ::LoadResource(hModule, m_hRsrc);
    if(m_hGlobal == NULL)
        return FALSE;
    m_pData = ::LockResource(m_hGlobal);
    m_dwSize = ::SizeofResource(hModule, m_hRsrc);
    return TRUE;
}

void* OExeResData::GetData() const
{
    return m_pData;
}

DWORD OExeResData::GetSize() const
{
    return m_dwSize;
}

void OExeResData::Release()
{
    if(m_pData == NULL)
        return;

    ::UnlockResource(m_pData);
    ::FreeResource(m_hGlobal);
    m_hRsrc = NULL;
    m_hGlobal = NULL;
    m_pData = NULL;
    m_dwSize = NULL;
}

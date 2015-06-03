#include "StdAfx.h"
#include "SetupData.h"

CSetupData::CSetupData(void)
{
    m_bStart = TRUE;
}

CSetupData::~CSetupData(void)
{
}

CSetupData& CSetupData::GetInst()
{
    static CSetupData inst;
    return inst;
}

void CSetupData::SetSetupDir(LPCTSTR szDir)
{
    m_strSetupDir = szDir;
}

CString CSetupData::GetSetupDir() const
{
    return m_strSetupDir;
}

void CSetupData::SetStart(BOOL bStart)
{
    m_bStart = bStart;
}

BOOL CSetupData::GetStart() const
{
    return m_bStart;
}
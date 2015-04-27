#include "StdAfx.h"
#include "StringBundle.h"


#include "OExeResData.h"

CStringBundle::CStringBundle(void)
{
}

CStringBundle::~CStringBundle(void)
{
}

CStringBundle& CStringBundle::GetInst()
{
    static CStringBundle inst;
    return inst;
}

void CStringBundle::Init()
{
    OExeResData data;
    data.LoadData(_T("Lang"), _T("BINARY"));
    LPBYTE pData = data.GetData();
    DWORD dwSize = data.GetSize();
    m_IniData.Parse(pData, dwSize);
}

CString CStringBundle::Get(CString strName) const
{
    return m_IniData.Get(strName);
}

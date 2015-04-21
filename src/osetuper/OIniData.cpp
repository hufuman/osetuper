#include "StdAfx.h"
#include "OIniData.h"

OIniData::OIniData(void)
{
}

OIniData::~OIniData(void)
{
}

BOOL OIniData::Parse(LPBYTE pData, int nDataCount)
{
    m_mapDatas.RemoveAll();

    if(pData[0] == 0xFF && pData[1] == 0xFE)
    {
        pData = pData + 2;
        nDataCount -= 2;
    }
    CString strLine, strKey, strValue;
    CString strData((LPCTSTR)pData, nDataCount / 2);
    int start = 0;
    BOOL bBreakLine = FALSE;
    for(;;)
    {
        strLine = strData.Tokenize(_T("\n"), start);
        if(start < 0 || strLine.IsEmpty())
            break;
        bBreakLine = strLine[strLine.GetLength() - 1] == _T('\r');
        int pos = strLine.Find(_T('='));
        if(pos <= 0)
            continue;
        strKey = strLine.Mid(0, pos);
        strValue = strLine.Mid(pos + 1, strLine.GetLength() - pos - (bBreakLine ? 2 : 1));
        m_mapDatas.SetAt(strKey, strValue);
    }
    return m_mapDatas.GetCount() > 0;
}

CString OIniData::Get(LPCTSTR szName) const
{
    CString strValue;
    if(!m_mapDatas.Lookup(szName, strValue))
        strValue = _T("");
    return strValue;
}

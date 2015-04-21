#pragma once

class OIniData
{
public:
    OIniData(void);
    ~OIniData(void);

    BOOL Parse(LPBYTE pData, int nDataCount);
    CString Get(LPCTSTR szName) const;

private:
    CMapStringToString m_mapDatas;
};

#pragma once


#include "OIniData.h"


class CStringBundle
{
    CStringBundle(void);
    CStringBundle(const CStringBundle&);
    CStringBundle& operator = (const CStringBundle&);
public:
    ~CStringBundle(void);

    static CStringBundle& GetInst();

    void Init();
    CString Get(CString strName) const;

private:
    OIniData m_IniData;
};

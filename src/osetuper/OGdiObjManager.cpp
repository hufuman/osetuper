#include "StdAfx.h"
#include "OGdiObjManager.h"

OGdiObjManager::OGdiObjManager(void)
{
}

OGdiObjManager::~OGdiObjManager(void)
{
    size_t count = m_arrFontData.size();
    for(size_t i=0; i<count; ++ i)
    {
        FontData* fontData = m_arrFontData[i];
        ::DeleteObject(fontData->hFont);
        delete fontData;
    }
    m_arrFontData.clear();
}

OGdiObjManager& OGdiObjManager::GetInst()
{
    static OGdiObjManager inst;
    return inst;
}

HFONT OGdiObjManager::GetFont(int fontSize)
{
    size_t count = m_arrFontData.size();
    for(size_t i=0; i<count; ++ i)
    {
        FontData* fontData = m_arrFontData[i];
        if(fontData->fontSize == fontSize)
            return fontData->hFont;
    }
    HFONT hFont = ::CreateFont(-fontSize, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("Tahoma"));
    FontData* fontData = new FontData;
    fontData->fontSize = fontSize;
    fontData->hFont = hFont;
    m_arrFontData.push_back(fontData);
    return hFont;
}

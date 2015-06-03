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

HFONT OGdiObjManager::GetFont(int fontSize, bool bold)
{
    return GetFontImpl(fontSize, bold, false);
}

HFONT OGdiObjManager::GetLinkFont(HFONT hFont)
{
    LOGFONT logFont;
    ::GetObject(hFont, sizeof(logFont), &logFont);
    return GetFontImpl(-logFont.lfHeight, logFont.lfWeight == FW_BOLD, true);
}

HFONT OGdiObjManager::GetFontImpl(int fontSize, bool bold, bool linkFont)
{
    size_t count = m_arrFontData.size();
    for(size_t i=0; i<count; ++ i)
    {
        FontData* fontData = m_arrFontData[i];
        if(fontData->fontSize == fontSize && fontData->bold == bold && fontData->linkFont == linkFont)
            return fontData->hFont;
    }

    HFONT hFont = ::CreateFont(-fontSize, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL, 0, linkFont ? 1 : 0, 0, GB2312_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("Tahoma"));
    FontData* fontData = new FontData;
    fontData->fontSize = fontSize;
    fontData->hFont = hFont;
    fontData->bold = bold;
    fontData->linkFont = linkFont;
    m_arrFontData.push_back(fontData);
    return hFont;
}
#pragma once





class OGdiObjManager
{
    struct FontData
    {
        int fontSize;
        bool bold;
        bool linkFont;
        HFONT hFont;
    };
    OGdiObjManager(void);
    OGdiObjManager(const OGdiObjManager&);
    OGdiObjManager operator = (const OGdiObjManager&);
public:
    ~OGdiObjManager(void);

    static OGdiObjManager& GetInst();

    HFONT GetFont(int fontSize, bool bold);
    HFONT GetLinkFont(HFONT hFont);

private:
    HFONT GetFontImpl(int fontSize, bool bold, bool linkFont);

private:
    typedef std::vector<FontData*> FontDataArray;
    FontDataArray   m_arrFontData;
};

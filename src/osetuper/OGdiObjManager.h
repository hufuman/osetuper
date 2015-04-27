#pragma once





class OGdiObjManager
{
    struct FontData
    {
        int fontSize;
        HFONT hFont;
    };
    OGdiObjManager(void);
    OGdiObjManager(const OGdiObjManager&);
    OGdiObjManager operator = (const OGdiObjManager&);
public:
    ~OGdiObjManager(void);

    static OGdiObjManager& GetInst();

    HFONT GetFont(int fontSize);

private:
    typedef std::vector<FontData*> FontDataArray;
    FontDataArray   m_arrFontData;
};

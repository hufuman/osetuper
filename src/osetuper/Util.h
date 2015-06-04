#pragma once



namespace Util
{
    DWORD GetModuleFileSize(HMODULE hModule);
    BOOL ExtractTo(LPCTSTR szPath);
    BOOL AlphaDraw(HBITMAP image,
        int srcX, int srcY, int srcWidth, int srcHeight,
        HDC hDstDc,
        int dstX, int dstY, int dstWidth, int dstHeight);

    HBITMAP LoadImage(LPCTSTR szImagePath);
    HBITMAP LoadImageFromExeRes(LPCTSTR szResName);

    ULONG_PTR InitGdiplus();
    void CleanGdiplus(ULONG_PTR uToken);
    BOOL IsRectsOverlapped(const RECT* pOne, const RECT* pAnother);

    BOOL IsKeyPressed(UINT virtualKey);

    CString AppendPath(LPCTSTR szSrc, LPCTSTR szMore);

    int GetTextHeight(HWND hWnd, HFONT hFont);

    CString BrowseForFolder(HWND hWnd, LPCTSTR szTitle, LPCTSTR szInitDir);

    BOOL IsDirExists(LPCTSTR szPath);
    CString RemovePath(LPCTSTR szSrcPath);
    CString MakeSureEndWith(LPCTSTR szSrcPath, LPCTSTR szEnd);
    DWORD GetSelfSize();
    BOOL MakeDirs(LPCTSTR szPath);
}

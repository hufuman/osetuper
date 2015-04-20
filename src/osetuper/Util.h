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

    BOOL InitGdiplus();
    BOOL IsRectsOverlapped(const RECT* pOne, const RECT* pAnother);

    BOOL IsKeyPressed(UINT virtualKey);
}

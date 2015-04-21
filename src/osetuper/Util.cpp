#include "stdafx.h"
#include "Util.h"

#include "XStreamImpl.h"
#include "OExeResData.h"


namespace Util
{
    DWORD GetModuleFileSize(HMODULE hModule)
    {
        LPVOID lpBuffer = (LPVOID)hModule;
        PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)lpBuffer;
        PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)((LPBYTE)lpBuffer + pDosHdr->e_lfanew); 
        LPBYTE pSectionHdr = (LPBYTE)((LPBYTE)lpBuffer + pDosHdr->e_lfanew + sizeof(IMAGE_NT_HEADERS));

        if (pNtHdr->FileHeader.NumberOfSections <= 0)
            return 0;

        int i = pNtHdr->FileHeader.NumberOfSections -1;
        PIMAGE_SECTION_HEADER pish = (PIMAGE_SECTION_HEADER)(pSectionHdr + i * sizeof(IMAGE_SECTION_HEADER));
        DWORD dwResPESize = pish->SizeOfRawData + pish->PointerToRawData;
        return dwResPESize;
    }

    class CHandleHolder
    {
        CHandleHolder();
        CHandleHolder(const CHandleHolder&);
        CHandleHolder& operator = (const CHandleHolder&);
    public:
        CHandleHolder(HANDLE hFile) : m_hFile(hFile)
        {
            ;
        }

        CHandleHolder& operator =(HANDLE hFile)
        {
            m_hFile = hFile;
            return (*this);
        }

        ~CHandleHolder()
        {
            if(IsValid())
                ::CloseHandle(m_hFile);
        }

        BOOL IsValid() const
        {
            return (m_hFile != NULL && m_hFile != INVALID_HANDLE_VALUE);
        }

        operator HANDLE() const
        {
            return m_hFile;
        }

    private:
        HANDLE m_hFile;
    };

    BOOL ExtractTo(LPCTSTR szPath)
    {
        CHandleHolder hOutputFile = ::CreateFile(szPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
        if(!hOutputFile.IsValid())
            return FALSE;

        TCHAR szExePath[1024];
        ::GetModuleFileName(NULL, szExePath, 1024);
        CHandleHolder hFile = ::CreateFile(szExePath, GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, NULL);
        if(!hFile.IsValid())
            return FALSE;

        DWORD dwFileSize = ::GetFileSize(hFile, NULL);
        DWORD dwExeSize = GetModuleFileSize(::GetModuleHandle(NULL));
        DWORD dwResSize = dwFileSize - dwExeSize;
        if(dwFileSize <= dwExeSize)
            return FALSE;

        LPVOID pBuffer = malloc(dwFileSize - dwExeSize);
        if(pBuffer == NULL)
            return FALSE;

        DWORD dwRead;
        if(::SetFilePointer(hFile, dwExeSize, 0, FILE_BEGIN) && !::ReadFile(hFile, pBuffer, dwResSize, &dwRead, NULL))
            return FALSE;

        DWORD dwWritten = 0;
        if(!::WriteFile(hOutputFile, pBuffer, dwResSize, &dwWritten, NULL))
            return FALSE;

        return TRUE;
    }

    BOOL AlphaDraw(HBITMAP image,
        int srcX, int srcY, int srcWidth, int srcHeight,
        HDC hDstDc,
        int dstX, int dstY, int dstWidth, int dstHeight)
    {
        HDC hMemDc = ::CreateCompatibleDC(hDstDc);
        HGDIOBJ hOldBmp = ::SelectObject(hMemDc, image);

        BLENDFUNCTION BlendFunc = {0};
        BlendFunc.BlendOp = AC_SRC_OVER;
        BlendFunc.SourceConstantAlpha = 255;
        BlendFunc.AlphaFormat = AC_SRC_ALPHA;
        BOOL bResult = ::AlphaBlend(
            hDstDc,
            dstX, dstY,
            dstWidth, dstHeight,
            hMemDc,
            srcX, srcY,
            srcWidth, srcHeight,
            BlendFunc);

        ::SelectObject(hMemDc, hOldBmp);
        ::DeleteDC(hMemDc);
        return bResult;
    }

    HBITMAP LoadImage(LPCTSTR szImagePath)
    {
        HBITMAP hResult = NULL;
        Gdiplus::Bitmap* pBitmap = Gdiplus::Bitmap::FromFile(szImagePath);
        if(pBitmap != NULL && pBitmap->GetLastStatus() == Gdiplus::Ok)
        {
            Gdiplus::Status status = pBitmap->GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &hResult);
            if(status != Gdiplus::Ok)
            {
                if(hResult != NULL)
                    ::DeleteObject(hResult);
                hResult = NULL;
            }
        }
        delete pBitmap;
        return hResult;
    }

    HBITMAP LoadImageFromExeRes(LPCTSTR szResName)
    {
        OExeResData data;
        if(!data.LoadData(szResName, _T("PNG")))
            return NULL;

        HBITMAP hResult = NULL;
        IStream* pStream = new XStreamImpl;
        ULONG uWritten;
        pStream->Write(data.GetData(), data.GetSize(), &uWritten);
        Gdiplus::Bitmap* pBitmap = Gdiplus::Bitmap::FromStream(pStream);
        if(pBitmap != NULL && pBitmap->GetLastStatus() == Gdiplus::Ok)
        {
            Gdiplus::Status status = pBitmap->GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &hResult);
            if(status != Gdiplus::Ok)
            {
                if(hResult != NULL)
                    ::DeleteObject(hResult);
                hResult = NULL;
            }
        }
        delete pBitmap;
        pStream->Release();
        return hResult;
    }

    BOOL InitGdiplus()
    {
        ULONG_PTR token;
        Gdiplus::GdiplusStartupInput input;
        Gdiplus::Status status = Gdiplus::GdiplusStartup(&token, &input, NULL);
        return (status == Gdiplus::Ok);
    }

    BOOL IsRectsOverlapped(const RECT* pOne, const RECT* pAnother)
    {
        if(pOne == NULL || pAnother == NULL)
            return TRUE;

        const LONG& l1 = pOne->left;
        const LONG& r1 = pOne->right;
        const LONG& t1 = pOne->top;
        const LONG& b1 = pOne->bottom;

        const LONG& l2 = pAnother->left;
        const LONG& r2 = pAnother->right;
        const LONG& t2 = pAnother->top;
        const LONG& b2 = pAnother->bottom;

        if(r1 <= l1
            || b1 <= t1
            || r2 <= l2
            || b2 <= t2)
            return FALSE;

        BOOL bResult = 
            (r1 < l2)
            || (r1 == l2 && (b1 < t2 || t1 > b2))
            || (r2 < l1)
            || (r2 == l1 && (b1 < t2 || t1 > b2))
            || (b1 < t2)
            || (b1 == t2 && (r1 < l2 || l1 > r2))
            || (t1 > b2)
            || (t1 == b2 && (r1 < l2 || l1 > r2))
            ;
        bResult = !bResult;
        return bResult;
    }

    BOOL IsKeyPressed(UINT virtualKey)
    {
        return ((GetKeyState(virtualKey) & 0x80000000) == 0x80000000);
    }

}

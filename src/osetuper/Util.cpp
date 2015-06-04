#include "stdafx.h"
#include "Util.h"

#include "XStreamImpl.h"
#include "OExeResData.h"
#include "resource.h"


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

    BOOL AlphaDrawNine(HBITMAP image,
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

        // Left
        BOOL bResult = ::AlphaBlend(
            hDstDc,
            dstX, dstY,
            1, dstHeight,
            hMemDc,
            srcX, srcY,
            1, srcHeight,
            BlendFunc);

        // Top
        bResult = ::AlphaBlend(
            hDstDc,
            dstX, dstY,
            dstWidth, 1,
            hMemDc,
            srcX, srcY,
            srcWidth, 1,
            BlendFunc) && bResult;

        // Right
        bResult = ::AlphaBlend(
            hDstDc,
            dstX + dstWidth - 1, dstY,
            1, dstHeight,
            hMemDc,
            srcX + srcWidth - 1, srcY,
            1, srcHeight,
            BlendFunc) && bResult;

        // Bottom
        bResult = ::AlphaBlend(
            hDstDc,
            dstX, dstY + dstHeight - 1,
            dstWidth, 1,
            hMemDc,
            srcX, srcY + srcHeight - 1,
            srcWidth, 1,
            BlendFunc) && bResult;

        // Center
        bResult = ::AlphaBlend(
            hDstDc,
            dstX + 1, dstY + 1,
            dstWidth - 2, dstHeight - 2,
            hMemDc,
            srcX + 1, srcY + 1,
            srcWidth - 2, srcHeight - 2,
            BlendFunc) && bResult;

        ::SelectObject(hMemDc, hOldBmp);
        ::DeleteDC(hMemDc);

        return bResult;
    }

    BOOL AlphaDraw(HBITMAP image,
        int srcX, int srcY, int srcWidth, int srcHeight,
        HDC hDstDc,
        int dstX, int dstY, int dstWidth, int dstHeight)
    {
        if(srcWidth != dstWidth || srcHeight != dstHeight)
            return AlphaDrawNine(image, srcX, srcY, srcWidth, srcHeight, hDstDc, dstX, dstY, dstWidth, dstHeight);

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

    ULONG_PTR InitGdiplus()
    {
        ULONG_PTR token;
        Gdiplus::GdiplusStartupInput input;
        Gdiplus::Status status = Gdiplus::GdiplusStartup(&token, &input, NULL);
        if(status == Gdiplus::Ok)
            return token;
        else
            return 0;
    }

    void CleanGdiplus(ULONG_PTR uToken)
    {
        Gdiplus::GdiplusShutdown(uToken);
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

    CString AppendPath(LPCTSTR szSrc, LPCTSTR szMore)
    {
        CString result(szSrc);
        result.Replace(_T('/'), _T('\\'));
        if(result.GetLength() > 0 && result[result.GetLength() - 1] != _T('\\'))
            result += _T("\\");
        result += szMore;
        return result;
    }

    int GetTextHeight(HWND hWnd, HFONT hFont)
    {
        HDC hDc = ::GetDC(hWnd);
        HGDIOBJ hOldObj = ::SelectObject(hDc, hFont);
        TEXTMETRIC tm;
        ::GetTextMetrics(hDc, &tm);

        int nExtra = tm.tmHeight < 15 ? 2 : 5;
        int result = tm.tmHeight + nExtra;

        ::SelectObject(hDc, hOldObj);
        ::ReleaseDC(hWnd, hDc);
        return result;
    }

    struct stBrowserCallbackParam
    {
        LPCTSTR szTitle;
        LPCTSTR szInitDir;
    };

    int CALLBACK BrowserCallbackProc(HWND hWnd,UINT uMsg,LPARAM lParam,LPARAM lpData) 
    { 
        UNREFERENCED_PARAMETER(lParam);
        switch (uMsg) 
        { 
        case BFFM_INITIALIZED: 
            {
                stBrowserCallbackParam* param = (stBrowserCallbackParam*)lpData;
                ::SendMessage(hWnd, BFFM_SETSELECTION, 1, (LPARAM)param->szInitDir);
                if(param->szTitle != NULL && param->szTitle[0] != 0)
                    ::SetWindowText(hWnd, param->szTitle);
                HICON hSmallIcon = (HICON)::LoadImage(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16, 16, LR_COLOR);
                HICON hBigIcon = (HICON)::LoadImage(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 32, 32, LR_COLOR);

                ::SendMessage(hWnd, WM_SETICON, TRUE, (LPARAM)hBigIcon);
                ::SendMessage(hWnd, WM_SETICON, FALSE, (LPARAM)hSmallIcon);
            }
            break; 
        default: 
            break; 
        } 
        return 0; 
    }

    CString BrowseForFolder(HWND hWnd, LPCTSTR szTitle, LPCTSTR szInitDir)
    {

        stBrowserCallbackParam param;
        param.szTitle = szTitle;
        param.szInitDir = szInitDir;

        BROWSEINFO _info = { 0 };
        _info.hwndOwner = hWnd;
        _info.lpfn = &BrowserCallbackProc;
        _info.lParam = (LPARAM)&param;
        _info.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;
        PIDLIST_ABSOLUTE idlResult = SHBrowseForFolder(&_info);
        if (idlResult == NULL)
        {
            return _T("");
        }

        CString strResult;
        TCHAR szFilePath[MAX_PATH] = { 0 };
        if (::SHGetPathFromIDList(idlResult, szFilePath))
        {
            strResult = szFilePath;
        }
        return strResult;
    }

    BOOL IsDirExists(LPCTSTR szPath)
    {
        DWORD dwAttr = ::GetFileAttributes(szPath);
        return dwAttr != INVALID_FILE_ATTRIBUTES && ((dwAttr & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
    }

    CString RemovePath(LPCTSTR szSrcPath)
    {
        if(szSrcPath == NULL || szSrcPath[0] == 0)
            return _T("");

        CString strResult(szSrcPath);
        strResult.Replace(_T('/'), _T('\\'));
        if(strResult[strResult.GetLength() - 1] == _T('\\'))
            strResult = strResult.Mid(0, strResult.GetLength() - 1);
        int pos = strResult.ReverseFind(_T('\\'));
        if(pos > 0)
            strResult = strResult.Mid(0, pos);
        return strResult;
    }

    CString MakeSureEndWith(LPCTSTR szSrcPath, LPCTSTR szEnd)
    {
        CString strResult(szSrcPath);
        strResult.MakeLower();
        CString strEnd(szEnd);
        strEnd.MakeLower();
        int lastPos = 0;
        for(;;)
        {
            int pos = strResult.Find(strEnd, lastPos);
            if(pos < 0)
                break;
            lastPos = pos + 1;
        }
        if(lastPos > 0 && lastPos + strEnd.GetLength() == strResult.GetLength())
            return szSrcPath;
        return AppendPath(szSrcPath, szEnd);
    }

    DWORD GetSelfSize()
    {
        HMODULE hModule = ::GetModuleHandle(NULL);
        PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)hModule;
        PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)((LPBYTE)hModule + pDosHdr->e_lfanew); 
        LPBYTE pSectionHdr = (LPBYTE)((LPBYTE)hModule + pDosHdr->e_lfanew + sizeof(IMAGE_NT_HEADERS));

        if (pNtHdr->FileHeader.NumberOfSections <= 0)
            return static_cast<DWORD>(-1);

        int i = pNtHdr->FileHeader.NumberOfSections -1;
        PIMAGE_SECTION_HEADER pish = (PIMAGE_SECTION_HEADER)(pSectionHdr + i * sizeof(IMAGE_SECTION_HEADER));
        DWORD dwResPESize = pish->SizeOfRawData + pish->PointerToRawData;
        return dwResPESize;
    }

    BOOL MakeDirs(LPCTSTR szPath)
    {
        if(IsDirExists(szPath))
            return TRUE;
        return ::CreateDirectory(szPath, NULL);
    }
}

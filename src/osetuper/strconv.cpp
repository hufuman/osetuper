#include "stdafx.h"
#include "strconv.h"


strconv_t::strconv_t()
{
}
strconv_t::~strconv_t()
{
    m_vctBufers.clear();
}

LPCWSTR strconv_t::a2w(__in_opt LPCSTR lpsz, __in int nSz /* = -1 */)
{
    LPCWSTR szResult = NULL;
    do 
    {
        if(lpsz==NULL)
            break;

        int count = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpsz, nSz, NULL, 0);
        if(count==0)
            break;

        ++ count;
        void* pBuffer = (void*)GetBuffer(count * sizeof(wchar_t));
        int result = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpsz, nSz, (LPWSTR)pBuffer, count);
        if(result==0)
        {
            break;
        }

        ((LPWSTR)pBuffer)[count-1] = 0;
        szResult = (LPCWSTR)pBuffer;
    } while (FALSE);
    return (szResult == NULL) ? L"" : szResult;
}

LPCSTR strconv_t::w2a(__in_opt LPCWSTR lpsz, __in int nSz /* = -1 */)
{
    LPCSTR szResult = NULL;
    do 
    {
        if(lpsz==NULL)
            break;

        int count = WideCharToMultiByte(CP_ACP, 0, lpsz, nSz, NULL, 0, NULL, NULL);
        if(count==0)
            break;

        ++ count;
        void* pBuffer = (void*)GetBuffer(sizeof(char) * count);
        int result = WideCharToMultiByte(CP_ACP, 0, lpsz, nSz, (LPSTR)pBuffer, count, NULL, NULL);
        if(result==0)
        {
            break;
        }

        ((LPSTR)pBuffer)[count-1] = 0;
        szResult = (LPCSTR)pBuffer;
    } while (FALSE);
    return (szResult == NULL) ? "" : szResult;
}

// Converts UNICODE little endian string to UNICODE big endian 
LPCWSTR strconv_t::w2w_be(__in_opt LPCWSTR lpsz, UINT cch)
{
    LPCWSTR szResult = NULL;
    do 
    {
        if(lpsz==NULL)
            break;

        ++ cch;
        wchar_t* pBuffer = (wchar_t*)GetBuffer(sizeof(wchar_t) * (cch+1));
        UINT i;
        for(i=0; i<cch; i++)
        {
            // Swap bytes
            pBuffer[i] = (wchar_t)MAKEWORD((lpsz[i]>>8), (lpsz[i]&0xFF));
        }

        pBuffer[cch] = 0; // Zero terminator

        szResult = (LPCWSTR)pBuffer;
    } while (FALSE);
    return (szResult == NULL) ? L"" : szResult;
}

LPCSTR strconv_t::a2utf8(__in_opt LPCSTR lpsz, __in int nSz /* = -1 */)
{
    LPCSTR szResult = NULL;
    do 
    {
        if(lpsz==NULL)
            break;

        // 1. Convert input ANSI string to widechar using 
        // MultiByteToWideChar(CP_ACP, ...) function (CP_ACP 
        // is current Windows system Ansi code page)

        // Calculate required buffer size
        int count = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpsz, nSz, NULL, 0);
        if(count==0)
            break;

        // Convert ANSI->UNICODE
        ++ count;
        wchar_t* pBuffer = (wchar_t*)GetBuffer(sizeof(wchar_t) * count);
        int result = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpsz, nSz, (LPWSTR)pBuffer, count);
        if(result==0)
        {
            break;
        }

        // 2. Convert output widechar string from previous call to 
        // UTF-8 using WideCharToMultiByte(CP_UTF8, ...)  function

        pBuffer[count-1] = 0;
        szResult = (LPCSTR)w2utf8(pBuffer);
    } while (FALSE);
    return (szResult == NULL) ? "" : szResult;
}

LPCSTR strconv_t::w2utf8(__in_opt LPCWSTR lpsz, __in int nSz /* = -1 */)
{
    LPCSTR szResult = NULL;
    do 
    {
        if(lpsz==NULL)
            break;

        // Calculate required buffer size
        int count = WideCharToMultiByte(CP_UTF8, 0, lpsz, nSz, NULL, 0, NULL, NULL);
        if(count==0)
        {
            break;
        }

        // Convert UNICODE->UTF8
        ++ count;
        LPSTR pBuffer = (LPSTR)GetBuffer(sizeof(char) * count);
        int result = WideCharToMultiByte(CP_UTF8, 0, lpsz, nSz, (LPSTR)pBuffer, count, NULL, NULL);
        if(result==0)
        {
            break;
        }

        ((LPSTR)pBuffer)[count-1] = 0;
        szResult = (LPCSTR)pBuffer;
    } while (FALSE);
    return (szResult == NULL) ? "" : szResult;
}

LPCWSTR strconv_t::utf82w(__in_opt LPCSTR lpsz, __in int nSz /* = -1 */)
{
    LPCWSTR szResult = NULL;
    do 
    {
        if(lpsz==NULL)
            break;

        // Calculate required buffer size
        int count = MultiByteToWideChar(CP_UTF8, 0, lpsz, nSz, NULL, 0);
        if(count==0)
        {
            break;
        }

        // Convert UNICODE->UTF8
        ++ count;
        LPWSTR pBuffer = (LPWSTR)GetBuffer(sizeof(wchar_t) * count);
        int result = MultiByteToWideChar(CP_UTF8, 0, lpsz, nSz, (LPWSTR)pBuffer, count);
        if(result==0)
        {
            break;
        }

        ((LPWSTR)pBuffer)[count-1] = 0;
        szResult = (LPCWSTR)pBuffer;
    } while (FALSE);
    return (szResult == NULL) ? L"" : szResult;
}

LPCWSTR strconv_t::utf82w(__in_opt LPCSTR pStr, UINT cch)
{
    LPCWSTR szResult = NULL;
    do 
    {
        if(pStr==NULL)
            break;

        // Calculate required buffer size
        int count = MultiByteToWideChar(CP_UTF8, 0, pStr, cch, NULL, 0);
        if(count==0)
        {
            break;
        }

        // Convert UNICODE->UTF8
        ++ count;
        LPWSTR pBuffer = (LPWSTR)GetBuffer(sizeof(wchar_t) * (count+1));
        int result = MultiByteToWideChar(CP_UTF8, 0, pStr, cch, (LPWSTR)pBuffer, count);
        if(result==0)
        {
            break;
        }

        // Zero-terminate
        pBuffer[count-1]=0;

        szResult = (LPCWSTR)pBuffer;
    } while (FALSE);
    return (szResult == NULL) ? L"" : szResult;
}

LPCSTR strconv_t::utf82a(__in_opt LPCSTR lpsz)
{
    return w2a(utf82w(lpsz));
}

LPCTSTR strconv_t::utf82t(__in_opt LPCSTR lpsz)
{
#ifdef UNICODE    
    return utf82w(lpsz);
#else
    return utf82a(lpsz);
#endif
}

LPCSTR strconv_t::t2a(__in_opt LPCTSTR lpsz)
{
#ifdef UNICODE    
    return w2a(lpsz);
#else
    return lpsz;
#endif
}

LPCWSTR strconv_t::t2w(__in_opt LPCTSTR lpsz)
{
#ifdef UNICODE    
    return lpsz;
#else
    return a2w(lpsz);
#endif
}

LPCTSTR strconv_t::a2t(__in_opt LPCSTR lpsz)
{
#ifdef UNICODE    
    return a2w(lpsz);
#else
    return lpsz;
#endif
}

LPCTSTR strconv_t::w2t(__in_opt LPCWSTR lpsz)
{
#ifdef UNICODE    
    return lpsz;
#else
    return w2a(lpsz);
#endif
}

LPCSTR strconv_t::t2utf8(__in_opt LPCTSTR lpsz, __in int nSz /* = -1 */)
{
#ifdef UNICODE    
    return w2utf8(lpsz, nSz);
#else
    return a2utf8(lpsz, nSz);
#endif
}

void* strconv_t::GetBuffer(DWORD dwSize)
{
    std::vector<BYTE> vct;
    m_vctBufers.push_back(vct);

    std::vector<BYTE>& result = m_vctBufers[m_vctBufers.size() - 1];
    result.resize(dwSize);
    return (void*)&result[0];
}


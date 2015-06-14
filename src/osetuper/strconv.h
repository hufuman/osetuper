/************************************************************************************* 
This file is a part of CrashRpt library.

Copyright (c) 2003, Michael Carruth
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this 
list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution.

* Neither the name of the author nor the names of its contributors 
may be used to endorse or promote products derived from this software without 
specific prior written permission.


THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************************/

// File: strconv.h
// Description: String conversion class
// Author: zexspectrum
// Date: 2009-2010

#ifndef _STRCONV_H
#define _STRCONV_H

#include <strstream>


class strconv_t
{
public:
    strconv_t();
    ~strconv_t();

    LPCWSTR a2w(__in_opt LPCSTR lpsz, __in int nSz = -1);
    LPCSTR w2a(__in_opt LPCWSTR lpsz, __in int nSz = -1);

    // Converts UNICODE little endian string to UNICODE big endian 
    LPCWSTR w2w_be(__in_opt LPCWSTR lpsz, UINT cch);

    LPCSTR a2utf8(__in_opt LPCSTR lpsz, __in int nSz = -1);

    LPCSTR w2utf8(__in_opt LPCWSTR lpsz, __in int nSz = -1);

    LPCWSTR utf82w(__in_opt LPCSTR lpsz, __in int nSz = -1);

    LPCWSTR utf82w(__in_opt LPCSTR pStr, UINT cch);

    LPCSTR utf82a(__in_opt LPCSTR lpsz);

    LPCTSTR utf82t(__in_opt LPCSTR lpsz);

    LPCSTR t2a(__in_opt LPCTSTR lpsz);

    LPCWSTR t2w(__in_opt LPCTSTR lpsz);

    LPCTSTR a2t(__in_opt LPCSTR lpsz);

    LPCTSTR w2t(__in_opt LPCWSTR lpsz);

    LPCSTR t2utf8(__in_opt LPCTSTR lpsz, __in int nSz = -1);

private:
    void* GetBuffer(DWORD dwSize);

private:
    std::vector<std::vector<BYTE> > m_vctBufers;
};

#endif  //_STRCONV_H



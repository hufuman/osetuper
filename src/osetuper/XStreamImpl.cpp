#include "stdafx.h"
#include "XStreamImpl.h"



XStreamImpl::XStreamImpl()
{
#ifdef TEST_STREAM
    m_pStream = NULL;
    CreateStreamOnHGlobal(NULL, TRUE, &m_pStream);
#endif // TEST_STREAM

    m_uRef = 1;
    m_pData = NULL;
    m_uLength = 0;
    m_dwCurPos = 0;
}

XStreamImpl::~XStreamImpl()
{
#ifdef TEST_STREAM
    m_pStream->Release();
    m_pStream = NULL;
#endif // TEST_STREAM

    m_pData = NULL;
    m_uLength = 0;
    m_dwCurPos = 0;
}

HRESULT XStreamImpl::QueryInterface(REFIID riid, void** ppvObject)
{
    if(memcmp(&IID_IUnknown, &riid, sizeof(IID)) == 0)
        *ppvObject = dynamic_cast<IUnknown*>(this);
    else if(memcmp(&IID_IStream, &riid, sizeof(IID)) == 0)
        *ppvObject = dynamic_cast<IStream*>(this);
    else
        return E_NOINTERFACE;

    AddRef();
    return S_OK;
}

ULONG XStreamImpl::AddRef(void)
{
    return (++ m_uRef);
}

ULONG XStreamImpl::Release(void)
{
    if((-- m_uRef) == 0)
    {
        delete this;
        return 0;
    }
    return m_uRef;
}

HRESULT XStreamImpl::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
{
#ifdef TEST_STREAM
    ULARGE_INTEGER uResult;
    DWORD dwCurPos = m_dwCurPos;
    HRESULT hResult = m_pStream->Seek(dlibMove, dwOrigin, &uResult);
#endif // TEST_STREAM

    switch(dwOrigin)
    {
    case STREAM_SEEK_SET:
        m_dwCurPos = (int)dlibMove.LowPart;
        break;

    case STREAM_SEEK_CUR:
        m_dwCurPos += dlibMove.LowPart;
        break;

    case STREAM_SEEK_END:
        m_dwCurPos = m_uLength - (int)dlibMove.LowPart;
        break;
    }

    if(m_dwCurPos < 0 || m_dwCurPos > m_uLength)
        return STG_E_INVALIDFUNCTION;

    if(plibNewPosition != NULL)
    {
        plibNewPosition->HighPart = 0;
        plibNewPosition->LowPart = m_dwCurPos;
    }

#ifdef TEST_STREAM
    if(plibNewPosition != NULL && (memcmp(&uResult, plibNewPosition, sizeof(uResult)) != 0))
    {
        int a = 0;
    }
#endif // TEST_STREAM
    return S_OK;
}

HRESULT XStreamImpl::Stat(__RPC__out STATSTG *pstatstg, DWORD /*grfStatFlag*/)
{
#ifdef TEST_STREAM
    STATSTG statstg;
    HRESULT hResult = m_pStream->Stat(&statstg, grfStatFlag);
#endif // TEST_STREAM

    memset(pstatstg, 0, sizeof(STATSTG));
    pstatstg->cbSize.HighPart = 0;
    pstatstg->cbSize.LowPart = m_uLength;
    pstatstg->type = STGTY_STREAM;

#ifdef TEST_STREAM
    if(memcmp(&statstg, pstatstg, sizeof(statstg)) != 0)
    {
        int a = 0;
    }
#endif // TEST_STREAM

    return S_OK;
}

HRESULT XStreamImpl::Read(void *pv, ULONG cb, ULONG *pcbRead)
{
#ifdef TEST_STREAM
    void* p = malloc(cb);
    ULONG uRightRead = 0;
    HRESULT hResult = m_pStream->Read(p, cb, &uRightRead);
#endif // TEST_STREAM

    DWORD dwRead = 0;
    dwRead = (m_dwCurPos + cb >= m_uLength) ? m_uLength - m_dwCurPos : cb;
    memcpy(pv, (BYTE*)m_pData + m_dwCurPos, dwRead);
    m_dwCurPos += dwRead;
    if(pcbRead != NULL)
    {
        *pcbRead = dwRead;
    }

#ifdef TEST_STREAM
    if(pcbRead != NULL)
    {
        if(*pcbRead != uRightRead)
        {
            int a = 0;
        }
    }
    if(memcmp(pv, p, uRightRead) != 0)
    {
        int a = 0;
    }
    free(p);
#endif // TEST_STREAM

    return S_OK;
}

HRESULT XStreamImpl::Write(const void *pv, ULONG cb, ULONG *pcbWritten)
{
#ifdef TEST_STREAM
    HRESULT hResult = m_pStream->Write(pv, cb, pcbWritten);
#endif // TEST_STREAM

    m_pData = pv;
    m_uLength = cb;
    m_dwCurPos = cb;
    if(pcbWritten != NULL)
    {
        *pcbWritten = cb;
    }
    return S_OK;
}


HRESULT XStreamImpl::SetSize(ULARGE_INTEGER /*libNewSize*/)
{
    return E_NOTIMPL;
}

HRESULT XStreamImpl::CopyTo(IStream * /*pstm*/, ULARGE_INTEGER /*cb*/, ULARGE_INTEGER * /*pcbRead*/, ULARGE_INTEGER * /*pcbWritten*/)
{
    return E_NOTIMPL;
}

HRESULT XStreamImpl::Commit(DWORD /*grfCommitFlags*/)
{
    return E_NOTIMPL;
}

HRESULT XStreamImpl::Revert(void)
{
    return E_NOTIMPL;
}

HRESULT XStreamImpl::LockRegion(ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/, DWORD /*dwLockType*/)
{
    return E_NOTIMPL;
}

HRESULT XStreamImpl::UnlockRegion(ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/, DWORD /*dwLockType*/)
{
    return E_NOTIMPL;
}

HRESULT XStreamImpl::Clone(__RPC__deref_out_opt IStream ** /*ppstm*/)
{
    return E_NOTIMPL;
}
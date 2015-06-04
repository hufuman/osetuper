#pragma once

#include "StringBundle.h"

template < typename T >
class BaseDlg : public CDialogImpl<T>
{
public:
    BaseDlg()
    {
        m_strAppName = CStringBundle::GetInst().Get(_T("TITLE"));
    }
    ~BaseDlg()
    {
        ;
    }

    int MsgBox(LPCTSTR szText, DWORD dwFlags)
    {
        return ::MessageBox(m_hWnd, szText, m_strAppName, dwFlags);
    }

private:
    CString m_strAppName;
};
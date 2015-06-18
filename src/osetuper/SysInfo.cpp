#include "StdAfx.h"
#include "SysInfo.h"

#include "./strconv.h"
#include "./xregutil.h"

#include <IPTypes.h>
#include <IPHlpApi.h>
#include <intrin.h>
#include <WbemCli.h>
#include <comutil.h>
#include <WbemProv.h>

#pragma warning(disable: 4996)
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"Iphlpapi.lib")
#pragma comment(lib,"Wbemuuid.lib")

#ifdef _DEBUG
#pragma comment(lib, "comsuppwd.lib")
#else
#pragma comment(lib, "comsuppw.lib")
#endif // _DEBUG


CSysInfo::CSysInfo(void)
{
    m_CpuInfo.dwCount = 0;
}

CSysInfo::~CSysInfo(void)
{
}

std::wstring CSysInfo::GetBoardSn()
{
    HRESULT hres = E_FAIL;
    HRESULT hInit = E_FAIL;
    IWbemLocator *pLoc = NULL;
    IWbemServices *pSvc = NULL;
    IEnumWbemClassObject* pEnumerator = NULL;
    IWbemClassObject *pclsObj = NULL;
    std::wstring ret = L"";

    do
    {
        hInit = CoInitializeEx(0, COINIT_MULTITHREADED);

        if (FAILED(hInit) && (hInit != RPC_E_CHANGED_MODE))
        {
            break;
        }

        hres = CoInitializeSecurity(
            NULL,
            -1,
            NULL,
            NULL,
            RPC_C_AUTHN_LEVEL_DEFAULT,
            RPC_C_IMP_LEVEL_IMPERSONATE,
            NULL,
            EOAC_NONE,
            NULL
            );
        if (FAILED(hres))
        {
            break;
        }

        hres = CoCreateInstance(
            CLSID_WbemLocator,
            0,
            CLSCTX_INPROC_SERVER,
            IID_IWbemLocator,
            (LPVOID*)&pLoc
            );
        if (FAILED(hres))
        {
            break;
        }

        hres = pLoc->ConnectServer(
            _bstr_t(L"ROOT\\CIMV2"),
            NULL,
            NULL,
            0,
            NULL,
            0,
            0,
            &pSvc
            );
        if (FAILED(hres))
        {
            break;
        }

        hres = CoSetProxyBlanket(
            pSvc,
            RPC_C_AUTHN_WINNT,
            RPC_C_AUTHZ_NONE,
            NULL,
            RPC_C_AUTHN_LEVEL_CALL,
            RPC_C_IMP_LEVEL_IMPERSONATE,
            NULL,
            EOAC_NONE
            );
        if (FAILED(hres))
        {
            break;
        }

        hres = pSvc->ExecQuery(
            bstr_t("WQL"),
            bstr_t("SELECT SerialNumber FROM Win32_BaseBoard"),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL,
            &pEnumerator
            );
        if (FAILED(hres))
        {
            break;
        }

        ULONG uReturn = 0;

        while (pEnumerator)
        {
            HRESULT hr = pEnumerator->Next(
                WBEM_INFINITE,
                1,
                &pclsObj,
                &uReturn
                );
            if (0 == uReturn)
            {
                break;
            }

            VARIANT vtProp;

            hr = pclsObj->Get(
                L"SerialNumber",
                0,
                &vtProp,
                0, 0
                );
            if (WBEM_S_NO_ERROR == hr)
            {
                if (VT_BSTR == vtProp.vt)
                {
                    ret = vtProp.bstrVal;
                }
            }
            VariantClear(&vtProp);
        }

    } while (FALSE);

    if (pSvc != NULL)
    {
        pSvc->Release();
    }
    if (pLoc != NULL)
    {
        pLoc->Release();
    }
    if (NULL != pEnumerator)
    {
        pEnumerator->Release();
    }
    if (NULL != pclsObj)
    {
        pclsObj->Release();
    }
    if (hInit != RPC_E_CHANGED_MODE)
    {
        CoUninitialize();
    }

    if (_tcsicmp(ret.c_str(), L"none") == 0)
    {
        ret = L"";
    }

    return ret;
}

std::wstring CSysInfo::GetOsVersion()
{
    if(!m_OsVersion.empty())
        return m_OsVersion;

    m_OsVersion = GetOsVersionImpl();
    return m_OsVersion;
}

const stCpuInfo& CSysInfo::GetCpuInfo()
{
    if(m_CpuInfo.dwCount != 0)
        return m_CpuInfo;

    GetCpuInfoImpl(m_CpuInfo);
    return m_CpuInfo;
}

BOOL CSysInfo::IsSys64()
{
    typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
    LPFN_ISWOW64PROCESS fnIsWow64Process;

    fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(GetModuleHandle(TEXT("kernel32")),"IsWow64Process");
    if(NULL != fnIsWow64Process)
    {
        BOOL bIsWow64 = FALSE;
        if(fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
            return bIsWow64;
    }
    return FALSE;
}

DWORD CSysInfo::GetMemSize()
{
    MEMORYSTATUSEX memory = {sizeof(MEMORYSTATUSEX)};
    if(!::GlobalMemoryStatusEx(&memory))
        return 0;
    return (DWORD)(memory.ullTotalPhys / 1024 / 1024);
}

void CSysInfo::GetScreenSize(DWORD& dwWidth, DWORD& dwHeight)
{
    dwWidth = GetSystemMetrics(SM_CXSCREEN);
    dwHeight = GetSystemMetrics(SM_CYSCREEN);
}

const std::vector<CVideoCardInfo>& CSysInfo::GetVideoCardInfos()
{
    if(m_vctVideoCards.size() > 0)
        return m_vctVideoCards;
    GetVideoCardInfosImpl(m_vctVideoCards);
    return m_vctVideoCards;
}

std::wstring CSysInfo::GetOsVersionImpl()
{
    TCHAR szTemp[500];
    OSVERSIONINFO ex = {sizeof(OSVERSIONINFO)};
    ::GetVersionEx(&ex);

    _sntprintf(szTemp,
        500,
        _T("%d.%d.%d.%s"),
        ex.dwMajorVersion,
        ex.dwMinorVersion,
        ex.dwBuildNumber,
        ex.szCSDVersion);
    return szTemp;
}

typedef struct _CPU_REGS
{
    INT Eax;
    INT Ebx;
    INT Ecx;
    INT Edx;

}CPU_REGS, *PCPU_REGS;

bool CSysInfo::GetCpuInfoImpl(stCpuInfo& cpuInfo)
{
    CPU_REGS CpuReg = {0};

    INT Reg[4] = {0};

    __try
    {
        SYSTEM_INFO sysInfo;
        ::GetSystemInfo(&sysInfo);
        cpuInfo.dwCount = sysInfo.dwNumberOfProcessors;
        cpuInfo.dwType = sysInfo.dwProcessorType;
        CRegUtil::GetStringValue(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"), _T("ProcessorNameString"), cpuInfo.strName);

        __cpuid(Reg, 1);

        CpuReg.Eax = Reg[0];
        CpuReg.Ebx = Reg[1];
        CpuReg.Ecx = Reg[2];
        CpuReg.Edx = Reg[3];

        TCHAR szTemp[100];
        _sntprintf(szTemp, 100, L"%0.8X", CpuReg.Eax);
        cpuInfo.strCpuId = szTemp;

        return true;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}

bool CSysInfo::GetVideoCardInfosImpl(std::vector<CVideoCardInfo>& vctVideoCards)
{
    CVideoCardInfo cardInfo;
    cardInfo.dwMemSize = 0;
    CRegUtil::GetStringValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winsat"), _T("PrimaryAdapterString"), cardInfo.strName);
    int nSize;
    CRegUtil::GetIntValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winsat"), _T("VideoMemorySize"), nSize);
    if(nSize > 0)
        cardInfo.dwMemSize = nSize / 1024 / 1024;
    vctVideoCards.push_back(cardInfo);
    return !cardInfo.strName.empty();
}

/*
bool CSysInfo::GetVideoCardInfosImpl(std::vector<CVideoCardInfo>& vctVideoCards)
{
    IWbemLocator* spLoc = NULL;
    IWbemServices* spServices = NULL;
    IWbemClassObject* spInstance = NULL;
    IEnumWbemClassObject* spEnumInst = NULL;
    _variant_t varProperty;
    _bstr_t bstrPropName;
    _bstr_t bstrNamespace(L"root\\cimv2");

    ULONG uNumOfInstances;
    HRESULT hrNext;

    bool result = false;
    for(;;)
    {
        if(FAILED(CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, 0)))
            break;

        if(FAILED(::CoCreateInstance(CLSID_WbemAdministrativeLocator, NULL, CLSCTX_INPROC_SERVER|CLSCTX_LOCAL_SERVER, __uuidof(IWbemLocator), (void**)&spLoc)))
            break;

        if(FAILED(spLoc->ConnectServer(bstrNamespace, NULL, NULL, 0, NULL, 0, 0, &spServices)))
            break;

        if(FAILED(CoSetProxyBlanket(spServices, RPC_C_AUTHN_DEFAULT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, 
            RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE)))
            break;

        if(FAILED(spServices->CreateInstanceEnum(_bstr_t(L"Win32_VideoController"), WBEM_FLAG_SHALLOW, NULL, &spEnumInst)))
            break;

        while(true) 
        {
            uNumOfInstances = 0;
            hrNext = spEnumInst->Next(10000, 1, &spInstance, &uNumOfInstances);

            if(hrNext == WBEM_S_FALSE) 
                break;

            if(hrNext == WBEM_S_NO_ERROR) 
            {
                CVideoCardInfo card;

                bstrPropName = L"AdapterRAM";
                if(SUCCEEDED(spInstance->Get(bstrPropName,0,&varProperty,NULL,0)))
                    card.dwMemSize = (int)varProperty/1024/1024;
                else
                    card.dwMemSize = 0;

                bstrPropName = L"Description";
                if(SUCCEEDED(spInstance->Get(bstrPropName,0,&varProperty,NULL,0)))
                    card.strName = (wchar_t *)(varProperty.pbstrVal);
                else
                    card.strName = L"";

                vctVideoCards.push_back(card);
            } 
            spInstance->Release();
        }

        result = true;
        break;
    }
    if(spLoc != NULL)
        spLoc->Release();
    if(spServices != NULL)
        spServices->Release();
    if(spEnumInst != NULL)
        spEnumInst->Release();
    return result;
}
*/
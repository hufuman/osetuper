#pragma once


struct stCpuInfo
{
    std::wstring strName;
    DWORD dwCount;
    std::wstring strCpuId;
    DWORD dwType;
};

struct CVideoCardInfo
{
    std::wstring strName;
    DWORD dwMemSize;
};

class CSysInfo
{
public:
    CSysInfo(void);
    ~CSysInfo(void);

    std::wstring GetBoardSn();
    std::wstring GetOsVersion();
    const stCpuInfo& GetCpuInfo();
    BOOL IsSys64();
    DWORD GetMemSize();
    void GetScreenSize(DWORD& dwWidth, DWORD& dwHeight);
    const std::vector<CVideoCardInfo>& GetVideoCardInfos();

private:
    static std::wstring GetOsVersionImpl();
    static bool GetCpuInfoImpl(stCpuInfo& cpuInfo);
    static bool GetVideoCardInfosImpl(std::vector<CVideoCardInfo>& vctVideoCards);

private:
    std::wstring m_BoardSn;
    std::wstring m_OsVersion;
    std::vector<CVideoCardInfo> m_vctVideoCards;
    stCpuInfo m_CpuInfo;
};

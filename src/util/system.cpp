#include "util/system.h"
#include <thread>
#include <chrono>
#include <random>
#include <filesystem>
#ifdef _WIN32
#include <Windows.h>
#include <ShlObj.h>
#include <shellapi.h>
#include <iphlpapi.h>
#include <iomanip>
#else
#include <cstdlib>
#endif
#include "util/strutils.h"
void ylib::system::sleep_msec(uint32 msec)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(msec));
}

int64 ylib::system::random(int64 min, int64 max)
{
    static std::random_device rd;  // 用于获取非确定性随机数
    static std::mt19937_64 gen(rd()); // 64位的随机数生成器
    std::uniform_int_distribution<long long> distrib(min, max);
    return distrib(gen);
}

std::string ylib::system::current_dir()
{
    return std::filesystem::current_path().string();
}
std::string ylib::system::current_filepath()
{
#ifdef _WIN32
    char exeFullPath[MAX_PATH]; // Full path 
    memset(exeFullPath, 0, MAX_PATH);
    GetModuleFileName(NULL, exeFullPath, MAX_PATH);
    return exeFullPath;
#else
    return "";
#endif
}
std::string ylib::system::temp_path()
{
#ifdef _WIN32
    char strTmpDir[MAX_PATH];
    memset(strTmpDir, 0, MAX_PATH);
    GetTempPathA(MAX_PATH, strTmpDir);
    return strutils::trim_end(strTmpDir, { '\\' });
#else
    const char* tempDir = std::getenv("TMPDIR"); // 尝试获取TMPDIR环境变量
    if (tempDir == nullptr) {
        tempDir = "/tmp"; // 如果TMPDIR环境变量不存在，使用默认的临时目录路径
    }
    return tempDir;
#endif
}
std::string ylib::system::desktop_path()
{
#ifdef _WIN32
    TCHAR sDesktopPath[MAX_PATH];
    memset(sDesktopPath, 0, MAX_PATH);
    SHGetSpecialFolderPathA(NULL, sDesktopPath, CSIDL_DESKTOPDIRECTORY, FALSE);
    return sDesktopPath;
#else
    return "";
#endif
}
std::string ylib::system::currentuser_path()
{
#ifdef _WIN32
    // 分配一个足够大的缓冲区来存储路径
    char path[MAX_PATH];
    memset(path,0,MAX_PATH);
    // 获取当前用户的目录
    HRESULT result = SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, path);
    if (SUCCEEDED(result))
        return path;
    return "";
#else
    return "~";
#endif
}
void ylib::system::notification(const std::string& title, const std::string& message)
{
#ifdef _WIN32
    // 初始化 NOTIFYICONDATA 结构体
    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = NULL; // 使用一个有效的窗口句柄
    nid.uFlags = NIF_INFO;
    nid.dwInfoFlags = NIIF_INFO; // 使用适当的图标，NIIF_NONE 为无图标
    lstrcpy(nid.szInfo, message.c_str());
    lstrcpy(nid.szInfoTitle, title.c_str());
    // 发送通知
    Shell_NotifyIcon(NIM_ADD, &nid);
    Sleep(3000);

    // 移除通知
    Shell_NotifyIcon(NIM_DELETE, &nid);

#else
#endif
}
std::vector<std::string> ylib::system::mac()
{

    std::vector<std::string> macAddresses;
#ifdef _WIN32
    DWORD dwSize = 0;
    GetAdaptersInfo(nullptr, &dwSize);
    auto* pAdapterInfo = reinterpret_cast<IP_ADAPTER_INFO*>(malloc(dwSize));

    if (GetAdaptersInfo(pAdapterInfo, &dwSize) == ERROR_SUCCESS) {
        for (IP_ADAPTER_INFO* pAdapter = pAdapterInfo; pAdapter; pAdapter = pAdapter->Next) {
            std::ostringstream macStream;
            for (UINT i = 0; i < pAdapter->AddressLength; i++) {
                macStream << std::setfill('0') << std::setw(2) << std::hex
                    << static_cast<int>(pAdapter->Address[i]);
                if (i < pAdapter->AddressLength - 1)
                    macStream << "-";
            }
            macAddresses.push_back(macStream.str());
        }
    }
    free(pAdapterInfo);
#else

#endif
    return macAddresses;
}
#ifdef _WIN32
void ylib::system::open_browser(const std::string& url)
{
    ShellExecuteA(NULL, "open", url.c_str(), "", NULL, SW_SHOW);
}
std::vector<char> ylib::system::disk_list()
{
    std::vector<char> drives;
    DWORD driveMask = GetLogicalDrives();

    for (char driveLetter = 'A'; driveLetter <= 'Z'; driveLetter++) {
        if (driveMask & (1 << (driveLetter - 'A'))) {
            std::string drivePath = std::string(1, driveLetter) + ":\\";
            if (GetDriveTypeA(drivePath.c_str()) == DRIVE_FIXED) {
                drives.push_back(driveLetter);
            }
        }
    }

    return drives;
}
ylib::DiskCapacity ylib::system::disk_capacity(char disk_name)
{
    char dname[3] = { disk_name ,':',0 };

    ylib::DiskCapacity result;
    uint64 i64FreeBytesToCaller = 0;
    uint64 i64TotalBytes = 0;
    uint64 i64FreeBytes = 0;

    GetDiskFreeSpaceExA(
        dname,
        (PULARGE_INTEGER)&result.freeBytesUser,
        (PULARGE_INTEGER)&result.freeBytes,
        (PULARGE_INTEGER)&result.totalBytes);
    return result;
}
char ylib::system::disk_max_capacity()
{
    char max_disk_name = 'C';
    uint64 max_disk_size = 0;
    auto disks = ylib::system::disk_list();
    for (uint32 i = 0; i < disks.size(); i++)
    {
        auto capacity = disk_capacity(disks[i]);
        if (i == 0)
        {
            max_disk_name = disks[i];
            max_disk_size = capacity.freeBytesUser;
            continue;
        }
        if (capacity.freeBytesUser > max_disk_size)
        {
            max_disk_name = disks[i];
            max_disk_size = capacity.freeBytesUser;
        }
    }
    return max_disk_name;
}
typedef LONG NTSTATUS, * PNTSTATUS;
#define STATUS_SUCCESS (0x00000000)
typedef NTSTATUS(WINAPI* pRtlGetVersion)(PRTL_OSVERSIONINFOW lpVersionInformation);
std::string ylib::system::version()
{
#ifdef _WIN32
    HMODULE hMod = ::LoadLibraryW(L"ntdll.dll");
    if (hMod) {
        pRtlGetVersion rtlGetVersion = (pRtlGetVersion)::GetProcAddress(hMod, "RtlGetVersion");
        if (rtlGetVersion != nullptr) {
            RTL_OSVERSIONINFOW rovi = { 0 };
            rovi.dwOSVersionInfoSize = sizeof(rovi);
            if (STATUS_SUCCESS == rtlGetVersion(&rovi)) {
                ::FreeLibrary(hMod);
                return "Windows " + std::to_string(rovi.dwMajorVersion) + "." + std::to_string(rovi.dwMinorVersion);
            }
        }
        ::FreeLibrary(hMod);
    }
#else
#endif
    return "Unknown OS";
}
std::string ylib::system::last_error()
{
#ifdef _WIN32
    DWORD dwError = GetLastError();
    if (dwError == 0) {
        return std::string(); // 无错误
    }

    LPVOID lpMsgBuf;
    DWORD bufLen = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dwError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    if (bufLen) {
        std::string errMsg((const char*)lpMsgBuf, bufLen);
        LocalFree(lpMsgBuf);
        return errMsg;
    }
    else {
        return std::string("未知错误");
    }
#else
    return "(该系统不可用,获取系统错误失败)";
#endif
}
#endif
#include "util/process.h"
#include "util/strutils.h"
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#else
#include <unistd.h>
#include <limits.h>
#endif
#include <sstream>
#include <sys/types.h>
#include <signal.h>
#ifdef _WIN32
bool ylib::process::create(const std::string& filepath, const std::string& working_directory, const std::vector<std::string>& args, bool wait_close, bool show_window, size_t* pid)
{
#ifdef _WIN32
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    if (!show_window) {
        si.dwFlags |= STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;
    }
    ZeroMemory(&pi, sizeof(pi));

    // 构建命令行字符串
    std::stringstream ss;
    ss << "\"" << filepath << "\"";
    for (const auto& arg : args) {
        ss << " \"" << arg << "\"";
    }
    std::string cmd = ss.str();
    std::vector<char> cmdCStr(cmd.begin(), cmd.end());
    cmdCStr.push_back('\0');

    // 创建进程
    if (!CreateProcessA(
        NULL,                // 应用程序名
        cmdCStr.data(),      // 命令行参数
        NULL,                // 进程安全属性
        NULL,                // 线程安全属性
        FALSE,               // 句柄继承选项
        0,                   // 创建标志
        NULL,                // 环境变量
        working_directory.empty() ? NULL : working_directory.c_str(), // 当前目录
        &si,                 // STARTUPINFO
        &pi                  // PROCESS_INFORMATION
    )) {
        return false;
    }

    // 如果需要，等待进程关闭
    if (wait_close) {
        WaitForSingleObject(pi.hProcess, INFINITE);
    }
    if(pid!=nullptr)
        *pid = pi.dwProcessId;
    // 关闭进程和线程句柄
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
#else
    return false;
#endif
}
bool ylib::process::destory(uint32 process_id)
{
#ifdef _WIN32
    HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, FALSE, process_id);
    if (process_handle == NULL) {
        return false;  // 打开进程失败
    }
    BOOL result = TerminateProcess(process_handle, 0);
    CloseHandle(process_handle);
    return result != 0;
#else
    return kill(process_id, SIGKILL) == 0;
#endif
}
// 系统进程列表
std::list<ylib::process::proc_info> ylib::process::list()
{
    std::list<ylib::process::proc_info> processList;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return processList;
    }
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hSnapshot, &pe32)) {
        do {
            ylib::process::proc_info info;
            info.parent_pid = pe32.th32ParentProcessID;
            info.pid = pe32.th32ProcessID;
            info.name = pe32.szExeFile;
            processList.push_back(info);
        } while (Process32Next(hSnapshot, &pe32));
    }
    CloseHandle(hSnapshot);
    return processList;
}
// 取进程路径
std::string ylib::process::getpath(uint32 process_id)
{
#ifdef _WIN32
    std::string path;
    HANDLE processHandle = nullptr;
    TCHAR buffer[MAX_PATH] = { 0 };

    processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id);
    if (processHandle != nullptr) {
        if (GetModuleFileNameEx(processHandle, NULL, buffer, MAX_PATH)) {
            path = buffer;
        }
        CloseHandle(processHandle);
    }
    return path;
#else
    std::string path;
    char buffer[PATH_MAX] = { 0 };
    std::string link = "/proc/" + std::to_string(process_id) + "/exe";
    ssize_t len = readlink(link.c_str(), buffer, sizeof(buffer) - 1);
    if (len != -1) {
        path = std::string(buffer, len);
    }
    return path;
#endif
}
bool ylib::process::exist(const std::string& filepath)
{
    auto list = process::list();
    for_iter(iter, list)
    {
        if (strutils::change_case(iter->path(), false) == strutils::change_case(filepath, false))
            return true;
    }
    return false;
}
// 检测多开
bool ylib::process::already_running(const std::string& name)
{
    //初始化临界区全局原子变量
    HANDLE MutexHandle = CreateMutexA(NULL, FALSE, name.c_str());  //创建互斥体. 信号量为0. 有信号的状态.wait可以等待
    DWORD ErrorCode = 0;
    ErrorCode = GetLastError();
    CloseHandle(MutexHandle);
    return ERROR_ALREADY_EXISTS == ErrorCode;
}
// 设置为启动
bool ylib::process::running(const std::string& name)
{
    //初始化临界区全局原子变量
    HANDLE MutexHandle = CreateMutexA(NULL, FALSE, name.c_str());  //创建互斥体. 信号量为0. 有信号的状态.wait可以等待
    DWORD ErrorCode = 0;
    ErrorCode = GetLastError();
    if (ERROR_ALREADY_EXISTS == ErrorCode)
    {
        CloseHandle(MutexHandle);
        return false;
    }
    return true;
}
#endif
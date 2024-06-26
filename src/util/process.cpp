﻿/*Software License

Copyright(C) 2024[liuyingjie]
License Terms
Usage Rights

Any individual or entity is free to use, copy, and distribute the binary form of this software without modification to the source code, without the need to disclose the source code.
If the source code is modified, the modifications must be open - sourced under the same license.This means that the modifications must be disclosed and accompanied by a copy of this license.
Future Versions Updates
From this version onwards, all future releases will be governed by the terms of the latest version of the license.This license will automatically be nullified and replaced by the new version.
Users must comply with the terms of the new license issued in future releases.
Liability and Disclaimer
This software is provided “as is”, without any express or implied warranties, including but not limited to the warranties of merchantability, fitness for a particular purpose, and non - infringement.In no event shall the author or copyright holder be liable for any claims, damages, or other liabilities, whether in an action of contract, tort, or otherwise, arising from, out of, or in connection with the software or the use or other dealings in the software.
Contact Information
If you have any questions, please contact us: 1585346868@qq.com Or visit our website fwlua.com.
*/

#include "util/process.h"
#include "util/strutils.h"
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#else
#include <iterator>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <sstream>
#endif
#include <sstream>
#include <sys/types.h>
#include <signal.h>
bool ylib::process::create(const std::string& filepath, const std::string& working_directory, const std::vector<std::string>& args, bool wait_close, bool show_window, int* return_code, size_t* pid)
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
    ss << filepath<<" ";
    for (const auto& arg : args) {
        ss << " "<<arg<<" ";
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

        // 获取子进程的返回值
        if (return_code != nullptr)
        {
            DWORD exitCode;
            if (GetExitCodeProcess(pi.hProcess, &exitCode)) {
                *return_code = exitCode;
            }
        }
        


    }
    if(pid!=nullptr)
        *pid = pi.dwProcessId;
    // 关闭进程和线程句柄
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
#else
    // 创建一个子进程
    pid_t pid_fork = fork();
    if (pid_fork == -1) {
        // fork失败
        return false;
    }
    else if (pid_fork == 0) {
        // 在子进程中

        // 如果不显示窗口，设置stdin、stdout和stderr为/dev/null
        if (!show_window) {
            int dev_null = open("/dev/null", O_RDWR);
            dup2(dev_null, STDIN_FILENO);
            dup2(dev_null, STDOUT_FILENO);
            dup2(dev_null, STDERR_FILENO);
            close(dev_null);
        }

        // 设置工作目录
        if (!working_directory.empty() && chdir(working_directory.c_str()) != 0) {
            // 如果chdir失败，退出子进程
            exit(EXIT_FAILURE);
        }

        // 构建参数列表
        std::vector<char*> argv;
        argv.push_back(const_cast<char*>(filepath.c_str()));
        for (const auto& arg : args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr);  // 参数列表以nullptr结尾

        // 执行新的程序
        execvp(filepath.c_str(), argv.data());
        // 如果execvp返回，说明发生了错误
        exit(EXIT_FAILURE);
    }
    else {
        // 在父进程中
        if (pid != nullptr) {
            *pid = static_cast<size_t>(pid_fork);
        }

        // 如果需要等待子进程结束
        if (wait_close) {
            int status;
            waitpid(pid_fork, &status, 0);
            if (return_code != nullptr) {
                if (WIFEXITED(status)) {
                    *return_code = WEXITSTATUS(status);
                }
                else {
                    *return_code = -1; // 表示子进程未正常退出
                }
            }
}

        return true;
    }
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
#ifdef _WIN32
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
#else
    DIR* dir = opendir("/proc");
    if (dir != nullptr) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            int pid = atoi(entry->d_name);
            if (pid > 0) {  // 过滤掉非数字目录
                proc_info info;
                info.pid = pid;

                // 读取进程名和父进程ID
                std::string pid_path = "/proc/" + std::to_string(pid) + "/stat";
                std::ifstream stat_file(pid_path);
                if (stat_file.is_open()) {
                    std::string line;
                    std::getline(stat_file, line);
                    std::istringstream iss(line);
                    std::vector<std::string> tokens(std::istream_iterator<std::string>{iss},
                        std::istream_iterator<std::string>());

                    if (tokens.size() > 3) {
                        info.name = tokens[1];  // 通常是进程名
                        info.parent_pid = std::stoul(tokens[3]);  // 父进程ID
                    }
                    processList.push_back(info);
                }
            }
        }
        closedir(dir);
    }
#endif
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
std::vector<size_t> ylib::process::exist(const std::string& filepath)
{
    std::string src_path = strutils::replace(strutils::change_case(filepath, false), '\\', '/');

    std::vector<size_t> result;
    auto list = process::list();
    for_iter(iter, list)
    {
        if (strutils::replace(strutils::change_case(iter->path(), false), '\\', '/') == src_path)
        {
            result.push_back(iter->pid);
        }
    }
    return result;
}
bool ylib::process::exist(size_t pid)
{
#ifdef _WIN32
    HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (processHandle == NULL) {
        return false;
    }
    DWORD exitCode;
    if (!GetExitCodeProcess(processHandle, &exitCode)) {
        CloseHandle(processHandle);
        return false;
    }
    CloseHandle(processHandle);
    return (exitCode == STILL_ACTIVE);
#else
    if (kill(pid, 0) == 0) {
        return true;
    }
    else {
        return (errno == EPERM);
    }
#endif
}

#ifdef _WIN32



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

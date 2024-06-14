/*Software License

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

#include "util/debug.h"
#ifdef _WIN32
#include <Windows.h>
#include <string.h>
#include "DbgHelp.h"


int GenerateMiniDump(HANDLE hFile, PEXCEPTION_POINTERS pExceptionPointers, PWCHAR pwAppName)
{
    BOOL bOwnDumpFile = FALSE;
    HANDLE hDumpFile = hFile;
    MINIDUMP_EXCEPTION_INFORMATION ExpParam;

    typedef BOOL(WINAPI* MiniDumpWriteDumpT)(
        HANDLE,
        DWORD,
        HANDLE,
        MINIDUMP_TYPE,
        PMINIDUMP_EXCEPTION_INFORMATION,
        PMINIDUMP_USER_STREAM_INFORMATION,
        PMINIDUMP_CALLBACK_INFORMATION
        );

    MiniDumpWriteDumpT pfnMiniDumpWriteDump = NULL;
    HMODULE hDbgHelp = LoadLibraryA("DbgHelp.dll");
    if (hDbgHelp)
        pfnMiniDumpWriteDump = (MiniDumpWriteDumpT)GetProcAddress(hDbgHelp, "MiniDumpWriteDump");

    if (pfnMiniDumpWriteDump)
    {
        if (hDumpFile == NULL || hDumpFile == INVALID_HANDLE_VALUE)
        {
            //TCHAR szPath[MAX_PATH] = { 0 };
            TCHAR szFileName[MAX_PATH] = { 0 };
            //TCHAR* szAppName = pwAppName;
            const TCHAR* szVersion = "v1.0";
            SYSTEMTIME stLocalTime;

            GetLocalTime(&stLocalTime);
            //GetTempPath(dwBufferSize, szPath);

            //wsprintf(szFileName, L"%s%s", szPath, szAppName);
            CreateDirectory(szFileName, NULL);

            wsprintf(szFileName, "%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp",
                //szPath, szAppName, szVersion,
                szVersion,
                stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
                stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond,
                GetCurrentProcessId(), GetCurrentThreadId());
            hDumpFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

            bOwnDumpFile = TRUE;
            OutputDebugString(szFileName);
        }

        if (hDumpFile != INVALID_HANDLE_VALUE)
        {
            ExpParam.ThreadId = GetCurrentThreadId();
            ExpParam.ExceptionPointers = pExceptionPointers;
            ExpParam.ClientPointers = FALSE;

            pfnMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
                hDumpFile, MiniDumpWithDataSegs, (pExceptionPointers ? &ExpParam : NULL), NULL, NULL);

            if (bOwnDumpFile)
                CloseHandle(hDumpFile);
        }
    }

    if (hDbgHelp != NULL)
        FreeLibrary(hDbgHelp);

    return EXCEPTION_EXECUTE_HANDLER;
}

LONG WINAPI ExceptionFilter(LPEXCEPTION_POINTERS lpExceptionInfo)
{
    if (IsDebuggerPresent())
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    return GenerateMiniDump(NULL, lpExceptionInfo, (PWCHAR)L"test");
}
void ylib::debug::detect_exception()
{
    SetUnhandledExceptionFilter(ExceptionFilter);
}
void ylib::debug::vs_console_println(const std::string& value)
{
    std::string v2 = value + "\n";
    OutputDebugString(value.c_str());
}
#endif

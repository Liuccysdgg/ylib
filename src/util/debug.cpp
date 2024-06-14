// MIT License﻿
// Copyright(c) 2024 FastWeb - fwlua.com - nianhua﻿
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :﻿
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.﻿
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.﻿
// ## Additional Terms for Commercial Use﻿
// This software is licensed for personal, educational, and non - commercial use.
// For commercial use or use within a company, organization, or institution, a
// separate commercial license is required.To obtain a commercial license,
// please contact
// EMail：1585346868@qq.com

// QQ：1585346868

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

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

#include "util/desktop.h"
#ifdef _WIN32
#include <Windows.h>
#include "util/window.h"
#include "util/process.h"
ylib::desktop::desktop(const std::string& name, bool create_new, bool destory)
{

    m_desktop = 0;
    m_destory = true;

	m_name = name;
    m_destory = destory;
    m_desktop = (size_t)OpenDesktopA(name.c_str(), 0, true, GENERIC_ALL);
    if(m_desktop == 0 && create_new)
	    m_desktop = (size_t)CreateDesktopA(name.c_str(), 0, 0, 0, GENERIC_ALL, 0);
}

ylib::desktop::desktop(size_t handle,bool destory)
{
    m_destory = destory;
    m_desktop = handle;
}

ylib::desktop::~desktop()
{
    if (m_desktop != 0 && m_destory == true)
    {
        CloseDesktop((HDESK)m_desktop);
    }
}

bool ylib::desktop::show()
{
    if(m_desktop == 0)
        return false;
	current();
	return SwitchDesktop((HDESK)m_desktop);
}

bool ylib::desktop::exec(const std::string& filepath,const std::string& path)
{ 
    if(m_desktop == 0)
        return false;
    STARTUPINFOA info = { sizeof(STARTUPINFOA) };
    info.dwFlags = STARTF_USESHOWWINDOW;
    info.wShowWindow = SW_SHOW;
    info.lpDesktop = (LPSTR)m_name.c_str();
    PROCESS_INFORMATION proc;
    if(!CreateProcessA(
        0, 
        (LPSTR)filepath.c_str(),
        0,
        0,
        FALSE,
        0,
        0,
        path.length()==0?NULL:path.c_str(),
        &info,
        &proc)
    )
        return false;
    CloseHandle(proc.hProcess);
    CloseHandle(proc.hThread);
    m_exec_hds.push_back(proc.dwProcessId);
    return true;
}

void ylib::desktop::current()
{
    if(m_desktop == 0)
        return;
    SetThreadDesktop((HDESK)m_desktop);
}

void ylib::desktop::destoryAllProcesses(bool subproc)
{
    std::list<process::proc_info> proc_list = process::list();
    for (size_t i = 0; i < m_exec_hds.size(); i++)
    {
        for_iter(iter, proc_list)
        {
            if (m_exec_hds[i] == iter->pid)
            {
                if (subproc)
                {
                    desktorySubProcesses(proc_list, iter->pid);
                    process::destory(iter->pid);
                }
            }
        }
        
    }
    m_exec_hds.clear();
}

size_t ylib::desktop::handle()
{
    return m_desktop;
}
size_t ylib::desktop::winhandle()
{
    auto list = window::find(0,"#32769");
    if (list.size() > 1 || list.size() == 0)
        return 0;
    return list[0];
}


void ylib::desktop::desktorySubProcesses(const std::list<ylib::process::proc_info>& list, size_t pid)
{
    for_iter(iter,list)
    {
        if (iter->parent_pid == pid)
        {
            desktorySubProcesses(list, iter->pid);
            ylib::process::destory(pid);
        }
    }
}

BOOL CALLBACK DeskEnum(LPSTR desk, LPARAM lParam)
{
    std::vector<desktop::DesktopInfo> *infos = (std::vector<desktop::DesktopInfo>*)lParam;
    HDESK hDesk = OpenDesktopA(desk, 0, FALSE, DESKTOP_READOBJECTS | DESKTOP_ENUMERATE);
    if (hDesk)
    {
        HDESK hCurrentDesk = GetThreadDesktop(GetCurrentThreadId());
        ylib::desktop::DesktopInfo info;
        info.name = desk;
        info.handle = (size_t)hDesk;
        infos->push_back(info);
    }
    return TRUE;
}


std::vector<desktop::DesktopInfo> ylib::desktop::all(const std::string& winsta)
{
    std::vector<desktop::DesktopInfo> infos;
    // Winsta0
    //HWINSTA current = GetProcessWindowStation();
    HWINSTA result = 0;
    if(winsta == "")
        result = OpenWindowStationA("Winsta0", FALSE, WINSTA_ENUMDESKTOPS);
    else
        result = OpenWindowStationA(winsta.c_str(), FALSE, WINSTA_ENUMDESKTOPS);
    if (result != 0)
    {
        //SetProcessWindowStation(hWinsta);
        EnumDesktopsA(result, &DeskEnum, (LPARAM)&infos);
        //SetProcessWindowStation(current);
        //CloseWindowStation(hWinsta);
    }
    return infos;
}

size_t ylib::desktop::get_current()
{
    return (size_t)GetThreadDesktop(GetCurrentThreadId());
}

void ylib::desktop::show(size_t handle)
{
    SetThreadDesktop((HDESK)handle);
    SwitchDesktop((HDESK)handle);
}

size_t ylib::desktop::get_desktop(const std::string& name)
{
    return (size_t)OpenDesktopA(name.c_str(), 0, true, GENERIC_ALL);
}

bool ylib::desktop::close(size_t desktop)
{
    return CloseDesktop((HDESK)desktop);
}
#endif

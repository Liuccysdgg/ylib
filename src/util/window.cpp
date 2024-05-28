#include "util/window.h"
#include "util/process.h"
#include "base/buffer.h"
#ifdef _WIN32
#include <Windows.h>
#include <vector>
#include <algorithm>
size_t ylib::window::find(const std::string &class_name, const std::string &title_name)
{
    return (size_t)FindWindowA(class_name.c_str(),title_name.c_str());
}


ylib::Rect ylib::window::rect(size_t handle)
{
    ylib::Rect result;
    RECT info;
    if (GetWindowRect((HWND)handle, &info) == false)
        return result;
    result.right = info.right;
    result.bottom = info.bottom;
    result.left = info.left;
    result.top = info.top;
    return result;
}
void ylib::window::set_parent(size_t handle, size_t parent)
{
    SetParent((HWND)handle,(HWND)parent);
}
std::string ylib::window::path(size_t handle)
{
    HWND hWnd = (HWND)handle;
    DWORD idProcess = NULL;
    GetWindowThreadProcessId(hWnd, &idProcess);
    return ylib::process::getpath(idProcess);
}
void ylib::window::close(size_t handle)
{
    SendMessageA((HWND)handle, WM_CLOSE, 0, 0);
}
std::string ylib::window::content(size_t handle)
{
    uint32 count = SendMessageA((HWND)handle, 14, 0, 0) + 1;
    ylib::buffer data;
    if (count > 50000)
    {
        uint32 tempSize = 50000;
        data.resize(50000);
        SendMessageA((HWND)handle, 13, tempSize + 1, (LPARAM)data.data());
        if (data[49999] < 0)
            data = data.left(data.length() - 1);
    }
    else
    {
        data.resize(count);
        SendMessageA((HWND)handle, 13, count, (LPARAM)data.data());
    }
    return data.to_string();
}
void ylib::window::content(size_t handle, const std::string& value)
{
    SendMessageA((HWND)handle, 12, 0, (LPARAM)value.c_str());
}
void EnumerateChildWindows(HWND parentWindow, std::vector<size_t>& windowHandles) {
    EnumChildWindows(parentWindow, [](HWND hWnd, LPARAM lParam) -> BOOL {
        std::vector<size_t>& handles = *reinterpret_cast<std::vector<size_t>*>(lParam);
        handles.push_back((size_t)hWnd);
        EnumerateChildWindows(hWnd, handles); // 递归调用，以遍历子窗口的子窗口
        return TRUE;
        }, reinterpret_cast<LPARAM>(&windowHandles));
}
std::vector<size_t> window::find(size_t parent, const std::string &class_name, const std::string &title_name, const std::string& filepath)
{
#if 0
    std::vector<size_t> list;
    HWND win_h = 0;
    if (parent == 0)
        parent = (size_t)GetDesktopWindow();
    win_h = GetWindow((HWND)parent,5);
    bool ok;
    while (win_h != 0)
    {
        ok = true;
        if (class_name.length() == 0 && title_name.length() == 0)
            list.push_back((size_t)win_h);
        else
        {
            if (class_name.length() != 0)
            {
                auto curr_cn = ::window::class_name((size_t)win_h);
                if (curr_cn.find(class_name) != -1)
                {
                    if (title_name.length() != 0)
                    {
                        auto curr_tn = ::window::title_name((size_t)win_h);
                        if (curr_tn.find(title_name) == -1)
                            goto _CONTINUE;
                        else
                            list.push_back((size_t)win_h);
                    }
                    else
                        list.push_back((size_t)win_h);
                }
            }
            else
            {
                if (title_name.length() != 0)
                {
                    auto curr_tn = ylib::window::title_name((size_t)win_h);
                    if (curr_tn.find(title_name) == -1)
                        goto _CONTINUE;
                    else
                        list.push_back((size_t)win_h);
                }
            }
        }
    _CONTINUE:
        win_h = GetWindow(win_h, 2);
    }
    return list;
#else
    std::vector<size_t> result;
    std::vector<size_t> list;
    EnumerateChildWindows((HWND)parent, list);
    for (size_t i = 0; i < list.size(); i++)
    {
        
        if (title_name.length() != 0)
        {
            auto curr_tn = ylib::window::title_name(list[i]);
            if (curr_tn.find(title_name) == -1)
                continue;
        }
        if (class_name.length() != 0)
        {
            auto curr_tn = ylib::window::class_name(list[i]);
            if (curr_tn.find(class_name) == -1)
                continue;
        }
        result.push_back(list[i]);
    }
    // 先对向量排序
    std::sort(result.begin(), result.end());
    // unique 重排向量，移动重复元素到向量末尾，并返回新的逻辑末尾位置
    auto last = std::unique(result.begin(), result.end());
    // 删除重复元素
    result.erase(last, result.end());

    std::vector<size_t> r2;
    for (size_t i = 0; i < result.size(); i++)
    {
        if (filepath.empty() == false)
        {
            if (ylib::window::path(result[i]) == filepath)
                r2.push_back(result[i]);
        }
        else
            r2.push_back(result[i]);
    }
    return r2;
#endif
}

size_t window::parent(size_t handle)
{
    return (size_t)GetParent((HWND)handle);
}

std::string window::class_name(size_t handle)
{
    char name[255];
    memset(name, 0, 255);
    GetClassNameA((HWND)handle,name,255);
    return name;
}

std::string window::title_name(size_t handle)
{
    char name[2048];
    memset(name, 0, sizeof(name));
    GetWindowTextA((HWND)handle, name, GetWindowTextLengthA((HWND)handle)+1);
    return name;
}

bool window::title_name(size_t handle, const std::string &name)
{
    return SetWindowTextA((HWND)handle,name.c_str());
}


bool window::button::click(size_t handle)
{
    if (PostMessageA((HWND)handle, 513, 1, 2 + 2 * 65536) == false)
        return false;
    return PostMessageA((HWND)handle, 514, 0, 2 + 2 * 65536);
}
bool window::button::click(size_t parent, const std::string& class_name, const std::string& title_name)
{
    auto list = ylib::window::find(parent, class_name, title_name);
    if (list.size() != 1)
        return false;
    button::click(list[0]);
    return true;
}

bool window::menu::click(size_t handle, size_t menu_child_handle, uint32 menu_item_idx)
{
    auto item = GetMenuItemID((HMENU)menu_child_handle, menu_item_idx);
    if (item == 0)
        return false;
    return PostMessageA((HWND)handle, 273, item, 0);
}

size_t window::menu::get(size_t handle)
{
    if (handle == 0)
        return 0;
    return (size_t)GetMenu((HWND)handle);
}

size_t window::menu::child(size_t menu_handle, uint32 menu_childs_idx)
{
    if (menu_handle == 0)
        return 0;
    return (size_t)GetSubMenu((HMENU)menu_handle, menu_childs_idx);
}
bool window::checkbox::checked(size_t handle)
{
    return !(SendMessageA((HWND)handle, 240, 0, 0) == 0);
}
bool window::checkbox::checked(size_t handle, bool checked)
{
    return SendMessageA((HWND)handle, 241, checked == true ? 1 : 0, 0) == 0;
}

void window::tab::current(size_t handle, uint32 idx)
{
    SendMessageA((HWND)handle, 4912, idx, 0);
}
bool window::enable(size_t win_handle, bool enable)
{
    return EnableWindow((HWND)win_handle, enable);
}

void window::list::insert(size_t handle, uint32 index, const std::string& value)
{
    SendMessageA((HWND)handle, 385, index, (LPARAM)value.c_str());
}
void window::rect_sort(std::vector<size_t>& list, uint32 type, bool ps)
{
#define SORT_RECT(VALUE)                                                                                                \
    uint32 temp = 0;                                                                                                            \
    for (uint32 i = 0; i < list.size(); i++)                                                                                   \
    {                                                                                                                                       \
        for (uint32 x = i; x < list.size(); x++)                                                                                \
        {                                                                                                                                       \
			auto rect_i = window::rect(list[i]);                                                                    \
            auto rect_x = window::rect(list[x]);                                                                \
            if (ps == true ? rect_i.VALUE > rect_x.VALUE:rect_i.VALUE < rect_x.VALUE)                   \
            {                                                                                                                                   \
                temp = list[i];                                                                                                         \
                list[i] = list[x];                                                                                                                  \
                list[x] = temp;                                                                                                     \
            }                                                                                                                           \
        }                                                                                                                                   \
    }                                                                                                                                           
    if (type == 0)
    {
        SORT_RECT(top);
    }
    else if (type == 1)
    {
        SORT_RECT(right);
    }
    else if (type == 2)
    {
        SORT_RECT(left);
    }
    else if (type == 3)
    {
        SORT_RECT(bottom);
    }
}

#endif
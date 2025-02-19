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

#include "net/http_cookie.h"
#if USE_NET_HTTP_UTIL
#include "util/strutils.h"
ylib::network::http::cookie::cookie()
{

}

ylib::network::http::cookie::~cookie()
{

}

void ylib::network::http::cookie::merge(const cookie& ck)
{
    for_iter(iter, ck.m_param)
        m_param[iter->first] = iter->second;
}
void ylib::network::http::cookie::merge(const std::string& set_cookie)
{
    if (set_cookie == "")
        return;
    std::map<std::string, std::string> set_cookie_map;
    std::map<std::string, std::string> local_cookie_map;
    std::vector<std::string> cookies = strutils::split(set_cookie,";");
    if (cookies.size() == 0)
    {
        auto arr = strutils::split(set_cookie,"=");
        if(arr.size() == 2)
            m_param[arr[0]] = arr[1];
    }
    else
    {
        for (uint32 i = 0; i < cookies.size(); i++)
        {
            auto arr = strutils::split(cookies[i],"=");
            if (arr.size() == 2)
                m_param[strutils::trim(arr[0],{' '})] = strutils::trim(arr[1],{' '});
        }
    }
    

}

std::string ylib::network::http::cookie::to_string()
{
    std::string cookie_string;
    uint32 idx = 0;
    for_iter(iter, m_param)
    {
        if (iter->first == "")
            continue;
        if (idx == m_param.size() - 1)
        {
            cookie_string += iter->first + "=" + iter->second;
        }
        else
        {
            cookie_string += iter->first + "=" + iter->second + "; ";
        }
    }
    return strutils::trim_end(cookie_string,{' ',';'});
}
void ylib::network::http::cookie::clear()
{
    m_param.clear();
}
std::map<std::string, std::string>& ylib::network::http::cookie::toSTL()
{
    return m_param;
}
#endif

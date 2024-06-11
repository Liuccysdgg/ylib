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
// Mobile：17367918735
// QQ：1585346868

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
#endif

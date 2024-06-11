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

#include "net/http_interceptor.h"
#include "net/http_reqpack.h"
#include "net/http_request.h"
#include <iostream>
#include "net/http_center.h"
#include "util/time.h"
#if USE_NET_HTTP_WEBSITE
network::http::interceptor::interceptor()
{

}
network::http::interceptor::~interceptor()
{
    clear();
}
bool network::http::interceptor::add(const std::string& pattern, const std::string& extra, std::function<bool(network::http::reqpack* rp, const std::string&,const std::string&)> callback)
{
    std::unique_lock<std::shared_mutex> lock(m_rw_mutex);
    // 判断是否存在
    for (size_t i = 0; i < m_list.size(); i++)
    {
        if (m_list.at(i).pattern == pattern)
            return false;
    }
    network::http::interceptor_info info;
    info.regex = std::regex(pattern.c_str());
    info.callback = callback;
    info.pattern = pattern;
    info.extra = extra;
    m_list.push_back(info);
    return true;
}
bool ylib::network::http::interceptor::remove(const std::string& pattern)
{
    std::unique_lock<std::shared_mutex> lock(m_rw_mutex);
    for (auto iter = m_list.begin(); iter != m_list.end();)
    {
        if (iter->pattern == pattern)
        {
            m_list.erase(iter);
            return true;
        }
        else
            iter++;
    }
    return false;
}
bool ylib::network::http::interceptor::exist(const std::string& pattern)
{
    std::shared_lock<std::shared_mutex> lock(m_rw_mutex);
    for (size_t i = 0; i < m_list.size(); i++)
    {
        if (m_list.at(i).pattern == pattern)
            return true;
    }
    return false;
}
bool network::http::interceptor::trigger(const std::string& url, network::http::reqpack* rp)
{
    std::shared_lock<std::shared_mutex> lock(m_rw_mutex);

    if (m_list.size() == 0)
        return true;
    for (size_t i = 0; i < m_list.size(); i++)
    {
        if (std::regex_match(url.c_str(), m_list[i].regex)) {
            bool result = m_list[i].callback(rp, m_list[i].pattern, m_list[i].extra);
            if (result == false) {
#if HTTP_INTERCEPTOR_PRINT == 1
                ylib::log->warn("[" + rp->exec_msec() + " ms] false url:" + url + "\t" + " express:" + info->express_string + " ip:" + rp->request()->remote_ipaddress(true), "interceptor");
#endif
            }
            return result;
        }
    }
    return true;
}
void ylib::network::http::interceptor::clear()
{
    std::unique_lock<std::shared_mutex> lock(m_rw_mutex);
    m_list.clear();
}
#endif

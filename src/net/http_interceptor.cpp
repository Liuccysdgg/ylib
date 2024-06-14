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

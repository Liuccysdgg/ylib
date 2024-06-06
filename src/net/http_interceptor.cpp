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
bool network::http::interceptor::add(const std::string& express, std::function<bool(network::http::reqpack* rp,const std::string&)> callback)
{
    std::unique_lock<std::shared_mutex> lock(m_rw_mutex);
    // 判断是否存在
    for (size_t i = 0; i < m_list.size(); i++)
    {
        if (m_list.at(i).express_string == express)
            return false;
    }
    network::http::interceptor_info info;
    info.express = std::regex(express.c_str());
    info.callback = callback;
    info.express_string = express;
    m_list.push_back(info);
    return true;
}
bool ylib::network::http::interceptor::remove(const std::string& regex_express)
{
    std::unique_lock<std::shared_mutex> lock(m_rw_mutex);
    for(auto iter = m_list.begin();iter != m_list.end();)
    {
        if (iter->express_string == regex_express)
        {
            m_list.erase(iter);
            return true;
        }
        else
            iter++;
    }
    return false;
}
bool ylib::network::http::interceptor::exist(const std::string& regex_express)
{
    std::shared_lock<std::shared_mutex> lock(m_rw_mutex);
    for (size_t i = 0; i < m_list.size(); i++)
    {
        if (m_list.at(i).express_string == regex_express)
            return true;
    }
    return false;
}
bool network::http::interceptor::trigger(const std::string& url, network::http::reqpack* rp)
{
    std::shared_lock<std::shared_mutex> lock(m_rw_mutex);

    if(m_list.size() == 0)
        return true;
	for(size_t i=0;i<m_list.size();i++)
	{
        if(std::regex_match(url.c_str(),m_list[i].express)) {
            bool result = m_list[i].callback(rp, m_list[i].express_string);
            if(result == false){
#if HTTP_INTERCEPTOR_PRINT == 1
                ylib::log->warn("["+rp->exec_msec()+" ms] false url:"+url+"\t"+" express:"+info->express_string+" ip:"+rp->request()->remote_ipaddress(true),"interceptor");
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

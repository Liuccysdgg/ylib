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
size_t network::http::interceptor::add(const std::string& express, std::function<bool(network::http::reqpack* rp,const std::string&)> callback)
{
    network::http::interceptor_info *info = new network::http::interceptor_info;
    info->express = std::regex(express.c_str());
    info->callback = callback;
    info->express_string = express;
    return m_array.append(info);
}
bool network::http::interceptor::trigger(const std::string& url, network::http::reqpack* rp)
{
    if(m_array.m_count == 0)
        return true;
	for(size_t i=0;i<m_array.m_count;i++)
	{
        auto info = m_array.get(i);
        if(std::regex_match(url.c_str(),info->express)){
            bool result = info->callback(rp,info->express_string);
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
    for (size_t i = 0; i < m_array.size(); i++)
        delete m_array.get(i);
    m_array.free();
}
#endif

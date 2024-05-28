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

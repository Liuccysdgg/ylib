﻿/*Software License

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

#include "net/http_website.h"
#if USE_NET_HTTP_UTIL
#include "net/http_host.h"
#include "net/http_ssl.h"
#include "net/http_center.h"
#include "net/http_router.h"
#include "net/http_session.h"
#include "net/util.h"
#include "net/http_agent.h"
#include "net/http_cache.h"
#include "net/http_cdn.h"
ylib::network::http::website::website()
{
    m_https = false;
    m_session_local_storage = new ylib::local_storage();
    m_router = new network::http::router;
    m_cache = new network::http::cache;
    m_cdn = new network::http::cdn();
   // m_agent = new network::http::agent;
    
}

ylib::network::http::website::~website()
{
    delete m_router;
    delete m_cache;
    delete m_cdn;
    delete m_session_local_storage;
}

bool ylib::network::http::website::start(const website_config& config)
{
    m_config = config;

    //m_session_local_storage->center(center());
    m_router->center(center());
    m_cache->center(center());
    m_cdn->center(center());
    m_cdn->website(this);

    auto find_ssl = [&](const std::string& host)->ssl_config {
        auto iter = this->center()->config().cert.find(host);
        if (this->center()->config().cert.end() == iter)
            return ssl_config();
        return iter->second;
    };
    
    if (m_config.host.size() == 0)
    {
        m_lastErrorDesc = "Host name needs to be set";
        return false;
    }
    
    for (size_t i = 0; i < m_config.host.size(); i++)
    {
        auto hostpoint = new network::http::host;


        network::http::ssl* ssl = nullptr;
        if (m_config.host[i].ssl)
        {
            m_https = true;
            auto ssl_config = find_ssl(m_config.host[i].domain);

            if (ssl_config.enable == false)
                ylib::log->warn("The certificate is not open or does not exist, Host:"+ m_config.host[i].domain);
            else
            {
                ssl = new network::http::ssl(center()->server(m_config.host[i].port),ssl_config);
                /*注册SSL*/
                if (ssl->regist() == false)
                {
                    ylib::log->warn(ssl->last_error());
                    delete ssl;
                    ssl = nullptr;
                }
                else
                {
                    /*绑定SSL*/
                    if (ssl->bind(m_config.host[i].domain) == false)
                    {
                        ylib::log->warn(ssl->last_error());
                        delete ssl;
                        ssl = nullptr;
                    }
                }
            }
        }

        hostpoint->init(m_config.host[i].domain, m_config.host[i].port,ssl);
        m_hosts.push_back(hostpoint);
    }
        if(m_cache->start(m_config.cache) == false)
    {
         m_lastErrorDesc = m_cache->last_error();
         return false;

    }
    if (m_session_local_storage->open(m_config.session.dirpath) == false)
    {
        m_lastErrorDesc = "session open failed("+m_session_local_storage->last_error() + "), dirpath: " + m_config.session.dirpath;
        return false;
    }
    if (m_router->start(m_config.router) == false)
    {
        m_session_local_storage->close();
        m_lastErrorDesc = m_router->last_error();
        return false;
    }
    if(m_cdn->start(m_config.cdn) == false)
    {
         m_lastErrorDesc = m_cdn->last_error();
         return false;
    }

    /*代理*/
    {
//        if (m_agent->start())
 //       {
            for (size_t i = 0; i < m_config.proxy.size(); i++)
            {
                auto proxy_cj = m_config.proxy[i];
                network::http::proxy* proxy = new network::http::proxy;
                proxy->src_str = proxy_cj.src;
                proxy->src_express = std::regex(proxy->src_str.c_str());
                proxy->dst = proxy_cj.dst;

                if(proxy->dst == "/"){
                    proxy->dst.clear();
                }
                // 解析地址
                {
                    proxy->remote_url = proxy_cj.remote;
                    std::string http_type;
                    std::string http_host;
                    std::string http_ipaddress;
                    ushort http_port;
                    std::string http_urlfield;
                    ylib::network::parse_url(proxy->remote_url, http_type, http_host, http_ipaddress, http_port, http_urlfield);
                    proxy->remote_ipaddress = http_host;
                    proxy->remote_port = http_port;
                    proxy->ssl = http_type=="https://";
                }
                proxy->host = proxy_cj.host;
                // 协议头
                proxy->request_headers = proxy_cj.header_request;
                proxy->response_headers = proxy_cj.header_response;
                m_proxy.append(proxy);
            }
   //     }
     //   else
       // {
        //    std::cout << "proxy start failed" << std::endl;
       // }
        
    }
    return true;
}

void ylib::network::http::website::close()
{
    m_router->close();
    m_cache->stop();
    m_cdn->close();
    m_session_local_storage->close();
}
network::http::router* network::http::website::router()
{
    return m_router;
}
bool ylib::network::http::website::host(const std::string& host)
{
    for (size_t i = 0; i < m_hosts.size(); i++)
    {
        //std::string hostname = m_hosts[i]->hostname();
        if (m_hosts[i]->hostname() == host || m_hosts[i]->hostname().find("0.0.0.0") != -1)
            return true;
    }
    return false;
}

ylib::nolock_array<network::http::proxy*>* ylib::network::http::website::proxy()
{
    return &m_proxy;
}
#endif

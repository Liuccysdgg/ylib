#include "ynet/http_website.h"
#if USE_NET_HTTP_UTIL
#include "ynet/http_host.h"
#include "ynet/http_ssl.h"
#include "ynet/http_center.h"
#include "ynet/http_router.h"
#include "ynet/http_session.h"
#include "ynet/util.h"
#include "ynet/http_agent.h"
#include "ynet/http_cache.h"
#include "ynet/http_cdn.h"
ylib::network::http::website::website()
{
    m_https = false;
    m_session = new network::http::session_mgr;
    m_router = new network::http::router;
    m_cache = new network::http::cache;
    m_cdn = new network::http::cdn();
   // m_agent = new network::http::agent;
    
}

ylib::network::http::website::~website()
{
    //delete m_agent;
    delete m_session;
    delete m_router;
    delete m_cache;
    delete m_cdn;
}

bool ylib::network::http::website::start(const ylib::json& config)
{


    m_session->center(center());
    m_router->center(center());
    m_cache->center(center());
    m_cdn->center(center());
    m_cdn->website(this);
    m_name = config["name"].to<std::string>();

    auto find_ssl = [&](const std::string& host)->ylib::json {
        ylib::json ssl_config = this->center()->config()["ssl"];
        return ssl_config[host];
    };
    
    if (config["host"].size() == 0)
    {
        m_lastErrorDesc = "Host name needs to be set";
        return false;
    }
    
    for (size_t i = 0; i < config["host"].size(); i++)
    {
        auto hostpoint = new network::http::host;
        ylib::json host_config = config["host"][i];
        std::string host = host_config["host"].to<std::string>();
        ushort port = 0;
        {
            std::string http_type;
            std::string ipaddress;
            std::string url_fid;
            ylib::network::parse_url(host,http_type, host, ipaddress,port, url_fid);
        }

        network::http::ssl* ssl = nullptr;
        if (host_config["ssl"].to<bool>())
        {
            m_https = true;
            ylib::json ssl_config = find_ssl(host);

            if (ssl_config["enable"].to<bool>() == false)
                ylib::log->warn("The certificate is not open or does not exist, Host:"+ host);
            else
            {
                ssl = new network::http::ssl(
                    center()->server(port),
                    (network::http::ssl_verify_type)host_config["ssl_type"].to<int>(),
                    ssl_config["pem_cert"].to<std::string>(),
                    ssl_config["pem_key"].to<std::string>(),
                    ssl_config["pem_ca"].to<std::string>(),
                    ssl_config["pem_password"].to<std::string>());
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
                    if (ssl->bind(host) == false)
                    {
                        ylib::log->warn(ssl->last_error());
                        delete ssl;
                        ssl = nullptr;
                    }
                }
            }
        }

        hostpoint->init(host, port,ssl);
        m_hosts.push_back(hostpoint);
    }
        if(m_cache->start(config["cache"]) == false)
    {
         m_lastErrorDesc = m_cache->last_error();
         return false;

    }
    if (m_session->start(config["session"]["path"].to<std::string>(), config["session"]["timeout_sec"].to<uint32>()) == false)
    {
        m_lastErrorDesc = m_session->last_error();
        return false;
    }
    if (m_router->start(config["router"]) == false)
    {
        m_session->close();
        m_lastErrorDesc = m_router->last_error();
        return false;
    }
    if(m_cdn->start(config["cdn"]) == false)
    {
         m_lastErrorDesc = m_cdn->last_error();
         return false;
    }

    /*代理*/
    {
//        if (m_agent->start())
 //       {
            for (size_t i = 0; i < config["proxy"].size(); i++)
            {
                ylib::json proxy_cj = config["proxy"][i];
                network::http::proxy* proxy = new network::http::proxy;
                proxy->src_str = proxy_cj["src"].to<std::string>();
                proxy->src_express = std::regex(proxy->src_str.c_str());
                proxy->dst = proxy_cj["dst"].to<std::string>();

                if(proxy->dst == "/"){
                    proxy->dst.clear();
                }
                // 解析地址
                {
                    proxy->remote_url = proxy_cj["remote"].to<std::string>();
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
                proxy->host = proxy_cj["host"].to<std::string>();
                // 协议头
                for (size_t h = 0; h < proxy_cj["headers"]["request"].size(); h++)
                    proxy->request_headers[proxy_cj["headers"]["request"][h]["key"].to<std::string>()] = proxy_cj["headers"]["request"][h]["value"].to<std::string>();
                for (size_t h = 0; h < proxy_cj["headers"]["response"].size(); h++)
                    proxy->response_headers[proxy_cj["headers"]["response"][h]["key"].to<std::string>()] = proxy_cj["headers"]["response"][h]["value"].to<std::string>();
                m_proxy.append(proxy);
            }
   //     }
     //   else
       // {
        //    std::cout << "proxy start failed" << std::endl;
       // }
        
    }

    m_info.gzip = config["gzip"].to<bool>();
    m_info.rootdir = config["rootdir"].to<std::string>();
    m_info.default_codec = config["default_codec"].to<std::string>();
    m_info.download_maxbaud = config["download_maxbaud"].to<uint32>();

    return true;
}

void ylib::network::http::website::close()
{
}
network::http::router* network::http::website::router()
{
    return m_router;
}

network::http::session_mgr* ylib::network::http::website::session()
{
    return m_session;
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

const network::http::website_info* ylib::network::http::website::info()
{
    return &m_info;
}
#endif

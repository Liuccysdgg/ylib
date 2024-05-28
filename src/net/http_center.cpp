#include "net/http_center.h"
#if USE_NET_HTTP_WEBSITE
#include "net/http_router.h"
#include "net/http_server.h"
#include "net/http_session.h"
#include "net/http_website.h"
#include "net/util.h"
ylib::network::http::center::center()
{
    for (uint32 i = 0; i < 10; i++)
        m_temp[i] = 0;
}

ylib::network::http::center::~center()
{
}
bool ylib::network::http::center::create(const start_config& config)
{
    m_config = config;

    //创建http服务
    {
        //检查所有端口
        auto ports = listen_ports();
        
        for (size_t i = 0; i < ports.size(); i++)
        {
            //std::cout<<"ListenPort:"<<ports[i]<<std::endl;
            auto server = new network::http::server;
            server->center(this);
            server->create(port_have_ssl(ports[i]),ports[i]);
            m_server.push_back(server);
        }
    }
    //创建website站点
    for (size_t i = 0; i < m_config.website.size(); i++)
    {
        auto website = new network::http::website;
        website->center(this);
        if (!website->start(m_config.website[i]))
        {
            m_lastErrorDesc = "website(" + m_config.website[i].name + ") start failed," + website->last_error();
            return false;
        }
            
        m_website.push_back(website);
    }

    return true;
}
bool ylib::network::http::center::start()
{
    //启动http服务
    {
        //检查所有端口
        for (size_t i = 0; i < m_server.size(); i++)
        {
            if (m_server[i]->start() == false)
            {
                m_lastErrorDesc = "listen "+std::to_string(m_server[i]->port()) + " failed." + m_server[i]->last_error();
                return false;
            }
        }
    }
    //m_info.gzip = config["gzip"].to<bool>();
    //m_info.rootdir = config["rootdir"].to<nstring>();
    //m_info.default_codec = config["default_codec"].to<nstring>();
    //m_info.download_maxbaud = config["download_maxbaud"].to<uint32>();
    return true;

}

void ylib::network::http::center::close()
{
    for (size_t i = 0; i < m_server.size(); i++)
    {
        if (m_server[i] != nullptr)
        {
            m_server[i]->close();
            delete m_server[i];
        }
    }
    m_server.clear();
}

ylib::network::http::server* ylib::network::http::center::server(ushort port)
{

    for (size_t i = 0; i < m_server.size(); i++)
    {
        if (m_server[i]->port() == port)
        {
            return m_server[i];
        }
    }
    return nullptr; 
}

ylib::network::http::website* ylib::network::http::center::website(const std::string& host)
{
    for (size_t i = 0; i < m_website.size(); i++)
    {
        if (m_website[i]->host(host))
        {
            return m_website[i];
        }
    }
    return nullptr;
}

ylib::network::http::website* network::http::center::website_byname(const std::string& name)
{
    for (size_t i = 0; i < m_website.size(); i++)
    {
        if (m_website[i]->config().name == name)
        {
            return m_website[i];
        }
    }
    return nullptr;

}

std::vector<ushort> ylib::network::http::center::listen_ports()
{
    std::vector<ushort> ports;
    for (size_t i = 0; i < m_config.website.size(); i++)
    {
        for (size_t x = 0; x < m_config.website[i].host.size(); x++)
        {
            std::string domain = m_config.website[i].host[x].domain;
            bool find = false;
            for_iter(iter, ports)
            {
                if (*iter == m_config.website[i].host[x].port)
                {
                    find = true;
                    break;
                }
            }
            if(find == false)
                ports.push_back(m_config.website[i].host[x].port);
        }
    }
    return ports;
}

bool ylib::network::http::center::port_have_ssl(ushort port)
{
    for (size_t i = 0; i < m_config.website.size(); i++)
    {
        for (size_t x = 0; x < m_config.website[i].host.size(); x++)
        {
            if (m_config.website[i].host[x].port == port)
            {
                return m_config.website[i].host[x].ssl;
            }
        }
    }
    return false;
}
#endif

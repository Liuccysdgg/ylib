#include "ynet/http_center.h"
#if USE_NET_HTTP_WEBSITE
#include "ynet/http_router.h"
#include "ynet/http_server.h"
#include "ynet/http_session.h"
#include "ynet/http_website.h"
#include "ynet/util.h"
ylib::network::http::center::center()
{
    for (uint32 i = 0; i < 10; i++)
        m_temp[i] = 0;
}

ylib::network::http::center::~center()
{
}
bool ylib::network::http::center::create(const ylib::json& config)
{
    m_json_config = config;
    //缓存配置
    {
        m_config.clear();

        std::vector<std::string> keys = config.keys();
        for (size_t i = 0; i < keys.size(); i++)
        {
            if (config[keys[i]].is_number())
                m_config.set(keys[i], std::to_string(config[keys[i]].to<double>()), true);
            else if (config[keys[i]].is_bool())
                m_config.set(keys[i], config[keys[i]].to<bool>() == true ? "true" : "false", true);
            else
                m_config.set(keys[i], config[keys[i]].to<std::string>(), true);
        }


    }
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
    auto website_config = m_json_config["website"];
    for (size_t i = 0; i < website_config.size(); i++)
    {
        auto website = new network::http::website;
        website->center(this);
        website->start(website_config[i]);
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
std::string ylib::network::http::center::read_config(const std::string &name)
{
    std::string value;
    m_config.get(name, value);
    return value;
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
        if (m_website[i]->name() == name)
        {
            return m_website[i];
        }
    }
    return nullptr;

}

std::vector<ushort> ylib::network::http::center::listen_ports()
{
    std::vector<ushort> ports;
    for (size_t i = 0; i < m_json_config["website"].size(); i++)
    {
        ylib::json hosts = m_json_config["website"][i]["host"];
        for (size_t x = 0; x < hosts.size(); x++)
        {
            std::string host = hosts[x]["host"].to<std::string>();
            ushort port = 0;
            {
                std::string http_type;
                std::string ipaddress;
                std::string url_fid;
                ylib::network::parse_url(host, http_type, host, ipaddress,port,url_fid);
            }
            
            bool find = false;
            for_iter(iter, ports)
            {
                if (*iter == port)
                {
                    find = true;
                    break;
                }
            }
            if(find == false)
                ports.push_back(port);
        }
    }
    return ports;
}

bool ylib::network::http::center::port_have_ssl(ushort port)
{
    auto website = m_json_config["website"];
    for (size_t i = 0; i < website.size(); i++)
    {
        ylib::json host_config = website[i]["host"];
        for (size_t x = 0; x < host_config.size(); x++)
        {
            ushort host_port = 0;
            {
                std::string host;
                std::string http_type;
                std::string ipaddress;
                std::string url_fid;
                ylib::network::parse_url(host_config[x]["host"].to<std::string>(), http_type, host,ipaddress, host_port, url_fid);
            }
            if (host_port == port)
            {
                return host_config[x]["ssl"].to<bool>();
            }
        }
    }
    return false;
}
#endif

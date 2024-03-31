
#include "ynet/http_request.h"
#if USE_NET_HTTP_WEBSITE
#include "yutil/strutils.h"
#include "yutil/codec.h"
#include "ynet/http_session.h"
#include "ynet/http_server.h"
#include "ynet/http_reqpack.h"
#include "ynet/http_center.h"
#include "ynet/http_website.h"
#include "HPSocket/HPSocket.h"
#define HPSERVER ((IHttpServer*)m_reqpack->server()->hpserver())

ylib::network::http::request::request()
{
    m_reqpack = nullptr;
    m_session = nullptr;
}
ylib::network::http::request::~request()
{
    if (m_session != nullptr)
        delete m_session;
}

bool ylib::network::http::request::header(const std::string &name, std::string &value)
{

    LPCSTR lpszValue = nullptr;
    if (HPSERVER->GetHeader((CONNID)m_reqpack->connid(), name.c_str(), &lpszValue) == false)
        return false;
    value = strutils::F(lpszValue);
    return true;
}

ylib::network::http::method ylib::network::http::request::method()
{

    return m_reqpack->method();
}

std::string ylib::network::http::request::filepath()
{
    return m_reqpack->filepath();
}

std::string ylib::network::http::request::host()
{
    return m_reqpack->host();
}

bool ylib::network::http::request::cookie(const std::string &name, std::string &value)
{
    LPCSTR lpszValue = nullptr;
    if (HPSERVER->GetCookie((CONNID)m_reqpack->connid(), name.c_str(), &lpszValue) == false)
        return false;
    value = strutils::F(lpszValue);
    return true;
}
network::http::session* ylib::network::http::request::session(const std::string& name)
{
    if (m_session == nullptr)
    {
        m_session = new network::http::session;
        m_session->m_mgr = m_reqpack->website()->session();
        std::string session_name;
        if (cookie(name, session_name) && session_name != "")
        {
            m_session->init(session_name);
        }
    }
    return m_session;
}
network::http::reqpack* ylib::network::http::request::reqpack()
{
    return m_reqpack;
}
network::http::parser* ylib::network::http::request::parser()
{
    std::string content_type;
    {
        header("Content-Type", content_type);
        if (content_type == "")
        {
            content_type = "application/x-www-form-urlencoded";
        }
    }
    
    m_parser.init(strutils::F(HPSERVER->GetUrlField((CONNID)m_reqpack->connid(), HUF_QUERY)), m_reqpack->method(), m_reqpack->data(), content_type);
    return &m_parser;
}

std::string ylib::network::http::request::remote_ipaddress(bool find_header,const std::string& inside_ipaddress)
{
    bool is_inside = false;
    std::string ipaddress;
    ushort port;
    reqpack()->server()->remote(reqpack()->connid(),ipaddress,port);
    {
        if(strutils::left(ipaddress,2) == "10" || strutils::left(ipaddress,6) == "172.16" || strutils::left(ipaddress,7) == "192.168" || (ipaddress == inside_ipaddress && inside_ipaddress != ""))
            is_inside = true;
    }

    if(find_header && is_inside)
        header("X-Real-IP",ipaddress);
    return ipaddress;
}
ushort ylib::network::http::request::remote_port()
{
    std::string ipaddress;
    ushort port;
    reqpack()->server()->remote(reqpack()->connid(), ipaddress, port);
    return port;
}
#endif

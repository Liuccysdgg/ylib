#include "ynet/http_session.h"
#if USE_NET_HTTP_WEBSITE
#include "yutil/codec.h"
#include "yutil/system.h"
#include "yutil/time.h"
#include "yutil/file.h"
#include "ynet/http_server.h"
#include "ynet/http_center.h"
#include "ynet/http_website.h"


ylib::network::http::session::session()
{
    
}

ylib::network::http::session::~session()
{
}

void ylib::network::http::session::init(network::http::website* website, const std::string& id)
{
    m_website = website;
    m_id = id;
    if (m_website == nullptr)
    {
        throw exception("session WEBSITE is nullptr");
    }
    if (m_id.empty())
    {
        throw exception("session ID is empty");
    }
    update();
}

std::string ylib::network::http::session::id()
{
    return m_id;
}

void ylib::network::http::session::update()
{
    if (m_website == nullptr)
        throw ylib::exception("website is nullptr");
    m_website->session()->write(m_id+"/update",std::to_string(time::now_sec()+m_website->config().session.timeout_sec));
}

void ylib::network::http::session::set(const std::string& name, const std::string& value)
{
    if (m_website == nullptr)
        throw ylib::exception("website is nullptr");
    m_website->session()->write(m_id + "/sets/"+name, value);
}

std::string ylib::network::http::session::get(const std::string& name)
{
    if (m_website == nullptr)
        throw ylib::exception("website is nullptr");
    std::string value;
    m_website->session()->read(m_id + "/sets/" + name, value);
    return value;
}

bool ylib::network::http::session::check()
{
    if (m_id.empty())
        return false;
    if (m_website == nullptr)
        throw ylib::exception("website is nullptr");
    std::string value;
    if (m_website->session()->read(m_id + "/update", value) == false)
        return false;
    if (value.empty())
        return false;
    return ylib::stoll(value) > time::now_sec();
}
#endif
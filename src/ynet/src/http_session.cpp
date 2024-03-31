#include "ynet/http_session.h"
#if USE_NET_HTTP_WEBSITE
#include "yutil/codec.h"
#include "yutil/system.h"
#include "yutil/time.h"
#include "yutil/file.h"
#include "ynet/http_server.h"
#include "ynet/http_center.h"
ylib::network::http::session::session()
{
    m_mgr = nullptr;
}
ylib::network::http::session::~session()
{
    m_mgr->set(m_session_name,*this);
}
void ylib::network::http::session::init(const std::string& session_name)
{
    if (this->m_mgr == nullptr)return;


    ::ylib::json::clear();
    this->m_session_name = session_name;
    if (this->m_session_name == "")
    {
        this->m_session_name = this->m_mgr->create(this->m_mgr->m_timeout_sec);
        return;
    }
    if (this->m_mgr->get(this->m_session_name, *this) == false)
    {
        this->m_mgr->add(this->m_session_name, this->m_mgr->m_timeout_sec);
    }
}
void ylib::network::http::session::destory()
{
    this->m_mgr->del(m_session_name);
}

std::string ylib::network::http::session::session_id()
{
    return m_session_name;
}
void ylib::network::http::session::update()
{
    this->m_mgr->update(this->m_session_name, this->m_mgr->m_timeout_sec);
}
ylib::network::http::session_mgr::session_mgr():m_timeout_sec(0)
{
    m_cache = nullptr;
}
ylib::network::http::session_mgr::~session_mgr()
{
    close();
}
bool ylib::network::http::session_mgr::start(const std::string& cache_path,uint32 timeout_sec)
{

    if (m_cache != nullptr)
    {
        m_lastErrorDesc = "session管理器不允许重复加载";
        return false;
    }
    m_cache = new ylib::cache;
    m_timeout_sec = timeout_sec;
    return m_cache->start(cache_path);
}
void ylib::network::http::session_mgr::close()
{
    if (m_cache != nullptr)
    {
        delete (ylib::cache*)m_cache;
        m_cache = nullptr;
    }
}
std::string ylib::network::http::session_mgr::create(int32 timeout_sec)
{
    std::string session_name = codec::md5(std::to_string(time::now_sec()) + std::to_string(m_index.make()));
    m_cache->write(session_name, "{}", timeout_sec);
    return session_name;
}
bool ylib::network::http::session_mgr::add(const std::string& session_name, int32 timeout_sec)
{
    return m_cache->write(session_name, "{}", timeout_sec);
}
bool ylib::network::http::session_mgr::exist(const std::string& session_name)
{
    return m_cache->exist(session_name);
}
void ylib::network::http::session_mgr::set(const std::string& session_name, http::session& session)
{
    m_cache->write(session_name, session.to_string());
}
void ylib::network::http::session_mgr::del(const std::string& session_name)
{
    m_cache->remove(session_name);
}
bool ylib::network::http::session_mgr::get(const std::string &session_name,http::session& session)
{
    std::string value;
    if(!m_cache->read(session_name, value))
        return false;
    session.parse(value);
    return true;
}
void ylib::network::http::session_mgr::update(const std::string &session_name,int32 timeout_sec)
{
    m_cache->update(session_name);
}
void ylib::network::http::session_mgr::clear()
{
    m_cache->clear();
}
ylib::cache* ylib::network::http::session_mgr::cache()
{
    return m_cache;
}
#endif

/*Software License

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

#include "net/http_session.h"
#if USE_NET_HTTP_WEBSITE
#include "util/codec.h"
#include "util/system.h"
#include "util/time.h"
#include "util/file.h"
#include "net/http_server.h"
#include "net/http_center.h"
#include "net/http_website.h"


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
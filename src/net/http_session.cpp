// MIT License﻿
// Copyright(c) 2024 FastWeb - fwlua.com - nianhua﻿
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :﻿
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.﻿
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.﻿
// ## Additional Terms for Commercial Use﻿
// This software is licensed for personal, educational, and non - commercial use.
// For commercial use or use within a company, organization, or institution, a
// separate commercial license is required.To obtain a commercial license,
// please contact
// EMail：1585346868@qq.com

// QQ：1585346868

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
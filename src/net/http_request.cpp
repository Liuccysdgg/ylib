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
// Mobile：17367918735
// QQ：1585346868


#include "net/http_request.h"
#if USE_NET_HTTP_WEBSITE
#include "util/strutils.h"
#include "util/codec.h"
#include "net/http_session.h"
#include "net/http_server.h"
#include "net/http_reqpack.h"
#include "net/http_center.h"
#include "net/http_website.h"
#include "HPSocket/HPSocket.h"
#define HPSERVER ((IHttpServer*)m_reqpack->server()->hpserver())

ylib::network::http::request::request()
{
    m_reqpack = nullptr;
}
ylib::network::http::request::~request()
{
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
#if 0       
bool ylib::network::http::request::cookie(const std::string &name, std::string &value)
{
    LPCSTR lpszValue = nullptr;
    if (HPSERVER->GetCookie((CONNID)m_reqpack->connid(), name.c_str(), &lpszValue) == false)
        return false;
    value = strutils::F(lpszValue);
    return true;
}
#endif
network::http::session& ylib::network::http::request::session(const std::string& session_id)
{
    if (session_id.empty())
        return m_session;
    m_session.init(website(), session_id);
    return m_session;
}
std::string ylib::network::http::request::token()
{
    std::string token;
    header("token", token);
    return token;
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

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

#include "net/http_server.h"
#if USE_NET_HTTP_WEBSITE
#include <string.h>
#include "HPSocket/HPSocket.h"
#include "HPSocket/HPSocket-SSL.h"

#include "net/http_server_lst.h"
#include "net/http_center.h"
#include "net/http_ssl.h"
#include "net/http_host.h"
#include "net/http_agent.h"




#define HPSERVER ((IHttpServer*)m_server)
ylib::network::http::server::server()
{
    m_server = nullptr;
    m_listener = nullptr;
    m_https = false;
    m_port = 0;

    m_init_ssl = false;
#if USE_NET_HTTP_AGENT == 1
    m_agent = new network::http::agent();
#endif
}
ylib::network::http::server::~server()
{
    close();
#if USE_NET_HTTP_AGENT == 1
    delete m_agent;
#endif
}
bool ylib::network::http::server::start()
{
    //HPSERVER->SetAcceptSocketCount(10000);
    //HPSERVER->SetMaxConnectionCount(100000);
    HPSERVER->SetWorkerThreadCount(5);
    if (HPSERVER->Start("0.0.0.0", m_port) == false)
    {
#ifndef _WIN32
        this->m_lastErrorDesc = "start failed, error code:" + std::string(SYS_GetLastErrorStr());
#else
        this->m_lastErrorDesc = "start failed, error code:" + std::to_string((uint32)SYS_GetLastError());
#endif
        return false;
    }
#if USE_NET_HTTP_AGENT == 1
    if(m_agent->start() == false)
    {
#if HTTP_SERVER_PRINT == 1
        ylib::log->error("agent start failed");
#endif
    }
#endif
    return true;
}

bool ylib::network::http::server::create(bool https, ushort port)
{
    m_listener = new http_server_lst(this);
    m_listener->m_server = this;
    m_https = https;
    m_port = port;
    if (m_https)
    {
        m_server = HP_Create_HttpsServer(m_listener);
    }
    else
    {
        m_server = HP_Create_HttpServer(m_listener);
    }
    return true;
}

bool ylib::network::http::server::close()
{
#if USE_NET_HTTP_AGENT == 1
    m_agent->stop();
#endif
    if (m_server == nullptr)
        return true;
    //关闭HP HTTPSERVER
    if (HPSERVER->GetState() == SS_STOPPED)
        return false;
    if (HPSERVER->Stop() == false)
    {
#ifndef _WIN32
        this->m_lastErrorDesc = "stop failed, error code:" + std::string(SYS_GetLastErrorStr());
#else
        this->m_lastErrorDesc = "stop failed, error code:" + std::to_string((uint32)SYS_GetLastError());
#endif
        return false;
    }
    HPSERVER->Wait(-1);
    //销毁释放
    if(m_https)
        HP_Destroy_HttpsServer(HPSERVER);
    else
        HP_Destroy_HttpServer(HPSERVER);
    delete m_listener;

    m_server = nullptr;
    m_listener = nullptr;



    return true;
}

bool ylib::network::http::server::remote(uint64 connid, std::string &ipaddress, ushort &port)
{
    int ipAddrLen = 128;
    TCHAR ipAddr[128];
    memset(ipAddr, 128, 0);
    if(!HPSERVER->GetRemoteAddress((CONNID)connid, ipAddr, ipAddrLen,port))
        return false;
    ipaddress = ipAddr;
    port = port;
    return true;
}

void* ylib::network::http::server::hpserver()
{
    return m_server;
}
ylib::network::qps* ylib::network::http::server::qps()
{
    return &m_qps;
}
#endif

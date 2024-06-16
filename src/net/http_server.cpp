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
    if (HPSERVER->Start("0.0.0.0", m_config.port) == false)
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

bool ylib::network::http::server::create(server_config config)
{
    m_listener = new http_server_lst(this);
    m_listener->m_server = this;
    m_config = config;
    if (m_config.https)
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
    if(m_config.https)
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

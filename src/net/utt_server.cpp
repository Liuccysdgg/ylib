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

#include "net/utt_server.h"
#include "HPSocket/HPSocket.h"
#define UDPSERVER ((IUdpArqServer*)m_udp_server)
struct AgentExtra {
    uint64 udp_server_connid = 0;
    bool isConnected = false;
};
#define DEBUG_LOG 0
class UdpServerListener :public IUdpServerListener
{
public:
    UdpServerListener(ylib::network::utt_server* utt) :m_utt(utt)
    {

    }
    virtual EnHandleResult OnPrepareListen(IUdpServer* pSender, SOCKET soListen)
    {
#if DEBUG_LOG == 1
        printf("%s\r\n", "[UdpServer] OnPrepareConnect");
#endif
        return HR_OK;
    }
    virtual EnHandleResult OnSend(IUdpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
    {
#if DEBUG_LOG == 1
        printf("%s%ld%s%ld\r\n", "[UdpServer] OnSend dwConnID:", dwConnID, "\tLegnth:", iLength);
#endif
        return HR_OK;
    }
    virtual EnHandleResult OnReceive(IUdpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
    {
#if DEBUG_LOG == 1
        printf("%s%ld%s%ld\r\n", "[UdpServer] OnReceive dwConnID:", dwConnID, "\tLegnth:", iLength);
#endif
        PVOID pvoid_data = 0;
        if (pSender->GetConnectionExtra(dwConnID, &pvoid_data))
        {
            uint64 agentConnID = (uint64)pvoid_data;
            if (m_utt->m_agent->GetConnectionExtra((CONNID)agentConnID, &pvoid_data))
            {
                AgentExtra* extra = (AgentExtra*)pvoid_data;
                if (extra->isConnected)
                {
#if DEBUG_LOG == 1
                    printf("%s%ld%s%d\r\n", "[UdpServer] OnRecv[SendAgentClient] dwConnID:", dwConnID, "\tLegnth:", iLength);
#endif
                    m_utt->m_agent->Send((CONNID)agentConnID, pData, iLength);
                }
                else
                {
#if DEBUG_LOG == 1
                    printf("%s%ld%s%d\r\n", "[UdpServer] OnRecv[WaitAgentClient] dwConnID:", dwConnID, "\tLegnth:", iLength);
#endif
                    // 加入到待发送数据
                    ylib::network::utt_server::Packages* packages = nullptr;
                    if (m_utt->m_wait_send_data.get(agentConnID, packages))
                        packages->push_back(ylib::buffer((const char*)pData, iLength));
                }
            }
        }
        return HR_OK;
    }
    virtual EnHandleResult OnClose(IUdpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
    {
#if DEBUG_LOG == 1
        printf("%s%ld\r\n", "[UdpServer] OnClose dwConnID:", dwConnID);
#endif
        PVOID agent_dwConnID = 0;
        if (pSender->GetConnectionExtra(dwConnID, &agent_dwConnID))
        {
            m_utt->m_agent->Disconnect((uint64)agent_dwConnID);
            m_utt->m_wait_send_data.del((uint64)agent_dwConnID);
        }
        return HR_OK;
    }
    virtual EnHandleResult OnAccept(IUdpServer* pSender, CONNID dwConnID, UINT_PTR soClient)
    {
#if DEBUG_LOG == 1
        printf("%s%ld\r\n", "[UdpServer] OnAccept dwConnID:", dwConnID);
#endif
        return HR_OK;
    }
    virtual EnHandleResult OnShutdown(IUdpServer* pSender)
    {
#if DEBUG_LOG == 1
        printf("%s\r\n", "[UdpServer] OnShutdown");
#endif
        return HR_OK;
    }
    virtual EnHandleResult OnHandShake(IUdpServer* pSender, CONNID dwConnID)
    {
#if DEBUG_LOG == 1
        printf("%s%ld\r\n", "[UdpServer] OnHandShake dwConnID:", dwConnID);
#endif
        auto extra = new AgentExtra();
        extra->udp_server_connid = dwConnID;
        extra->isConnected = false;
        CONNID agent_dwConnID = 0;
        if (m_utt->m_agent->Connect(m_utt->m_remote_ap.address.c_str(), m_utt->m_remote_ap.port, &agent_dwConnID, (PVOID)extra) == false)
        {
            pSender->Disconnect(dwConnID);
            return HR_OK;
        }
        pSender->SetConnectionExtra(dwConnID, (PVOID)agent_dwConnID);
        m_utt->m_wait_send_data.del((uint64)agent_dwConnID);
        m_utt->m_wait_send_data.add((uint64)agent_dwConnID, new ylib::network::utt_server::Packages());
        return HR_OK;
    }
    virtual EnHandleResult OnReceive(IUdpServer* pSender, CONNID dwConnID, int iLength)
    {

        return HR_OK;
    }
private:
    ylib::network::utt_server* m_utt = nullptr;
};
class TcpAgentListener :public ITcpAgentListener
{
public:
    TcpAgentListener(ylib::network::utt_server* utt) :m_utt(utt)
    {

    }
    ~TcpAgentListener()
    {

    }
private:
    virtual EnHandleResult OnHandShake(ITcpAgent* pSender, CONNID dwConnID) override
    {
#if DEBUG_LOG == 1
        printf("%s%ld\r\n", "[TcpAgent] OnHandShake dwConnID:", dwConnID);
#endif
        ylib::network::utt_server::Packages* packages = nullptr;
        if (m_utt->m_wait_send_data.get(dwConnID, packages))
        {
            for (auto it = packages->rbegin(); it != packages->rend(); ++it) {
                pSender->Send(dwConnID,(const BYTE*)it->data(), (int)it->length());
            }
#if DEBUG_LOG == 1
            printf("%s%ld%s%ld\r\n", "[TcpAgent] OnHandShake dwConnID:", dwConnID, "\tpackages:", packages->size());
#endif
            packages->clear();
        }
        else
        {
#if DEBUG_LOG == 1
            printf("%s%ld\r\n", "[TcpAgent] OnHandShake dwConnID:", dwConnID);
#endif
        }

        PVOID pvoid_data = 0;
        if (pSender->GetConnectionExtra(dwConnID, &pvoid_data))
        {
            AgentExtra* extra = (AgentExtra*)pvoid_data;
            extra->isConnected = true;
        }
        return HR_OK;
    }
    virtual EnHandleResult OnSend(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
    {
#if DEBUG_LOG == 1
        printf("%s%ld%s%d\r\n", "[TcpAgent] OnSend dwConnID:", dwConnID, "\tLength:", iLength);
#endif
        return HR_OK;
    }
    virtual EnHandleResult OnReceive(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
    {
#if DEBUG_LOG == 1
        printf("%s%ld%s%d\r\n", "[TcpAgent] OnReceive dwConnID:", dwConnID, "\tLength:", iLength);
#endif
        PVOID pvoid_data = 0;
        if (pSender->GetConnectionExtra(dwConnID, &pvoid_data))
        {
            AgentExtra* extra = (AgentExtra*)pvoid_data;
            ((IUdpArqServer*)m_utt->m_udp_server)->Send((CONNID)extra->udp_server_connid, pData, iLength);
        }
            
        return HR_OK;
    }
    virtual EnHandleResult OnReceive(ITcpAgent* pSender, CONNID dwConnID, int iLength) override
    {
        return HR_OK;
    }
    virtual EnHandleResult OnClose(ITcpAgent* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode) override
    {
#if DEBUG_LOG == 1
        printf("%s%ld\r\n", "[TcpAgent] OnClose dwConnID:", dwConnID);
#endif
        PVOID pvoid_data = 0;
        if (pSender->GetConnectionExtra(dwConnID, &pvoid_data))
        {
            AgentExtra* extra = (AgentExtra*)pvoid_data;
            ((IUdpArqServer*)m_utt->m_udp_server)->Disconnect((CONNID)extra->udp_server_connid);
            delete extra;
        }
        return HR_OK;
    }
    virtual EnHandleResult OnShutdown(ITcpAgent* pSender) override
    {
#if DEBUG_LOG == 1
        printf("%s\r\n", "[TcpAgent] OnShutdown");
#endif
        return HR_OK;
    }
    virtual EnHandleResult OnPrepareConnect(ITcpAgent* pSender, CONNID dwConnID, SOCKET socket) override
    {
#if DEBUG_LOG == 1
        printf("%s%ld\r\n", "[TcpAgent] OnPrepareConnect dwConnID:", dwConnID);
#endif
        return HR_OK;
    }
    virtual EnHandleResult OnConnect(ITcpAgent* pSender, CONNID dwConnID) override
    {
        return HR_OK;
    }

private:
    ylib::network::utt_server* m_utt = nullptr;
};



ylib::network::utt_server::utt_server()
{
    m_agent_listener = new TcpAgentListener(this);
    m_udp_server_listener = new UdpServerListener(this);
    m_udp_server = HP_Create_UdpArqServer(m_udp_server_listener);
    m_agent = HP_Create_TcpAgent(m_agent_listener);
}

ylib::network::utt_server::~utt_server()
{
    HP_Destroy_TcpAgent(m_agent);
    HP_Destroy_UdpArqServer(UDPSERVER);
    delete m_agent_listener;
    delete m_udp_server_listener;
}

bool ylib::network::utt_server::start(const ylib::AddressPort& bind_ap,const ylib::AddressPort& remote_ap)
{
    m_remote_ap = remote_ap;
    if (m_agent->Start() == false)
        return false;
    if (UDPSERVER->Start(bind_ap.address.c_str(), bind_ap.port) == false)
    {
        
        m_agent->Stop();
        m_agent->Wait();
        return false;
    }
    return true;
}

void ylib::network::utt_server::stop()
{
    m_agent->Stop();
    m_agent->Wait();
    UDPSERVER->Stop();
    UDPSERVER->Wait();
}
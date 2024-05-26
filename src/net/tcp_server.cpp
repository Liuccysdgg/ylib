#include "net/tcp_server.h"
#ifdef USE_NET_TCP_SERVER
#include "net/tcp_server_lst.h"
#include "HPSocket/HPSocket.h"


ylib::network::tcp::server::server()
{

}


ylib::network::tcp::server::~server()
{
    close();
}

bool ylib::network::tcp::server::start(const ylib::AddressPort& bind_ap, ylib::receive_model model)
{
    m_model = model;
    //判断是否已创建
    if (m_server != nullptr)
    {
        this->m_lastErrorDesc = "started";
        return false;
    }
    //创建HPSocket
    m_server_listener = new network::tcp::server_lst(this);

    switch (m_model)
    {
    case ylib::PUSH_DEFAULT:
        m_server = (ITcpServer*)HP_Create_TcpServer(m_server_listener);
        break;
    case ylib::PACK:
        m_server = (ITcpServer*)HP_Create_TcpPackServer(m_server_listener);
        break;
    case ylib::PULL:
        m_server = (ITcpServer*)HP_Create_TcpPullServer(m_server_listener);
        break;
    default:
        break;
    }
       

    if (m_server->Start(bind_ap.address.c_str(), bind_ap.port) == false)
    {
        this->m_lastErrorDesc = "tcpserver start failed, error code:" + std::string(m_server->GetLastErrorDesc());
        close();
        return false;
    }

    return true;
    
}
void ylib::network::tcp::server::close()
{
    if (m_server != nullptr)
    {
        m_server->Stop();
        m_server->Wait();


        switch (m_model)
        {
        case ylib::PUSH_DEFAULT:
            HP_Destroy_TcpServer((ITcpServer*)m_server);
            break;
        case ylib::PACK:
            HP_Destroy_TcpPackServer((ITcpPackServer*)m_server);
            break;
        case ylib::PULL:
            HP_Destroy_TcpPullServer((ITcpPullServer*)m_server);
            break;
        default:
            break;
        }
        delete m_server_listener;
    }
    m_server = nullptr;
    m_server_listener = nullptr;
}
bool ylib::network::tcp::server::send(uint64 connid, const char* data, uint32 len)
{
    return m_server->Send((CONNID)connid, (const BYTE*)data, len);
}
bool ylib::network::tcp::server::send(uint64 connid, const ylib::buffer& data)
{
    return m_server->Send((CONNID)connid, (const BYTE*)data.data(), data.length());
}
bool ylib::network::tcp::server::started()
{
    return m_server != nullptr;
}
bool ylib::network::tcp::server::disConnect(uint64 connid)
{
    return m_server->Disconnect(connid);
}
bool ylib::network::tcp::server::isConnect(uint64 connid)
{
    return m_server->IsConnected(connid);
}
void* ylib::network::tcp::server::extra_ptr(uint64 connid)
{
    PVOID result = 0;
    if (!m_server->GetConnectionExtra(connid,&result))
        return nullptr;
    return (void*)result;
}
bool ylib::network::tcp::server::extra_ptr(uint64 connid, void* extra)
{
    return m_server->SetConnectionExtra(connid, extra);
}
#endif
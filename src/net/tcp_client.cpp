#include "net/tcp_client.h"
#ifdef USE_NET_TCP_CLIENT
#include "HPSocket/HPSocket.h"

#include "util/time.h"
#include "util/system.h"
#include "net/tcp_client_lst.h"
ylib::network::tcp::client::client(ylib::receive_model model):m_model(model)
{
}

ylib::network::tcp::client::~client()
{
    close();
}

bool ylib::network::tcp::client::start(const ylib::AddressPort& ap)
{
    if (start())
    {
        return connect(ap, -1);
    }
    return false;
}

bool ylib::network::tcp::client::start()
{
    //判断是否已创建
    if (m_client != nullptr)
    {
        this->m_lastErrorDesc = "started";
        return false;
    }
    //创建HPSocket
    m_client_listener = new network::tcp::client_lst(this);
    switch (m_model)
    {
    case ylib::PUSH_DEFAULT:
        m_client = (ITcpClient*)HP_Create_TcpClient(m_client_listener);
        break;
    case ylib::PACK:
        m_client = (ITcpClient*)HP_Create_TcpPackClient(m_client_listener);
        break;
    case ylib::PULL:
        m_client = (ITcpClient*)HP_Create_TcpPullClient(m_client_listener);
        break;
    default:
        break;
    }
    return true;
}

bool ylib::network::tcp::client::disconn()
{
    return m_client->Stop();
}

bool ylib::network::tcp::client::close(uint32 wait_msec)
{
    if (m_client == nullptr)
        return true;
    m_client->Stop();
    m_client->Wait();

    switch (m_model)
    {
    case ylib::PUSH_DEFAULT:
        HP_Destroy_TcpClient((ITcpClient*)m_client);
        break;
    case ylib::PACK:
        HP_Destroy_TcpPackClient((ITcpPackClient*)m_client);
        break;
    case ylib::PULL:
        HP_Destroy_TcpPullClient((ITcpPullClient*)m_client);
        break;
    default:
        break;
    }
        
    delete m_client_listener;

    m_client = nullptr;
    m_client_listener = nullptr;
    return true;
}


bool ylib::network::tcp::client::send(const char* buff, uint32 len)
{
    return m_client->Send((const BYTE*)buff,len);
}

//bool ylib::network::tcp::client::send(const buffer& data)
//{
//    return send(data.data(), data.length());
//}
bool ylib::network::tcp::client::connect(const ylib::AddressPort& remote_ap, int32 wait_msec)
{
    if (remote_ap.address != "" && remote_ap.port != 0)
        m_remote_ap = remote_ap;

    if (m_client->GetState() != SS_STOPPED)
    {
        this->m_lastErrorDesc = "not stopped";
        return false;
    }
    //连接
    if (m_client->Start(m_remote_ap.address.c_str(), m_remote_ap.port, TRUE) == false)
    {
        this->m_lastErrorDesc = m_client->GetLastErrorDesc();
        return false;
    }
    timestamp start_time = time::now_msec();
    while (m_client_listener->m_state == 0)
    {
        if (wait_msec != -1)
        {
            if (time::now_msec() - start_time > wait_msec)
                break;
        }
        ylib::system::sleep_msec(10);
    }
    return m_client_listener->m_state == 2;
}

#endif
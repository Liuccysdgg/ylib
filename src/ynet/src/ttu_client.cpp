#include "ynet/ttu_client.h"
#include "ynet/tcp_server.h"
#include "yutil/queue.hpp"
#include "HPSocket/HPSocket.h"
#define DEBUG_LOG 0
struct extra_tcp_server
{
    IUdpArqClient* client = nullptr;
    UdpClientListener* listener = nullptr;
};
class UdpClientListener :public IUdpClientListener
{
public:
    bool IsConnected() {
        return m_isConnected;
    }
    UdpClientListener(ylib::network::ttu_client* ttu, uint64 tcpserver_connid) :m_tcpserver_connid(tcpserver_connid), m_ttu(ttu)
    {

    }
    ~UdpClientListener()
    {

    }
    void setUdpClient(IUdpArqClient* udpclient) { m_client = udpclient; }

private:
    virtual EnHandleResult OnPrepareConnect(IUdpClient* pSender, CONNID dwConnID, SOCKET socket) override
    {
#if DEBUG_LOG == 1
        printf("%s%ld\r\n", "[UdpClient] OnPrepareConnect dwConnID:",dwConnID);
#endif
        return HR_OK;
    }
    virtual EnHandleResult OnConnect(IUdpClient* pSender, CONNID dwConnID) override
    {
#if DEBUG_LOG == 1
        printf("%s%ld\r\n", "[UdpClient] OnConnect dwConnID:",dwConnID);
#endif
        return HR_OK;
    }
    virtual EnHandleResult OnHandShake(IUdpClient* pSender, CONNID dwConnID) override
    {
        
        ylib::network::ttu_client::Packages *packages = nullptr;
        if (m_ttu->m_wait_send_data.get((uint64)m_client, packages))
        {

            for (auto it = packages->rbegin(); it != packages->rend(); ++it) {
                m_client->Send((const BYTE*)it->data(),(int)it->length());
            }
            packages->clear();
#if DEBUG_LOG == 1
            printf("%s%ld%s%ld\r\n", "[UdpClient] OnHandShake dwConnID:", dwConnID, "\tpackages:", packages->size());
#endif
        }
        else
        {
#if DEBUG_LOG == 1
            printf("%s%ld\r\n", "[UdpClient] OnHandShake dwConnID:", dwConnID);
#endif
        }

       


        m_isConnected = true;
        return HR_OK;
    }
    virtual EnHandleResult OnReceive(IUdpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
    {
#if DEBUG_LOG == 1
        printf("%s%ld%s%ld\r\n", "[UdpClient] OnReceive dwConnID:", dwConnID, "\tLegnth:", iLength);
#endif
        m_ttu->m_server->send(m_tcpserver_connid, (const char*)pData, iLength);
        return HR_OK;
    }
    virtual EnHandleResult OnSend(IUdpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
    {
#if DEBUG_LOG == 1
        printf("%s%ld%s%ld\r\n", "[UdpClient] OnSend dwConnID:", dwConnID, "\tLegnth:", iLength);
#endif
        return HR_OK;
    }
    virtual EnHandleResult OnClose(IUdpClient* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode) override
    {
        m_isConnected = false;
#if DEBUG_LOG == 1
        printf("%s%ld\r\n", "[UdpClient] OnClose dwConnID:", dwConnID);
#endif
        m_ttu->m_server->getHP()->Disconnect(m_tcpserver_connid);
        return HR_OK;
    }
    virtual EnHandleResult OnReceive(IUdpClient* pSender, CONNID dwConnID, int iLength) override
    {
        return HR_OK;
    }
   
private:
    bool m_isConnected = false;
    ylib::network::ttu_client* m_ttu = nullptr;
    IUdpArqClient* m_client = nullptr;
    uint64 m_tcpserver_connid = 0;
};
ylib::network::ttu_client::ttu_client()
{
    m_server = new network::tcp::server();
}

ylib::network::ttu_client::~ttu_client()
{
    close();
    delete m_server;
    m_server = nullptr;
}
bool ylib::network::ttu_client::start(const ylib::AddressPort& listen)
{
    m_server->on_close([&](ylib::network::tcp::server* server, uint64 connid) {
#if DEBUG_LOG == 1
        printf("%s%lld\r\n", "[TcpServer] OnClose dwConnID:", connid);
#endif
        PVOID pvoid_data = 0;
        if (server->getHP()->GetConnectionExtra(connid, &pvoid_data))
        {
            auto extra = (extra_tcp_server*)pvoid_data;
            if (extra == nullptr)
                return;
            // 待发送数据销毁
            {
                ttu_client::Packages* packages = nullptr;
                if (m_wait_send_data.get((uint64)extra->client, packages))
                    delete packages;
                m_wait_send_data.del((uint64)extra->client);
            }
            

            extra->client->Stop();
            extra->client->Wait(1000);
            HP_Destroy_UdpArqClient(extra->client);
            delete extra->listener;
            delete extra;
        }
    });
    m_server->on_handshake([&](ylib::network::tcp::server* server, uint64 connid) {
        if (m_max_connection_count != -1)
        {
#ifdef _DEBUG
            uint32 current_count = m_server->getHP()->GetConnectionCount();
#endif
            if (m_server->getHP()->GetConnectionCount() >= m_max_connection_count)
            {
                m_server->getHP()->Disconnect((CONNID)connid);
                return;
            }
        }
        extra_tcp_server* extra = new extra_tcp_server;
        extra->listener = new UdpClientListener(this,connid);
        extra->client = HP_Create_UdpArqClient(extra->listener);
        extra->listener->setUdpClient(extra->client);
        
#if DEBUG_LOG == 1
        printf("%s%lld\r\n", "[TcpServer] OnHandshake dwConnID:", connid);
#endif
        std::cout << "Remote:" << m_remote_ap.address << "\tPort:" << m_remote_ap.port << std::endl;
        if (extra->client->Start(m_remote_ap.address.c_str(), m_remote_ap.port, FALSE) == false)
        {
            server->getHP()->Disconnect(connid);
            HP_Destroy_UdpArqClient(extra->client);
            delete extra->listener;
            delete extra;
            return;
        }

        // 待发送数据初始化
        m_wait_send_data.del((uint64)extra->client);
        m_wait_send_data.add((uint64)extra->client, new ttu_client::Packages);

        server->getHP()->SetConnectionExtra(connid,(PVOID)extra);
    });
    m_server->on_recv([&](ylib::network::tcp::server* server, uint64 connid, const char* data, uint32 len) {
#if DEBUG_LOG == 1
        printf("%s%lld%s%ld\r\n", "[TcpServer] OnRecv dwConnID:", connid, "\tLegnth:", len);
#endif 
        PVOID pvoid_data = 0;
        if (server->getHP()->GetConnectionExtra(connid, &pvoid_data))
        {
            auto extra = (extra_tcp_server*)pvoid_data;
            if (extra == nullptr)
                return;
            if (extra->listener->IsConnected())
            {
#if DEBUG_LOG == 1
                printf("%s%lld%s%ld\r\n", "[TcpServer] OnRecv[SendUdpClient] dwConnID:", connid, "\tLegnth:", len);
#endif
                extra->client->Send((const BYTE*)data, len);
            }
            else
            {
#if DEBUG_LOG == 1
                printf("%s%lld%s%ld\r\n", "[TcpServer] OnRecv[WaitUdpClient] dwConnID:", connid, "\tLegnth:", len);
#endif
                // 加入到待发送数据
                ttu_client::Packages* packages = nullptr;
                if (m_wait_send_data.get((uint64)extra->client, packages))
                    packages->push_back(ylib::buffer(data, len));
            }
        }
        
    });
    if (m_server->start(listen) == false)
    {
        return false;
    }
    return true;
}

void ylib::network::ttu_client::close()
{
    m_server->close();
   
}
size_t ylib::network::ttu_client::getConnectionCount()
{
    return m_server->getHP()->GetConnectionCount();
}

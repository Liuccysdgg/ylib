#include "net/tcp_client_lst.h"
#ifdef USE_NET_TCP_CLIENT
#include "net/tcp_client.h"
ylib::network::tcp::client_lst::client_lst(client* client):m_client(client)
{
    m_state = 0;
}
ylib::network::tcp::client_lst::~client_lst()
{

}
EnHandleResult ylib::network::tcp::client_lst::OnPrepareConnect(ITcpClient* pSender, CONNID dwConnID, SOCKET socket)
{

    return HR_OK;

}
EnHandleResult ylib::network::tcp::client_lst::OnSend(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
    return HR_OK;

}
EnHandleResult ylib::network::tcp::client_lst::OnReceive(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
    if (iLength <= 0)
        return HR_OK;
    /*回调*/
    if (m_client->m_callback_onrecv != NULL)
    {
        m_client->m_callback_onrecv(this->m_client, (const char*)pData, iLength);
    }
    return HR_OK;
}
EnHandleResult ylib::network::tcp::client_lst::OnClose(ITcpClient* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
    this->m_state = 1;
    if (this->m_client->m_callback_onclose != nullptr)
        this->m_client->m_callback_onclose(this->m_client);
    return HR_OK;
}
EnHandleResult ylib::network::tcp::client_lst::OnReceive(ITcpClient* pSender, CONNID dwConnID, int iLength)
{
    if (this->m_client->m_callback_onfilter != nullptr)
        this->m_client->m_callback_onfilter(this->m_client, iLength);
    return HR_OK;
}
EnHandleResult ylib::network::tcp::client_lst::OnConnect(ITcpClient* pSender, CONNID dwConnID)
{
   
    return HR_OK;

}
EnHandleResult ylib::network::tcp::client_lst::OnHandShake(ITcpClient* pSender, CONNID dwConnID)
{
    this->m_state = 2;
    if (this->m_client->m_callback_onaccept != nullptr)
        this->m_client->m_callback_onaccept(this->m_client);
    return HR_OK;
}
#endif

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

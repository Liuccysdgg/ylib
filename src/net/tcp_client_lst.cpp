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

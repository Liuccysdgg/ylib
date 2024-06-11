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

#include "net/tcp_server_lst.h"
#ifdef USE_NET_TCP_SERVER
#include "net/tcp_server.h"

ylib::network::tcp::server_lst::server_lst(ylib::network::tcp::server* server):m_server(server)
{
	
}

ylib::network::tcp::server_lst::~server_lst()
{
}

EnHandleResult ylib::network::tcp::server_lst::OnPrepareListen(ITcpServer* pSender, SOCKET soListen)
{
	return HR_OK;
}

EnHandleResult ylib::network::tcp::server_lst::OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	if (m_server->m_callback_send != nullptr)
		m_server->m_callback_send(m_server, (uint64)dwConnID, (const char*)pData, iLength);
	return HR_OK;
}

EnHandleResult ylib::network::tcp::server_lst::OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	if (m_server->m_callback_recv != nullptr)
		m_server->m_callback_recv(m_server, (uint64)dwConnID, (const char*)pData, iLength);
	return HR_OK;
}

EnHandleResult ylib::network::tcp::server_lst::OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	if (m_server->m_callback_close != nullptr)
		m_server->m_callback_close(m_server, (uint64)dwConnID);
	return HR_OK;
}

EnHandleResult ylib::network::tcp::server_lst::OnAccept(ITcpServer* pSender, CONNID dwConnID, UINT_PTR soClient)
{
	if (m_server->m_callback_accept != nullptr)
		m_server->m_callback_accept(m_server, (uint64)dwConnID);
	return HR_OK;
}

EnHandleResult ylib::network::tcp::server_lst::OnShutdown(ITcpServer* pSender)
{
	return HR_OK;
}

EnHandleResult ylib::network::tcp::server_lst::OnHandShake(ITcpServer* pSender, CONNID dwConnID)
{
	if (m_server->m_callback_handshake != nullptr)
		m_server->m_callback_handshake(m_server, (uint64)dwConnID);
	return HR_OK;
}

EnHandleResult ylib::network::tcp::server_lst::OnReceive(ITcpServer* pSender, CONNID dwConnID, int iLength)
{
	if (m_server->m_callback_filter != nullptr)
		m_server->m_callback_filter(m_server, (uint64)dwConnID, iLength);
	return HR_OK;
}

#endif
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
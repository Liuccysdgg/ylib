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
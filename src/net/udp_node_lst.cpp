#include "net/udp_node_lst.h"
#if USE_NET_UDP_NODE
#include "net/udp_node.h"

ylib::network::udp::node_lst::node_lst(ylib::network::udp::node* node):m_node(node)
{
}

ylib::network::udp::node_lst::~node_lst()
{
}

EnHandleResult ylib::network::udp::node_lst::OnPrepareListen(IUdpNode* pSender, SOCKET soListen)
{
	return HR_OK;
}

EnHandleResult ylib::network::udp::node_lst::OnSend(IUdpNode* pSender, LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE* pData, int iLength)
{
	return HR_OK;
}

EnHandleResult ylib::network::udp::node_lst::OnReceive(IUdpNode* pSender, LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE* pData, int iLength)
{
	if (m_node->m_callback_recv != nullptr)
		m_node->m_callback_recv(m_node, { lpszRemoteAddress,usRemotePort }, (const char*)pData, iLength);
	return HR_OK;
}

EnHandleResult ylib::network::udp::node_lst::OnError(IUdpNode* pSender, EnSocketOperation enOperation, int iErrorCode, LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE* pBuffer, int iLength)
{
	return HR_OK;
}

EnHandleResult ylib::network::udp::node_lst::OnShutdown(IUdpNode* pSender)
{
	return HR_OK;
}

#endif
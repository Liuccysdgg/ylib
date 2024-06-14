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
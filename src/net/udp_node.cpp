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

#include "net/udp_node.h"
#if USE_NET_UDP_NODE
#include "HPSocket/HPSocket.h"
#include "net/udp_node_lst.h"
ylib::network::udp::node::node()
{
}

ylib::network::udp::node::~node()
{
}

bool ylib::network::udp::node::start(const ylib::AddressPort& bind_ap)
{
	//判断是否已存在服务端
	if (this->m_node != nullptr)
	{
		this->m_lastErrorDesc = "started";
		return false;
	}
	m_node_listener = new ylib::network::udp::node_lst(this);
	m_node = HP_Create_UdpNode(m_node_listener);
	
	m_local_ap = bind_ap;
	//m_local_ap.port = bind_ap.port;
	if (m_node->Start(bind_ap.address.empty()==true?nullptr:bind_ap.address.c_str(), bind_ap.port) == false)
	{
		this->m_lastErrorDesc = "node start failed, error code:" + std::string(m_node->GetLastErrorDesc());
		close();
		return false;
	}
	return true;
}

bool ylib::network::udp::node::close()
{
	HP_Destroy_UdpNode(m_node);
	delete m_node_listener;
	m_node_listener = nullptr;
	m_node = nullptr;
	return false;
}

bool ylib::network::udp::node::send(const std::string& remote_ipaddress, ushort remote_port, const char* pData, uint32 len)
{
	return m_node->Send(remote_ipaddress.c_str(),remote_port,(const BYTE*)pData,len);
}
ushort ylib::network::udp::node::listen_port()
{
	TCHAR address[256];
	int address_len = 256;
	ushort port = 0;
	m_node->GetLocalAddress(address, address_len, port);
	return port;
}
#endif
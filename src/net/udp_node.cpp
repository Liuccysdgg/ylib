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
	if (m_node->Start(bind_ap.address.c_str(), bind_ap.port) == false) 
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
#endif
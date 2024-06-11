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
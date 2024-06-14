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

#include "net/tcp_agent.h" 
#include "net/tcp_agent_lst.h"
ylib::network::tcp::agent::agent()
{
	m_listener = new network::tcp::agent_lst(this);
}
ylib::network::tcp::agent::~agent()
{
	stop();
	delete m_listener;
}
bool ylib::network::tcp::agent::start(bool async, ylib::receive_model model)
{
	m_model = model;
	if (m_agent != nullptr)
	{
		m_lastErrorDesc = "started";
		return false;
	}
	switch (m_model)
	{
	case ylib::PUSH_DEFAULT:
		m_agent = HP_Create_TcpAgent(m_listener);
		break;
	case ylib::PACK:
		m_agent = HP_Create_TcpPackAgent(m_listener);
		break;
	case ylib::PULL:
		m_agent = HP_Create_TcpPullAgent(m_listener);
		break;
	default:
		break;
	}
	return m_agent->Start(NULL, async);
}
void ylib::network::tcp::agent::stop()
{
	if (m_agent == nullptr)
		return;
	m_agent->Stop();
	m_agent->Wait();



	switch (m_model)
	{
	case ylib::PUSH_DEFAULT:
		HP_Destroy_TcpAgent((ITcpPullAgent*)m_agent);
		break;
	case ylib::PACK:
		HP_Destroy_TcpPackAgent((ITcpPackAgent*)m_agent);
		break;
	case ylib::PULL:
		HP_Destroy_TcpPullAgent((ITcpPullAgent*)m_agent);
		break;
	default:
		break;
	}
		
	m_agent = nullptr;
}
bool ylib::network::tcp::agent::connect(const std::string& address,const ushort& port, uint64& connid, void* extra)
{
	CONNID dwConnID = 0;
	auto result = m_agent->Connect(address.c_str(),port,&dwConnID,(PVOID)extra);
	if (!result)
		return false;
	connid = dwConnID;
	return true;
}
bool ylib::network::tcp::agent::disConnect(uint64 connid)
{
	return m_agent->Disconnect(connid);
}
bool ylib::network::tcp::agent::isConnect(uint64 connid)
{
	return m_agent->IsConnected(connid);
}
bool ylib::network::tcp::agent::send(uint64 connid, const char* data, uint32 len)
{
	return m_agent->Send(connid, (const BYTE*)data, len);
}
bool ylib::network::tcp::agent::send(uint64 connid, const ylib::buffer& data)
{
	return m_agent->Send(connid, (const BYTE*)data.data(), data.length());
}
void* ylib::network::tcp::agent::extra_ptr(uint64 connid)
{
	PVOID extra = 0;
	m_agent->GetConnectionExtra(connid, &extra);
	return (void*)extra;
}
bool ylib::network::tcp::agent::extra_ptr(uint64 connid, void* extra)
{
	return m_agent->SetConnectionExtra(connid, extra);
}
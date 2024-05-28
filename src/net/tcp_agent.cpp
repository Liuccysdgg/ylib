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
#include "net/tcp2tcp.h"
#include "net/tcp_server.h"
#include "net/tcp_agent.h"
ylib::network::tcp::tcp2tcp::tcp2tcp()
{
	m_server = new network::tcp::server();
	m_agent = new network::tcp::agent();
}
ylib::network::tcp::tcp2tcp::~tcp2tcp()
{
	stop();
	delete m_server;
	delete m_agent;
}
bool ylib::network::tcp::tcp2tcp::start(const std::string& listen_address, ushort listen_port, bool agent_async_connect, ylib::receive_model agent_model, ylib::receive_model server_model)
{
	if (m_server->started())
	{
		m_lastErrorDesc = "started";
		return false;
	}
	if (m_server->start({ listen_address, listen_port }, server_model) == false)
	{
		m_lastErrorDesc = m_server->last_error();
		return false;
	}
	if (m_agent->start(agent_async_connect, agent_model) == false)
	{
		m_server->close();
		m_lastErrorDesc = m_agent->last_error();
		return false;
	}
	return true;
}
void ylib::network::tcp::tcp2tcp::stop()
{
	m_server->close();
	m_agent->stop();
}

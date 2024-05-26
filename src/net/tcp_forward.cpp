#include "net/tcp_forward.h"
#include "net/tcp_agent.h"
#include "net/tcp_server.h"
#include "HPSocket/HPSocket.h"
#include "util/strutils.h"
ylib::network::tcp::forward::forward()
{
}

ylib::network::tcp::forward::~forward()
{
}

bool ylib::network::tcp::forward::start(const std::string& listen_address,ushort listen_port)
{
	//================= AGENT ================
	m_t2t.agent()->on_connect([&](ylib::network::tcp::agent* agent, uint64 connid) {

	});
	m_t2t.agent()->on_close([&](ylib::network::tcp::agent* agent, uint64 connid) {
		uint64 server_connid = (uint64)agent->extra_ptr(connid);
		if (server_connid == 0)
			return;
		m_t2t.server()->disConnect(server_connid);
	});
	m_t2t.agent()->on_receive([&](ylib::network::tcp::agent* agent, uint64 connid, const char* data, int len) {
		uint64 server_connid = (uint64)agent->extra_ptr(connid);
		if (server_connid == 0)
			return;
		m_t2t.server()->send(server_connid,data,len);
	});
	m_t2t.agent()->on_handshake([&](ylib::network::tcp::agent* agent, uint64 connid) {
		Packages* packages = nullptr;
		if (m_wait_send_data.get(connid, packages))
		{
			for (auto it = packages->rbegin(); it != packages->rend(); ++it) {
				agent->send(connid,(const char*)it->data(), (int)it->length());
			}
			packages->clear();
		}
	});
	//================= SERVER ================
	m_t2t.server()->on_accept([&](ylib::network::tcp::server* server, uint64 connid) {

	});
	m_t2t.server()->on_close([&](ylib::network::tcp::server* server, uint64 connid) {
		uint64 agent_connid = (uint64)server->extra_ptr(connid);
		if (agent_connid == 0)
			return;
		// 销毁待发送数据
		{
			Packages* packages = nullptr;
			if (m_wait_send_data.get(agent_connid, packages))
				delete packages;
			m_wait_send_data.del(agent_connid);
		}
		// 断开代理
		m_t2t.agent()->disConnect(agent_connid);
	});
	m_t2t.server()->on_handshake([&](ylib::network::tcp::server* server, uint64 connid) {

		if (m_max_connection_count != 0)
		{
			if (m_t2t.server()->getHP()->GetConnectionCount() > m_max_connection_count)
			{
				m_t2t.server()->disConnect(connid);
				return;
			}
		}
		uint64 agent_connid = 0;
		if (m_t2t.agent()->connect(m_remote.address, m_remote.port, agent_connid, (void*)connid) == false)
			server->disConnect(connid);
		m_t2t.server()->extra_ptr(connid,(void*)agent_connid);
	});
	m_t2t.server()->on_recv([&](ylib::network::tcp::server* server, uint64 connid, const char* data, uint32 len) {
		uint64 agent_connid = (uint64)server->extra_ptr(connid);


		if (m_t2t.agent()->isConnect(agent_connid) == false)
		{
			Packages * packages = nullptr;
			if (m_wait_send_data.get(agent_connid, packages))
				packages->push_back(ylib::buffer(data, len));
		}
		else
		{
			m_t2t.agent()->send(agent_connid,data, len);
		}
	});

	return m_t2t.start(listen_address,listen_port);
}

void ylib::network::tcp::forward::stop()
{
	m_t2t.stop();
}

uint64 ylib::network::tcp::forward::connectionCount()
{
	return m_t2t.server()->getHP()->GetConnectionCount();
}

void ylib::network::tcp::forward::disIpAddress(const std::string& ipaddress)
{
	DWORD connCount = 0;
	if (m_t2t.server()->getHP()->GetAllConnectionIDs(NULL, connCount) == false || connCount == 0)
		return;
	CONNID* ids = new CONNID[connCount];
	
	int ipAddrLen = 128;
	TCHAR ipAddr[128];
	USHORT port = 0;
	for (size_t i = 0; i < connCount; i++)
	{
		memset(ipAddr, 128, 0);
		if (!m_t2t.server()->getHP()->GetRemoteAddress(ids[i], ipAddr, ipAddrLen, port))
			continue;
#if UNICODE == 1
		std::string id_ip =strutils::wto_string(ipAddr);
#else
		std::string id_ip = ipAddr;
#endif
		if (ipaddress == id_ip)
		{
			m_t2t.server()->disConnect(ids[i]);
		}
	}
	delete[] ids;
}

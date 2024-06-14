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

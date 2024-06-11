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

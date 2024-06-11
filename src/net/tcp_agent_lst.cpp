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

#include "net/tcp_agent_lst.h"
#include "net/tcp_agent.h"
ylib::network::tcp::agent_lst::agent_lst(ylib::network::tcp::agent* agent):m_agent(agent)
{

}
ylib::network::tcp::agent_lst::~agent_lst()
{

}
EnHandleResult ylib::network::tcp::agent_lst::OnHandShake(ITcpAgent* pSender, CONNID dwConnID)
{
	if(m_agent->m_on_handshake_callback != nullptr)
		m_agent->m_on_handshake_callback(m_agent, dwConnID);
	return HR_OK;
}
EnHandleResult ylib::network::tcp::agent_lst::OnSend(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	if (m_agent->m_on_send_callback != nullptr)
		m_agent->m_on_send_callback(m_agent, dwConnID,(const char*)pData,iLength);
	return HR_OK;
}
EnHandleResult ylib::network::tcp::agent_lst::OnReceive(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	if (m_agent->m_on_receive_callback != nullptr)
		m_agent->m_on_receive_callback(m_agent, dwConnID,(const char*)pData,iLength);
	return HR_OK;
}
EnHandleResult ylib::network::tcp::agent_lst::OnReceive(ITcpAgent* pSender, CONNID dwConnID, int iLength)
{
	if (m_agent->m_on_receive_notdata_callback != nullptr)
		m_agent->m_on_receive_notdata_callback(m_agent, dwConnID,iLength);
	return HR_OK;
}
EnHandleResult ylib::network::tcp::agent_lst::OnClose(ITcpAgent* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	if (m_agent->m_on_close_callback != nullptr)
		m_agent->m_on_close_callback(m_agent, dwConnID);
	return HR_OK;
}
EnHandleResult ylib::network::tcp::agent_lst::OnShutdown(ITcpAgent* pSender)
{
	if (m_agent->m_on_shutdown_callback != nullptr)
		m_agent->m_on_shutdown_callback(m_agent);
	return HR_OK;
}
EnHandleResult ylib::network::tcp::agent_lst::OnPrepareConnect(ITcpAgent* pSender, CONNID dwConnID, SOCKET socket)
{
	if (m_agent->m_on_prepareconnect_callback != nullptr)
		m_agent->m_on_prepareconnect_callback(m_agent, dwConnID);
	return HR_OK;
}
EnHandleResult ylib::network::tcp::agent_lst::OnConnect(ITcpAgent* pSender, CONNID dwConnID) 
{
	if (m_agent->m_on_connect_callback != nullptr)
		m_agent->m_on_connect_callback(m_agent, dwConnID);
	return HR_OK;
}
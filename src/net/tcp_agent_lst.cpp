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
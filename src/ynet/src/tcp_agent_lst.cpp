#include "ynet/tcp_agent_lst.h"
#include "ynet/tcp_agent.h"
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
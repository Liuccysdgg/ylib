//#include "connection.h"
//#include "ynet/tcp_agent.h"
//#include "ynet/tcp_server.h"
//#include "define.h"
//ns::client::connectionManager::connectionManager(network::tcp::tcp2tcp* t2t):m_t2t(t2t)
//{
//}
//
//ns::client::connectionManager::~connectionManager()
//{
//}
//
//ns::client::connection* ns::client::connectionManager::create()
//{
//	ns::client::connection* connection = new ns::client::connection(this);;
//	return connection;
//}
//
//void ns::client::connectionManager::destory(ns::client::connection* connection)
//{
//	
//	if (m_waitDestoryConnection == connection)
//		return;
//	if (m_waitDestoryConnection != nullptr)
//		delete m_waitDestoryConnection;
//	m_waitDestoryConnection = connection;
//}
//
//ns::client::connection* ns::client::connectionManager::getByAgent(uint64 agent_connid)
//{
//	auto iter = m_aID.find(agent_connid);
//	if (iter == m_aID.end())
//		return nullptr;
//	return iter->second;
//}
//
//ns::client::connection* ns::client::connectionManager::getByServer(uint64 server_connid)
//{
//	auto iter = m_sID.find(server_connid);
//	if (iter == m_sID.end())
//		return nullptr;
//	return iter->second;
//}
//
//void ns::client::connectionManager::setAgentId(uint64 agent_connid, ns::client::connection* conntion)
//{
//	if (m_aID.find(agent_connid) != m_aID.end())
//		abort();
//	m_aID.emplace(agent_connid, conntion);
//}
//
//void ns::client::connectionManager::setServerId(uint64 server_connid, ns::client::connection* conntion)
//{
//	if (m_sID.find(server_connid) != m_sID.end())
//		abort();
//	m_sID.emplace(server_connid, conntion);
//}
//
//void ns::client::connectionManager::freeAgentID(uint64 agent_connid)
//{
//	auto iter_agent = m_aID.find(agent_connid);
//	if (iter_agent != m_aID.end())
//		m_aID.erase(iter_agent);
//}
//
//void ns::client::connectionManager::freeServerID(uint64 server_connid)
//{
//	auto iter_server = m_sID.find(server_connid);
//	if (iter_server != m_sID.end())
//		m_sID.erase(iter_server);
//}
//
//
//
//
//ns::client::connection::connection(ns::client::connectionManager* manager):m_manager(manager)
//{
//
//}
//
//ns::client::connection::~connection()
//{
//
//}
//
//void ns::client::connection::setAgentID(uint64 connid)
//{
//	if (m_agent_id == connid)
//		return;
//	if (m_agent_id != 0)
//		abort(); 
//	m_agent_id = connid;
//	m_manager->setAgentId(connid,this);
//}
//
//void ns::client::connection::setServerID(uint64 connid)
//{
//	if (m_server_id == connid)
//		return;
//	if (m_server_id != 0)
//		abort(); 
//	m_server_id = connid;
//	m_manager->setServerId(connid,this);
//}
//
//bool ns::client::connection::connectAgent()
//{
//	if (m_agent_connect_status == Connecting)
//		return false;
//	uint64 agent_connid = 0;
//	if (m_agent_status == ns::client::NotAS)
//	{
//		std::cout << "-> First Connect" << std::endl;
//	}
//	else
//	{
//#ifdef _DEBUG
//			std::cout << "-> ReConnect:" << m_reconnect_count << std::endl;
//#endif
//	}
//
//	m_agent_connect_status = Connecting;
//	// 建立集群连接
//	m_reconnect_count++;
//	auto cluster_ap = ns::client::cluster_remote_address();
//	if (m_manager->t2t()->agent()->connect(cluster_ap.address, cluster_ap.port, agent_connid) == false)
//	{
//
//		m_agent_connect_status = DisConnect;
//		return false;
//	}
//	setAgentID(agent_connid);
//	return true;
//}
//
//void ns::client::connection::disConnectServer(bool inEvent)
//{
//	if (inEvent == false)
//	{
//		if (m_server_id != 0)
//		{
//			// 发送终端主动断开请求
//			m_manager->t2t()->server()->disConnect(m_server_id);
//		}
//	}
//	if (m_server_id != 0)
//		m_manager->freeServerID(m_server_id);
//	m_server_id = 0;
//	checkFree();
//}
//
//void ns::client::connection::disConnectAgent(bool inEvent)
//{
//	if (inEvent == false)
//	{
//		if (m_agent_id != 0)
//		{
//			// 发送终端主动断开请求
//			m_manager->t2t()->agent()->send(m_agent_id, make_cmd(0xE5C1));
//		}
//	}
//	if (m_agent_id != 0)
//		m_manager->freeAgentID(m_agent_id);
//	m_agent_id = 0;
//	checkFree();
//}
//
//void ns::client::connection::ClusterConnect()
//{
//	m_manager->t2t()->agent()->send(m_agent_id, make_cmd<int64>(0xE4A1, NS_USER_ID));
//}
//
//void ns::client::connection::HandleClusterMsg(const char* data, uint32 len)
//{
//	// 集群返回
//	switch ((uchar)data[1])
//	{
//	case 0xA1:
//		std::cout << "->[Cluster] Connect Gateway Success" << std::endl;
//		m_agent_status = ns::client::AgentStatus::HandShakeGatewaySuccess;
//		// 与网关建立连接
//		m_manager->t2t()->agent()->send(m_agent_id, make_cmd<int64>(0xE5A1, (int64)m_src_connid));
//		break;
//	case 0xA2:
//		std::cout << "->[Cluster] Gateway offLine" << std::endl;
//		m_manager->t2t()->agent()->disConnect(m_agent_id);
//		break;
//	case 0xA3:
//		std::cout << "->[Cluster] The user load exceeds the limit" << std::endl;
//		m_manager->t2t()->agent()->disConnect(m_agent_id);
//		break;
//	case 0xA4:
//		std::cout << "->[Cluster] Not Find UserID" << std::endl;
//		m_manager->t2t()->agent()->disConnect(m_agent_id);
//		break;
//	case 0xA5:
//		std::cout << "->[Cluster] Not have Gateway" << std::endl;
//		m_manager->t2t()->agent()->disConnect(m_agent_id);
//		break;
//	case 0xA6:
//		std::cout << "->[Cluster] Check Auth Failed" << std::endl;
//		m_manager->t2t()->agent()->disConnect(m_agent_id);
//		break;
//	case 0xA7:
//		std::cout << "->[Cluster] Load exceeding limit" << std::endl;
//		m_manager->t2t()->agent()->disConnect(m_agent_id);
//		break;
//	case 0xB1:
//		// Gateway断开连接或连接丢失，尝试再次连接。
//		std::cout << "->[Cluster] Gateway disconnect" << std::endl;
//		break;
//	default:
//		break;
//	}
//}
//
//void ns::client::connection::HandleGatewayMsg(const char* data, uint32 len)
//{
//	// 集群返回
//	switch ((uchar)data[1])
//	{
//	case 0xA1:
//		m_agent_status = ns::client::HandShakeSrcSuccess;
//		// 读取SRC CONNID
//		{
//			if (len != 14)
//				return;
//			bytes::to_uint64(m_src_connid, data + 2 + 4);
//			std::cout << "->[Gateway] Connect Success SRC_connid:" << m_src_connid << std::endl;
//		}
//		// 发送待发送数据
//		{
//			ylib::buffer sd;
//			while (m_agent_wait_send.pop(sd))
//			{
//				m_manager->t2t()->agent()->send(m_agent_id, sd);
//			}
//		}
//		break;
//	case 0xA2:
//		std::cout << "->[Gateway] Unassigned SRC service" << std::endl;
//		m_manager->t2t()->agent()->disConnect(m_agent_id);
//		break;
//	case 0xA3:
//		std::cout << "->[Gateway] SRC service connection failed" << std::endl;
//		m_manager->t2t()->agent()->disConnect(m_agent_id);
//		break;
//	case 0xA4:
//		std::cout << "->[Gateway] Service exception - load exceeding limit" << std::endl;
//		m_manager->t2t()->agent()->disConnect(m_agent_id);
//		break;
//	case 0xA5:
//		std::cout << "->[Gateway] CONNID not available" << std::endl;
//		m_manager->t2t()->agent()->disConnect(m_agent_id);
//		break;
//	case 0xD1:
//	{
//		if (len < 6)
//			return;
//		int32 length = 0;
//		bytes::to_int(length, data + 2);
//		if (len != 2 + 4 + length)
//			return;
//		ylib::buffer recv_data;
//		recv_data.append(data + 6, length);
//		//std::cout << "->[Gateway] SRC Msg:"<< recv_data.to_string() << std::endl;
//		m_manager->t2t()->server()->send(m_server_id, data + 6, length);
//	}
//	break;
//	default:
//		break;
//	}
//}
//
//void ns::client::connection::HandleClientMsg(const char* data, uint32 len)
//{
//	// 收到客户端消息
//	bool wait_push = true;
//	if (m_agent_connect_status == Connected)
//	{
//		switch (m_agent_status)
//		{
//		case ns::client::HandShakeSrcSuccess:
//			wait_push = false;
//			break;
//		default:
//			break;
//		}
//	}
//	else
//	{
//		if (m_agent_status == ns::client::HandShakeSrcSuccess)
//		{
//			// 超过最大重新连接次数
//			if (this->reconnectCount() >= 3)
//			{
//				this->disConnectAgent(false);
//				this->disConnectServer(false);
//				return;
//			}
//			// 重连
//			this->connectAgent();
//		}
//	}
//
//	if (wait_push)
//	{
//		m_agent_wait_send.push(make_data(0xE5D1, data, len));
//	}
//	else
//	{
//		m_manager->t2t()->agent()->send(m_agent_id, make_data(0xE5D1, data, len));
//	}
//}
//
//void ns::client::connection::checkFree()
//{
//	if (m_agent_id == 0 && m_server_id == 0)
//		m_manager->destory(this);
//}

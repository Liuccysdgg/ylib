//#pragma once
//#include "ybase/define.h"
//#include "ynet/tcp2tcp.h"
//#include "yutil/queue.hpp"
//#include <mutex>
////#include "../common/common.h"
//
//namespace ns::client
//{
//	enum AgentStatus
//	{
//		// 未连接
//		NotAS,
//		// 连接集群成功
//		ConnectClusterSuccess,
//		// 握手网关成功
//		HandShakeGatewaySuccess,
//		// 握手源成功
//		HandShakeSrcSuccess,
//	};
//	class connection;
//	class connectionManager
//	{
//	public:
//		connectionManager(network::tcp::tcp2tcp* t2t);
//		~connectionManager();
//
//		ns::client::connection* create();
//		void destory(ns::client::connection* connection);
//		ns::client::connection* getByAgent(uint64 agent_connid);
//		ns::client::connection* getByServer(uint64 server_connid);
//		
//		network::tcp::tcp2tcp* t2t() { return m_t2t; }
//
//		friend class ns::client::connection;
//	private:
//		void setAgentId(uint64 agent_connid, ns::client::connection* conntion);
//		void setServerId(uint64 server_connid, ns::client::connection* conntion);
//
//		void freeAgentID(uint64 agent_connid);
//		void freeServerID(uint64 server_connid);
//	private:
//		std::map<uint64, ns::client::connection*> m_aID;
//		std::map<uint64, ns::client::connection*> m_sID;
//		network::tcp::tcp2tcp* m_t2t = nullptr;
//		ns::client::connection* m_waitDestoryConnection = nullptr;
//	};
//	class connection
//	{
//	public:
//		connection(ns::client::connectionManager* manager);
//		~connection();
//		void setAgentID(uint64 connid); //{ if (m_agent_id != 0)abort(); m_agent_id = connid; }
//		void setServerID(uint64 connid);// { if (m_server_id != 0)abort(); m_server_id = connid; }
//		uint64 agentID() { return m_agent_id; }
//		uint64 serverID() { return m_server_id; }
//		ns::client::AgentStatus& agentStatus() { return m_agent_status; }
//		ConnectStatus& agentConnectStatus() { return m_agent_connect_status; }
//		uint32 reconnectCount() { return m_reconnect_count; }
//		void resetReconnectCount() { m_reconnect_count = 0; }
//		// 连接集群
//		bool connectAgent();
//		// 终端断开连接
//		void disConnectServer(bool inEvent);
//		// 集群断开连接
//		void disConnectAgent(bool inEvent);
//		// 集群连接
//		void ClusterConnect();
//		// 集群消息
//		void HandleClusterMsg(const char* data,uint32 len);
//		// 网关消息
//		void HandleGatewayMsg(const char* data, uint32 len);
//		// 客户端消息
//		void HandleClientMsg(const char* data,uint32 len);
//		friend class ns::client::connectionManager;
//	private:
//		void checkFree();
//	private:
//		uint64 m_agent_id = 0;
//		uint64 m_server_id = 0;
//
//		uint64 _m_agent_id = 0;
//		uint64 _m_server_id = 0;
//
//		ns::client::connectionManager* m_manager = nullptr;
//		// 代理状态
//		AgentStatus m_agent_status = NotAS;
//		// 代理连接状态
//		ConnectStatus m_agent_connect_status = DisConnect;
//		// 网关源连接ID
//		uint64 m_src_connid = 0;
//		// 代理待发送数据
//		ylib::queue<ylib::buffer> m_agent_wait_send;
//		// 重连次数
//		uint32 m_reconnect_count = 0;
//
//	};
//}
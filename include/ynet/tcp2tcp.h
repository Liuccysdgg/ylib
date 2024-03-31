#pragma once
#include <string>
#include "ybase/define.h"
#include "ybase/buffer.h"
#include "ybase/error.h"
#include "yutil/map.hpp"

namespace ylib::network::tcp 
{
	class agent;
	class server;
	class tcp2tcp :public ylib::error_base
	{
	public:
		tcp2tcp();
		~tcp2tcp(); 
		bool start(const std::string& listen_address,ushort listen_port, bool agent_async_connect = false/*是否异步*/, ylib::receive_model agent_model = PUSH_DEFAULT, ylib::receive_model server_model = PUSH_DEFAULT);
		void stop();
		network::tcp::server* server() { return m_server; }
		network::tcp::agent* agent() { return m_agent; }
	private:
		// TCP服务
		network::tcp::server* m_server = nullptr;
		// TCP代理
		network::tcp::agent* m_agent = nullptr;
	};
}
#pragma once
#include "http_define.h"
#if USE_NET_HTTP_WEBSITE
#include <map>
#include <string>
#include <vector>
#include "http_interface.h"
#include "yutil/counter.hpp"
#include "yutil/cache.h"
namespace ylib
{
    namespace network
    {
        namespace http
        {
            class server;
            struct session_config
            {
                session_config()
                {
                    update_sec = 0;
                    timeout = 0;
                }
                // 更新时间
                uint64 update_sec;
                // 过期时间
                uint32 timeout;
                // 数据
                ylib::json data;
            };
            class session_mgr;
            class session :public ylib::json
            {
            public:
                session();
                ~session();
                void init(const std::string& session_name);
                void destory();
                std::string session_id();
                void update();
                friend class session_mgr;
                friend class request;
            private:
                std::string m_session_name;
                session_mgr* m_mgr;
            };
            class session_mgr :public ylib::error_base, public network::http::interface_
            {
            public:
                session_mgr();
                ~session_mgr();
                bool start(const std::string& cache_path, uint32 timeout_sec);
                void close();
                std::string create(int32 timeout_sec);
                bool add(const std::string& session_name, int32 timeout_sec);
                bool exist(const std::string& session_name);
                void set(const std::string& session_name, http::session& session);
                void del(const std::string& session_name);
                bool get(const std::string& session_name, http::session& session);
                void update(const std::string& session_name, int32 timeout_sec);
                void clear();
                ylib::cache* cache();
                friend class session;
            private:
                ylib::counter<size_t> m_index;
                ylib::cache* m_cache;
                uint32 m_timeout_sec;
            };
        }
    }
}



#endif

#pragma once
#include "http_define.h"
#if USE_NET_HTTP_UTIL
#include <functional>
#include <map>
#include <regex>
#include "http_interface.h"
#include "yutil/array.hpp"
namespace ylib
{
    namespace network
    {
        namespace http
        {
            class server;
            class router;
            class session_mgr;
            class host;
            class agent;
            class cache;
            class cdn;
            class website :public ylib::error_base, public network::http::interface_
            {
            public:

            public:
                website();
                ~website();
                /******************************************************************
                 * function：启动
                 * param
                 *      config                  ：           配置项
                 * return：
                 *      失败可通过 last_error() 返回错误信息。
                 ******************************************************************/
                bool start(const ylib::json& config);
                /******************************************************************
                 * function：关闭
                 ******************************************************************/
                void close();

                network::http::router* router();
                inline network::http::cache* cache() { return m_cache; }
                inline network::http::cdn* cdn() { return m_cdn; }

                network::http::session_mgr* session();
                bool host(const std::string& host);
                ylib::nolock_array<network::http::proxy*>* proxy();
                const network::http::website_info* info();
                inline const std::string& name() { return m_name; }
                /*                inline network::http::agent* agent() { return m_agent; }*/
            private:
                // 文件缓存
                network::http::cache* m_cache;
                // SESSION缓存
                network::http::session_mgr* m_session;
                // router路由 服务
                network::http::router* m_router;
                // CDN服务
                network::http::cdn* m_cdn;
                // 配置信息
                struct website_info m_info;
                // HOST
                std::vector<network::http::host*> m_hosts;
                // HTTPS
                bool m_https;
                // 反向代理
                ylib::nolock_array<network::http::proxy*> m_proxy;
                // 名称
                std::string m_name;
            };
        }
    }
}

#endif

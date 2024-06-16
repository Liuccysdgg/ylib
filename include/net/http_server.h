#pragma once
#include "http_define.h"
#if USE_NET_HTTP_WEBSITE
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <map>
#include "http_interface.h"
#include "qps.hpp"
namespace ylib
{
    namespace network
    {
        namespace http
        {
            class http_server_lst;
            class host;
            /*************************************************************************
             * class：HTTP服务器
             *************************************************************************/
            class server :public ylib::error_base, public network::http::interface_
            {
            public:
                server();
                ~server();
                bool create(server_config config);
                /******************************************************************
                 * Function：启动
                 * Param：
                 *      ipaddress               ：           监听地址
                 *      port                        ：           监听端口
                 ******************************************************************/
                bool start();
                /******************************************************************
                 * Function：关闭
                 ******************************************************************/
                bool close();
                /******************************************************************
                 * Function：取远程信息
                 * Param：
                 *      connid                   ：           连接ID
                 *      ipaddress                ：           远程地址
                 *      port                     ：           远程端口
                 ******************************************************************/
                bool remote(uint64 connid, std::string& ipaddress, ushort& port);

                /******************************************************************
                 * Function：HP服务指针
                 ******************************************************************/
                void* hpserver();
                /******************************************************************
                 * Function：每秒请求数
                 ******************************************************************/
                network::qps* qps();
                
                /// <summary>
                /// 取配置
                /// </summary>
                /// <returns></returns>
                const server_config& config() { return m_config; }
#if USE_NET_HTTP_AGENT == 1
                inline network::http::agent* agent() { return m_agent; }
#endif
            public:
                bool m_init_ssl = false;
            private:
                friend class http_server_lst;
            private:
                // HP HttpServer 指针
                void* m_server;
                // HP Listener 指针
                network::http::http_server_lst* m_listener;
                // 配置
                server_config m_config;
                // QPS
                network::qps m_qps;
#if USE_NET_HTTP_AGENT == 1
                // 代理指针
                network::http::agent* m_agent;
#endif
            };
        }
    }
}




#endif

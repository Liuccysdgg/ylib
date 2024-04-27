#pragma once
#include "http_define.h"
#if USE_NET_HTTP_WEBSITE
#include <functional>
#include <vector>
#include "yutil/map.hpp"
#include "yutil/json.h"


namespace ylib
{
    namespace network
    {
        namespace http
        {
            /*绑定域名*/
            struct domain_info
            {
                domain_info() {
                    https = false;
                }
                // 是否HTTPS
                bool https;
                // 域名
                std::string domain;
            };

            class server;
            class router;
            class session_mgr;
            class website;

            /***************************************************************
             * Class：控制中心
             ***************************************************************/
            class center :public ylib::error_base
            {
            public:

                center();
                ~center();
                /******************************************************************
                 * function：创建
                 * param
                 *      config                  ：           配置项
                 * return：
                 *      失败可通过 last_error() 返回错误信息。
                 ******************************************************************/
                bool create(const ylib::json& config);

                bool start();

                /******************************************************************
                 * function：关闭
                 ******************************************************************/
                void close();

                /******************************************************************
                 * Function：读配置
                 * Param
                 *      name                      ：           名称
                 * Return
                 *      配置内容
                 ******************************************************************/
                std::string read_config(const std::string& name);
                inline const ylib::json& config() { return m_json_config; }
                network::http::server* server(ushort port);

                network::http::website* website(const std::string& host);
                network::http::website* website_byname(const std::string& name);
            private:
                //所有监听端口
                std::vector<ushort> listen_ports();
                //端口是否需要SSL
                bool port_have_ssl(ushort port);
            private:
                // HTTP 服务
                std::vector<network::http::server*> m_server;
                // 站点
                std::vector<network::http::website*> m_website;
                // 配置缓存
                ylib::map<std::string, std::string> m_config;
                // JSON配置
                ylib::json m_json_config;
            public:
                uint64 m_temp[10];

            };
        }
    }
}

#endif

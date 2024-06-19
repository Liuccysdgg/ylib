#pragma once
#include "http_define.h"
#if USE_NET_HTTP_WEBSITE
#include "http_server.h"
#include "http_interface.h"
#include "util/strutils.h"
#include "util/time.h"

namespace ylib
{
    namespace network
    {
        namespace http
        {
            class server;
            class response;
            class request;
            class website;
            /// <summary>
            /// 请求包
            /// </summary>
            class reqpack:public network::http::interface_
            {
            public:
                /// <summary>
                /// 构造函数
                /// </summary>
                /// <param name="url"></param>
                /// <param name="host"></param>
                /// <param name="data"></param>
                /// <param name="connid"></param>
                /// <param name="server"></param>
                /// <param name="website"></param>
                reqpack(const std::string& url, const std::string& host, const ylib::buffer& data, uint64 connid, network::http::server* server,network::http::website* website);
                ~reqpack();
                /// <summary>
                /// 请求对象
                /// </summary>
                /// <returns></returns>
                const std::shared_ptr<network::http::request>& request();
                /// <summary>
                /// 回复对象
                /// </summary>
                /// <returns></returns>
                const std::shared_ptr<network::http::response>& response();
                /// <summary>
                /// httpserver
                /// </summary>
                /// <returns></returns>
                network::http::server* server() { return m_server; }
                /// <summary>
                /// 连接ID
                /// </summary>
                /// <returns></returns>
                uint64 connid() { return m_connid; }
                /// <summary>
                /// 请求完整URL
                /// </summary>
                /// <returns></returns>
                std::string url() { return m_url; }
                /// <summary>
                /// 请求路径
                /// </summary>
                /// <returns></returns>
                std::string filepath() { return m_filepath; }
                /// <summary>
                /// 请求主机
                /// </summary>
                /// <returns></returns>
                std::string host() { return m_host; }
                /// <summary>
                /// 请求数据
                /// </summary>
                /// <returns></returns>
                ylib::buffer& body() { return m_data; }
                /// <summary>
                /// 附加数据
                /// </summary>
                /// <returns></returns>
                ylib::json& extra() { return m_extra; }
            private:
                // 请求
                std::shared_ptr<network::http::request> m_request;
                // 回复
                std::shared_ptr<network::http::response> m_response;
                // httpserver
                network::http::server* m_server = nullptr;
                // 请求地址
                std::string m_url;
                // 请求路径
                std::string m_filepath;
                // 请求主机
                std::string m_host;
                // 请求数据
                ylib::buffer m_data;
                // 请求ID
                uint64 m_connid = 0;
                // 附加数据
                ylib::json m_extra;
            };
        }
    }
}
#endif

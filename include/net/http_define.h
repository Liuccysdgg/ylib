#pragma once
#include <vector>
#include <map>
#include <regex>
#include "define.h"

#include "util/file.h"
#include "base/buffer.h"
#include "base/environment.h"

#define POINT_QUEUE_REQUEST_CLEAR_MAX 1000
#define POINT_QUEUE_REQUEST_CLEAR_SEC 60

#define POINT_QUEUE_RESPONSE_CLEAR_MAX 1000
#define POINT_QUEUE_RESPONSE_CLEAR_SEC 60

#define POINT_QUEUE_ROUTER_CLEAR_MAX 1000
#define POINT_QUEUE_ROUTER_CLEAR_SEC 60

#define POINT_QUEUE_REQPACK_CLEAR_MAX 1000
#define POINT_QUEUE_REQPACK_CLEAR_SEC 60

// http_agent 调试日志打印
#define HTTP_AGENT_DEBUG_PRINT 0
// http_agent 生产日志打印
#define HTTP_AGENT_PRINT 0
// http_server 调试日志打印
#define HTTP_SERVER_DEBUG_PRINT 0
// http_server 生产日志打印
#define HTTP_SERVER_PRINT 0
// http_router 生产日志打印
#define HTTP_ROUTER_PRINT 0
// http_interceptor 生产日志打印
#define HTTP_INTERCEPTOR_PRINT 0
// http lua 引擎
//#define HTTP_LUA_ENGINE 0
namespace ylib
{
    namespace network
    {
        namespace http
        {
            /// <summary>
            /// 服务端缓存
            /// </summary>
            struct server_cache_config {
                // 缓存保存目录
                std::string dirpath;
                // 开启
                bool enable = false;
                // 最小过期时间
                uint32 timeout_min;
                // 支持扩展名
                std::vector<std::string> exts;
            };
            /// <summary>
            /// SSL验证类型
            /// </summary>
            enum ssl_verify_type
            {
                // 完全忽略验证证书的结果。当握手必须完成的话，就选中这个选项。其实真正有证书的人很少，尤其是在中国，那么如果 SSL 运用于一些免费的服务，比如 EMAIL 的时候，SERVER 端最好采用这个模式。
                SVT_VERIFY_NONE = 0x00,
                // 希望验证对方的证书。这个是最一般的模式。对 CLIENT 来说，如果设置了这样的模式，验证SERVER的证书出了任何错误，SSL 握手都告吹。对 SERVER 来说,如果设置了这样的模式，CLIENT 倒不一定要把自己的证书交出去。如果 CLIENT 没有交出证书，SERVER 自己决定下一步怎么做。
                SVT_VERIFY_PEER = 0x01,
                // 这是 SERVER 使用的一种模式，在这种模式下， SERVER 会向 CLIENT 要证书。如果 CLIENT 不给，SSL 握手告吹
                SVT_VERIFY_FAIL_IF_NO_PEER_CERT = 0x02,
                // 这是仅能使用在 SSL SESSION RENEGOTIATION 阶段的一种方式。如果不是用这个模式的话,那么在 RENEGOTIATION 的时候，CLIENT 都要把自己的证书送给 SERVER，然后做一番分析。这个过程很消耗 CPU 时间的，而这个模式则不需要 CLIENT 在 RENEGOTIATION 的时候重复送自己的证书了。
                SVT_VERIFY_CLIENT_ONCE = 0x03

            };
            /// <summary>
            /// 主机域名配置
            /// </summary>
            struct host_config {
                // 域名
                std::string domain;
                // 端口
                ushort port = 0;
                // 开启SSL
                bool ssl = false;
            };
            /// <summary>
            /// CDN配置
            /// </summary>
            struct cdn_config {
                struct node_config {
                    std::string host;
                    std::string key;
                    std::string mang_domain;
                   
                };
                bool manager = false;
                bool enable = false;
                std::vector<node_config> node;
                uint64 max_band = 0;
                std::string key;
            };
            /// <summary>
            /// SSL配置
            /// </summary>
            struct ssl_config {
                bool enable = false;
                // 验证类型
                ssl_verify_type type = SVT_VERIFY_PEER;
                std::string pem_cert;
                std::string pem_key;
                std::string pem_ca;
                std::string pem_password;
            };
            /// <summary>
            /// 线程池配置
            /// </summary>
            struct threadpool_config {
                // 线程数量
                uint32 size = 0;
                // 最大队列数量
                uint32 queuemax = 0;
            };
            /// <summary>
            /// 路由配置
            /// </summary>
            struct router_config {
                // 线程池
                threadpool_config threadpool;
            };
            /// <summary>
            /// 会话配置
            /// </summary>
            struct session_config {
                // 超时时间
                uint32 timeout_sec = 0;
                // 保存路径
                std::string dirpath;
            };
            /// <summary>
            /// 代理配置
            /// </summary>
            struct proxy_config {
                std::string src;
                std::string dst;
                std::string remote;
                std::string host;
                std::map<std::string, std::string> header_request;
                std::map<std::string, std::string> header_response;
            };
            /// <summary>
            /// 网站配置
            /// </summary>
            struct website_config {
                // 名称
                std::string name;
                // 域名列表
                std::vector<host_config> host;
                // 路由
                router_config router;
                // 会话
                session_config session;
                // 本地缓存
                server_cache_config cache;
                // CDN
                cdn_config cdn;
                // 代理
                std::vector<proxy_config> proxy;
               
                // GZIP
                bool gzip = false;
            };
            /// <summary>
            /// HTTP服务配置
            /// </summary>
            struct server_config {
                // 最大上传大小限制
                uint64 max_upload_size = 0;
                // Https
                bool https = false;
                // 端口
                ushort port = 0;
            };
            /// <summary>
            /// 启动信息
            /// </summary>
            struct start_config {
                // 网站
                std::vector<website_config> website;
                // 证书
                std::map<std::string, ssl_config> cert;
                // 最大上传大小限制
                uint64 max_upload_size = 0;
            };
            
            class agent;
            class website;
            // 临时接收
            struct temp_recv
            {
                temp_recv()
                {
                    agent_connid = 0;
                    agent_ssl = false;
                }
                void clear()
                {
                    agent_connid = 0;
                    agent_ssl = false;
                    url.clear();
                    ipaddress_port.clear();
                    host.clear();
                }
                // 接收数据
                ylib::buffer data;
                // 代理ID
                uint64 agent_connid;
                // 代理SSL
                bool agent_ssl;
                // URL
                std::string url;
                // 代理连接IP及端口
                std::string ipaddress_port;
                // Host
                std::string host;
                // 缓存路径
                ylib::file_io cache_file;
            };
            // 代理
            struct proxy
            {
                proxy()
                {
                    remote_port = 0;
                    ssl = false;
                }
                // 拦截地址正则
                std::regex src_express;
                // 拦截地址字符串
                std::string src_str;
                // 目标地址
                std::string dst;
                // 请求URL
                std::string remote_url;
                // 请求地址
                std::string remote_ipaddress;
                // 请求端口
                ushort remote_port;
                // 主机
                std::string host;
                // 附加协议头
                std::map<std::string, std::string> request_headers;
                std::map<std::string, std::string> response_headers;
                // SSL
                bool ssl;

            };
            // httpserver代理后连接附加数据
            struct httpserver_proxy_extra
            {
                httpserver_proxy_extra()
                {
                    agent = nullptr;
                    connid = 0;
                    index = 45854;
                }
                void* agent;
                uint64 connid;
                uint64 index;
            };

            struct multipart {
                std::map<std::string,std::string> param;
                size_t offset = 0;
                size_t length = 0;
            };
        }

    }
}

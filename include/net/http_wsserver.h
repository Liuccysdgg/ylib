#pragma once
#include "http_define.h"
#if USE_NET_HTTP_WS == 1
#include <string_view>
#include <functional>
#include "base/define.h"
#include "base/error.h"
class wsserver_lst;
namespace ylib
{
    namespace network
    {
        namespace http
        {
           
            /// <summary>
            /// WebSocket服务器
            /// </summary>
            class wsserver:public ylib::error_base {
            public:
                wsserver();
                ~wsserver();
                /// <summary>
                /// 启动
                /// </summary>
                /// <param name="port"></param>
                /// <param name="ssl"></param>
                /// <param name="ssl_key"></param>
                /// <param name="ssl_pem"></param>
                /// <returns></returns>
                bool start(ushort port, bool ssl = false,const std::string& ssl_key = "", const std::string& ssl_pem = "");
                /// <summary>
                /// 停止
                /// </summary>
                void stop();
                /// <summary>
                /// 发送
                /// </summary>
                bool send(int64 conn, std::string_view value);
                /// @brief 断开连接
                /// @param conn 
                void disconn(int64 conn);

                void on_accept(std::function<bool(int64)> callback) { m_callback_accept = callback; }
                void on_recv(std::function<bool(int64,const char* data, int len)> callback) { m_callback_recv = callback; }
                void on_close(std::function<void(int64)> callback) { m_callback_close = callback; }

                friend class wsserver_lst;
            private:
                // 端口
                ushort m_port = 0;
                // 是否为SSL
                bool m_ssl = false;
                // SSL-KEY
                std::string m_ssl_key;
                // SSL-PEM
                std::string m_ssl_pem;
                // 监听器
                wsserver_lst* m_listener = nullptr;
                // HPServer
                void* m_server = nullptr;
            public:
                std::function<bool(int64)> m_callback_accept;
                std::function<bool(int64,const char* data, int len)> m_callback_recv;
                std::function<void(int64)> m_callback_close;
            };
        }
    }
}




#endif

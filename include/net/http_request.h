#pragma once
#include "http_define.h"
#if USE_NET_HTTP_WEBSITE
#include <string>
#include <vector>
#include "http_session.h"
#include "http_interface.h"
namespace ylib
{
    namespace network
    {
        namespace http
        {
            class reqpack;
            class server;
            /// <summary>
            /// HTTP请求
            /// </summary>
            class request :public network::http::interface_
            {
            public: 
                request(network::http::reqpack* reqpack);
                ~request(); 

                /// <summary>
                /// 取请求头
                /// </summary>
                /// <param name="name"></param>
                /// <param name="value"></param>
                /// <returns></returns>
                bool header(const std::string& name, std::string& value);
                /// <summary>
                /// 请求动作
                /// </summary>
                /// <returns></returns>
                std::string method();
                /// <summary>
                /// 取请求路径
                /// </summary>
                /// <returns></returns>
                std::string filepath();
                /// <summary>
                /// 取请求主机
                /// </summary>
                /// <returns></returns>
                std::string host();
                /// <summary>
                /// session
                /// </summary>
                /// <param name="session_id"></param>
                /// <returns></returns>
                network::http::session& session(const std::string& session_id);
                network::http::reqpack* reqpack();
                /// <summary>
                /// 远程信息
                /// </summary>
                /// <returns></returns>
                ylib::AddressPort remote();
                /// <summary>
                /// 取请求参数
                /// </summary>
                /// <param name="name"></param>
                /// <param name="value"></param>
                /// <returns></returns>
                bool get_url_param(const std::string& name,std::string& value);
                bool get_body_param(const std::string& name, std::string& value);
                std::shared_ptr<std::map<std::string, std::string>>& url_param() { return m_url_param; }
                std::shared_ptr<std::map<std::string, std::string>>& body_param() { return m_body_param; }

                /// <summary>
                /// 表单数据
                /// </summary>
                /// <returns></returns>
                const std::shared_ptr<std::vector<network::http::multipart>>& multipart();
                /// <summary>
                /// 是否为请求类型
                /// </summary>
                /// <returns></returns>
                bool content_type(std::string type_name);

                /// <summary>
                /// Body
                /// </summary>
                /// <returns></returns>
                ylib::buffer& body();

            private:

                // reqpack
                network::http::reqpack* m_reqpack = nullptr;
                // session
                network::http::session m_session;
                // 请求动作
                std::string m_method;
            private:
                // 请求参数[URL]
                std::shared_ptr<std::map<std::string, std::string>> m_url_param;
                // 请求参数[BODY]
                std::shared_ptr<std::map<std::string, std::string>> m_body_param;
                // FORM数据
                std::shared_ptr<std::vector<network::http::multipart>> m_form;

            };
        }
    }
}
#endif

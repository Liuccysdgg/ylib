#pragma once
#include "http_define.h"
#if USE_NET_HTTP_WEBSITE
#include <functional>
#include "http_interface.h"
#include <map>
namespace ylib::network::http
{
    class server;
    /******************************************************
     * class：域名器
     ******************************************************/
    class ssl :public ylib::error_base, public network::http::interface_
    {
    public:
        /**********************************************************************************************
         * function：构造函数
         * param
         *          server                                      ：                                   HTTP服务
         *          verify_type                               ：                                  验证方式
         *          pem_cert                                 ：                                   证书内容
         *          pem_key                                  ：                                   证书密钥
         *          pem_ca                                    ：                                   CA证书
         *          password                                 ：                                   证书密码，没有留空
         **********************************************************************************************/
        ssl(network::http::server* server,ssl_verify_type verify_type,const std::string& pem_cert,const std::string&pem_key, const std::string& pem_ca = "", const std::string& password = "");
        ~ssl();
        /**********************************************************************************************
         * function：注册证书
         **********************************************************************************************/
        bool regist();
        /**********************************************************************************************
         * function：绑定
         **********************************************************************************************/
        bool bind(const std::string& host);
    private:
        ssl_verify_type m_verify_type;
        std::string m_pem_cert;
        std::string m_pem_key;
        std::string m_pem_ca;
        std::string m_pem_password;
        network::http::server* m_server;
        int32 m_index;
    };
}
#endif

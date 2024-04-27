#pragma once
#include "define.h"
#if USE_NET_HTTP_WEBSITE
#include <regex>
#include <functional>
#include "ynet/http_interface.h"
#include "yutil/array.hpp"

namespace ylib
{
    namespace network
    {
        namespace http
        {
            class reqpack;

            struct interceptor_info {
                std::regex express;
                std::string express_string;
                std::function<bool(network::http::reqpack* rp)> callback;
            };
            /******************************************************
             * class：拦截器
             ******************************************************/
            class interceptor :public ylib::error_base, public network::http::interface_
            {
            public:
                interceptor();
                ~interceptor();
                size_t add(const std::string& regex_express, std::function<bool(network::http::reqpack* rp)> callback);
                void remove(size_t index);
                bool trigger(const std::string& url, network::http::reqpack* rp);
            private:
                ylib::nolock_array<interceptor_info*> m_array;
            };
        }
    }
}

#endif

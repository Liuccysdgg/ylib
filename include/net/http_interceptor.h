﻿#pragma once
#include "define.h"
#if USE_NET_HTTP_WEBSITE
#include <regex>
#include <functional>
#include "net/http_interface.h"
#include "util/array.hpp"
#include <shared_mutex>

namespace ylib
{
    namespace network
    {
        namespace http
        {
            class reqpack;

            struct interceptor_info {
                std::regex regex;
                std::string pattern;
                std::string extra;
                std::function<bool(network::http::reqpack* rp, const std::string& pattern,const std::string& extra)> callback;
            };
            /******************************************************
             * class：拦截器
             ******************************************************/
            class interceptor :public ylib::error_base, public network::http::interface_
            {
            public:
                interceptor();
                ~interceptor();
                bool add(const std::string& pattern,const std::string& extra,std::function<bool(network::http::reqpack* rp, const std::string& pattern, const std::string& extra)> callback);
                bool remove(const std::string& pattern);
                bool exist(const std::string& pattern);
                bool trigger(const std::string& url, network::http::reqpack* rp);
                void clear();
            private:
                std::vector<interceptor_info> m_list;
                std::shared_mutex m_rw_mutex;
            };
        }
    }
}

#endif

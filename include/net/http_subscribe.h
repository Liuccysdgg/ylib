#pragma once
#include "define.h"
#if USE_NET_HTTP_WEBSITE
#include <regex>
#include <functional>
#include <shared_mutex>
#include "http_interface.h"

namespace ylib
{
    namespace network
    {
        namespace http
        {
            class request;
            class response;
            class reqpack;
            struct subscribe_info {
                std::regex regex;
                std::string pattern;
                std::string extra;
                std::function<void(network::http::request* request,network::http::response* response,const std::string& pattern,const std::string& extra)> callback;
            };
            /******************************************************
             * class：订阅器
             ******************************************************/
            class subscribe :public ylib::error_base, public network::http::interface_
            {
            public:
                subscribe();
                ~subscribe();
                bool add(const std::string& pattern,const std::string& extra,std::function<void(network::http::request* request, network::http::response* response, const std::string& pattern, const std::string& extra)> callback);
                bool remove(const std::string& pattern);
                bool exist(const std::string& pattern);
                bool trigger(const std::string& url, network::http::reqpack* rp);
                void clear();
            private:
                std::vector<subscribe_info> m_list;
                std::shared_mutex m_rw_mutex;
            };
        }
    }
}

#endif

#pragma once
#include "http_define.h"
#include "http_reqpack.h"
#include "http_interface.h"
#include "yutil/map.hpp"
namespace ylib
{
    namespace network
    {
        namespace http
        {
            class cache :public network::http::interface_, public ylib::error_base
            {
            public:
                cache();
                ~cache();
                bool start(const ylib::json& config);
                void stop();
                bool have_send(network::http::reqpack* rp);
                bool enable();
                std::string make_key(const std::string& filepath);

                bool find_ext(const std::string& filepath);

                std::string make_cache_filepath(const std::string& filepath);
            private:
                bool m_enable;
                std::string m_filepath;

                std::vector<std::string> m_exts;

                timestamp m_timeout_min;

                ylib::map<std::string, std::string> m_keys;
            };
        }
    }
}

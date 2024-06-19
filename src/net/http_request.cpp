/*Software License

Copyright(C) 2024[liuyingjie]
License Terms
Usage Rights

Any individual or entity is free to use, copy, and distribute the binary form of this software without modification to the source code, without the need to disclose the source code.
If the source code is modified, the modifications must be open - sourced under the same license.This means that the modifications must be disclosed and accompanied by a copy of this license.
Future Versions Updates
From this version onwards, all future releases will be governed by the terms of the latest version of the license.This license will automatically be nullified and replaced by the new version.
Users must comply with the terms of the new license issued in future releases.
Liability and Disclaimer
This software is provided “as is”, without any express or implied warranties, including but not limited to the warranties of merchantability, fitness for a particular purpose, and non - infringement.In no event shall the author or copyright holder be liable for any claims, damages, or other liabilities, whether in an action of contract, tort, or otherwise, arising from, out of, or in connection with the software or the use or other dealings in the software.
Contact Information
If you have any questions, please contact us: 1585346868@qq.com Or visit our website fwlua.com.
*/


#include "net/http_request.h"
#if USE_NET_HTTP_WEBSITE
#include "util/strutils.h"
#include "util/codec.h"
#include "net/http_session.h"
#include "net/http_server.h"
#include "net/http_reqpack.h"
#include "net/http_center.h"
#include "net/http_website.h"
#include "net/util.h"
#include "HPSocket/HPSocket.h"
#define HPSERVER ((IHttpServer*)m_reqpack->server()->hpserver())

ylib::network::http::request::request(network::http::reqpack* reqpack):m_reqpack(reqpack)
{

}
ylib::network::http::request::~request()
{
} 

bool ylib::network::http::request::header(const std::string &name, std::string &value)
{

    LPCSTR lpszValue = nullptr;
    if (HPSERVER->GetHeader((CONNID)m_reqpack->connid(), name.c_str(), &lpszValue) == false)
        return false;
    value = strutils::F(lpszValue);
    return true;
}

std::string ylib::network::http::request::method()
{
    if (m_method.empty())
        m_method = strutils::change_case(strutils::F(HPSERVER->GetMethod((CONNID)m_reqpack->connid())),true);
    return m_method;
}

std::string ylib::network::http::request::filepath()
{
    return m_reqpack->filepath();
}

std::string ylib::network::http::request::host()
{
    return m_reqpack->host();
}
network::http::session& ylib::network::http::request::session(const std::string& session_id)
{
    if (session_id.empty())
        return m_session;
    m_session.init(website(), session_id);
    return m_session;
}
network::http::reqpack* ylib::network::http::request::reqpack()
{
    return m_reqpack;
}
ylib::AddressPort ylib::network::http::request::remote()
{
    ylib::AddressPort ap;
    m_reqpack->server()->remote(m_reqpack->connid(), ap.address, ap.port);
    return ap;
}
bool ylib::network::http::request::get_url_param(const std::string& name, std::string& value)
{
    if (m_url_param == nullptr)
    {
        m_url_param = std::make_shared<std::map<std::string, std::string>>();
        // 解析URL参数
        std::string url_query = strutils::F(HPSERVER->GetUrlField((CONNID)m_reqpack->connid(), HUF_QUERY));
        auto results = strutils::split(url_query, "&");
        for (size_t i = 0; i < results.size(); i++)
        {
            std::string key;
            std::string value;
            auto arr = strutils::split(results[i], '=');
            if (arr.size() > 0)
                key = arr[0];
            if (arr.size() > 1)
                value = arr[1];
            m_url_param->emplace(key, value);
        }
    }
    // 查找
    auto iter = m_url_param->find(name);
    if (iter == m_url_param->end())
        return false;
    value = iter->second;
    return true;
}
bool ylib::network::http::request::get_body_param(const std::string& name, std::string& value)
{
    if (m_body_param == nullptr)
    {
        m_body_param = std::make_shared<std::map<std::string, std::string>>();
        if (content_type("application/x-www-form-urlencoded"))
        {
            auto results = strutils::split(m_reqpack->body(), "&");
            for (size_t i = 0; i < results.size(); i++)
            {
                std::string key;
                std::string value;
                auto arr = strutils::split(results[i], '=');
                if (arr.size() > 0)
                    key = arr[0];
                if (arr.size() > 1)
                    value = arr[1];
                m_body_param->emplace(key, value);
            }
        }
        else if (content_type("application/json"))
        {
            auto data_json = ylib::json::from(m_reqpack->body());
            auto keys = data_json.keys();
            for (size_t i = 0; i < keys.size(); i++)
            {
                std::string strvalue = data_json[keys[i]].to<std::string>();
                double dobvalue = data_json[keys[i]].to<double>();

                if (strvalue.empty())
                {
                    if (dobvalue == 0.0)
                        continue;
                    else
                        m_body_param->emplace(keys[i], std::to_string(dobvalue));
                }
                else
                    m_body_param->emplace(keys[i],strvalue);
            }
        }
    }

    // 查找
    auto iter = m_body_param->find(name);
    if (iter == m_body_param->end())
        return false;
    value = iter->second;
    return true;
}
const std::shared_ptr<std::vector<network::http::multipart>>& ylib::network::http::request::multipart()
{
    if (m_form == nullptr)
    {
        m_form = std::make_shared<std::vector<network::http::multipart>>();
        if (content_type("multipart/form-data"))
        {
            std::string header_value;
            if (header("Content-Type", header_value))
            {
                auto arr = strutils::split(header_value, ';');
                for (size_t i = 0; i < arr.size(); i++)
                {
                    if (arr[i].find("boundary") != -1)
                    {
                        auto kv = strutils::split(strutils::trim(arr[i], { ' ' }), "=");
                        if (kv.size() == 2)
                        {
                            network::parse_multipart_form_data(m_reqpack->body(), kv[1], *m_form.get());
                        }
                        break;
                    }
                }
                
            }
            
        }
    }
    return m_form;
}
bool  ylib::network::http::request::content_type(std::string type_name)
{
    type_name = strutils::change_case(type_name, false);
    std::string header_value;
    if (header("Content-Type", header_value))
    {
        auto results = strutils::split(header_value, ';');
        for (size_t i = 0; i < results.size(); i++)
        {
            if (strutils::change_case(results[i], false).find(type_name) != -1)
                return true;
        }
    }
    return false;
}
ylib::buffer& ylib::network::http::request::body()
{
    return m_reqpack->body();
}

#endif

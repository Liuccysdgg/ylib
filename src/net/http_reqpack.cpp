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

#include "net/http_reqpack.h"
#if USE_NET_HTTP_WEBSITE
#include "HPSocket/HPSocket.h"
#include "net/http_server.h"
#include "net/http_request.h"
#include "net/http_response.h"
#include "util/time.h"
#define HPSERVER 
ylib::network::http::reqpack::reqpack()
{
   
}

ylib::network::http::reqpack::~reqpack()
{
    clear();
}

void ylib::network::http::reqpack::init(const std::string& url,const std::string& host,const ylib::buffer& data, uint64 connid, network::http::server* server)
{
    m_host = host;
    m_url = url;
    m_method = network::http::ALL;
    m_request = nullptr;
    m_response = nullptr;
    m_data = data;
    m_server = server;
    m_connid = connid;
    m_website = nullptr;
    m_begin_msec = time::now_msec();
}

void ylib::network::http::reqpack::clear()
{
    this->m_data.clear();
    if (this->m_request != nullptr)
        delete this->m_request;
    if (this->m_response != nullptr)
        delete this->m_response;

    m_host.clear();
    m_method = network::http::OTHER;
    m_begin_msec = 0;
    m_filepath.clear();
    m_server = nullptr;
    m_referrer.clear();
    m_connid = 0;
    m_website = nullptr;
    m_url.clear();
    m_remote_ipaddress.clear();
    m_request = nullptr;
    m_response = nullptr;
}

network::http::request *network::http::reqpack::request()
{
    if(m_request == nullptr){
        m_request = new network::http::request;
        m_request->m_reqpack = this;
        m_request->center(m_server->center());
        m_request->website(website());
    }
    return m_request;
}

network::http::response *network::http::reqpack::response()
{
    if(m_response == nullptr){
        m_response = new network::http::response;
        m_response->center(m_server->center());
        m_response->website(website());
        m_response->init(this);
    }
    return m_response;
}
network::http::method network::http::reqpack::method()
{
    if (m_method == network::http::ALL)
    {
        std::string method = strutils::F(((IHttpServer*)m_server->hpserver())->GetMethod((CONNID)m_connid));
        if (method == "GET")
            m_method = network::http::GET;
        else if (method == "POST")
            m_method = network::http::POST;
        else
            m_method = network::http::OTHER;
    }
    return m_method;
}
const std::string& network::http::reqpack::filepath()
{
    if (m_filepath.empty())
    {

        m_filepath = strutils::F(((IHttpServer*)m_server->hpserver())->GetUrlField((CONNID)m_connid, HUF_PATH));
        // 防止访问上级目录
        m_filepath = strutils::remove(m_filepath, "..");
    }
    return m_filepath;
}
void network::http::reqpack::filepath(const std::string &path)
{
    m_filepath = path;
}
#endif

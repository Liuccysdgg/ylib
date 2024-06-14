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

#include "net/http_client_cache.h"
#if USE_NET_HTTP_CLIENT
#include "net/http_client_plus.h"
#include "util/codec.h"
#include "util/file.h"
ylib::network::http::client_cache::client_cache()
{
    m_open = false;
}

ylib::network::http::client_cache::~client_cache()
{
}

void ylib::network::http::client_cache::open(const std::string& dirpath)
{
    m_dirpath = dirpath;
    m_open = true;
    ylib::file::create_dir(m_dirpath,true);
}
#ifdef LIB_CURL
bool ylib::network::http::client_cache::read(ylib::network::http::client* client,ylib::buffer& cache)
{
    if (client->status() != 304)
        return false;
    std::string cache_file_path = m_dirpath + "\\" + codec::md5(client->m_url);
    std::string cache_date_path = m_dirpath + "\\" + codec::md5(client->m_url) + "_lastmodified.txt";
    
    if (ylib::file::exist(cache_date_path) == false || ylib::file::exist(cache_file_path) == false)
        return false;

    ylib::buffer cache_data;
    ylib::file::read(cache_date_path, cache_data);
    if (cache_data.to_string() != client->headers_response()->get("Last-Modified").to_string())
        return false;
    return ylib::file::read(cache_file_path, cache);
}
#endif
bool ylib::network::http::client_cache::read(ylib::network::http::client_plus* client, ylib::buffer& cache)
{
    if (client->status() != 304)
        return false;
    std::string cache_file_path = m_dirpath + "\\" + codec::md5(client->url());
    std::string cache_date_path = m_dirpath + "\\" + codec::md5(client->url()) + "_lastmodified.txt";

    if (ylib::file::exist(cache_date_path) == false || ylib::file::exist(cache_file_path) == false)
        return false;

    ylib::buffer cache_data = ylib::file::read(cache_date_path);
    if (cache_data.to_string() != client->headers_response().get("Last-Modified").to_string())
        return false;
    return ylib::file::read(cache_file_path, cache);
}
#ifdef LIB_CURL
void ylib::network::http::client_cache::set_header(ylib::network::http::client* client, const std::string& url)
{
    std::string cache_file_path = m_dirpath + "\\" + codec::md5(url);
    std::string cache_date_path = m_dirpath + "\\" + codec::md5(url) + "_lastmodified.txt";

    if (ylib::file::exist(cache_date_path) == false || ylib::file::exist(cache_file_path) == false)
        return ;
    ylib::buffer cache_data;
    ylib::file::read(cache_date_path, cache_data);
    client->header("If-Modified-Since",cache_data.to_string());
}
#endif
void ylib::network::http::client_cache::set_header(ylib::network::http::client_plus* client, const std::string& url)
{
    std::string cache_file_path = m_dirpath + "\\" + codec::md5(url);
    std::string cache_date_path = m_dirpath + "\\" + codec::md5(url) + "_lastmodified.txt";

    if (ylib::file::exist(cache_date_path) == false || ylib::file::exist(cache_file_path) == false)
        return;
    ylib::buffer cache_data;
    ylib::file::read(cache_date_path, cache_data);
    client->headers_request().set("If-Modified-Since", cache_data.to_string());
}

void ylib::network::http::client_cache::close()
{
    m_open = false;
}
#ifdef LIB_CURL
void ylib::network::http::client_cache::write(ylib::network::http::client* client)
{
    auto header = client->headers_response()->get("Cache-Control");
    if (!header.exist("no-cache"))
        return;
    if (client->status() == 304)
        return;
    std::string cache_file_path = m_dirpath + "\\" + codec::md5(client->m_url);
    std::string cache_date_path = m_dirpath + "\\" + codec::md5(client->m_url)+"_lastmodified.txt";

    if (ylib::file::exist(cache_date_path))
    {
        ylib::buffer cache_data;
        ylib::file::read(cache_date_path,cache_data);
        if (cache_data.to_string() == client->headers_response()->get("Last-Modified").to_string())
            return;
    }
    ylib::file::write(cache_date_path, client->headers_response()->get("Last-Modified").to_string());
    ylib::file::write(cache_file_path, client->response());
}
#endif
void ylib::network::http::client_cache::write(ylib::network::http::client_plus* client)
{
    auto header = client->headers_response().get("Cache-Control");
    if (!header.exist("no-cache"))
        return;
    if (client->status() == 304)
        return;
    std::string cache_file_path = m_dirpath + "\\" + codec::md5(client->url());
    std::string cache_date_path = m_dirpath + "\\" + codec::md5(client->url()) + "_lastmodified.txt";

    if (ylib::file::exist(cache_date_path))
    {
        ylib::buffer cache_data;
        ylib::file::read(cache_date_path, cache_data);
        if (cache_data.to_string() == client->headers_response().get("Last-Modified").to_string())
            return;
    }
    ylib::file::write(cache_date_path, client->headers_response().get("Last-Modified").to_string());
    ylib::file::write(cache_file_path, client->response());
}

bool ylib::network::http::client_cache::read(const std::string& url, ylib::buffer& cache)
{
    std::string cache_file_path = m_dirpath + "\\" + codec::md5(url);
    if(ylib::file::exist(cache_file_path) == false)
        return false;
    return ylib::file::read(cache_file_path,cache);
}
#endif

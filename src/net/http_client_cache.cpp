// MIT License﻿
// Copyright(c) 2024 FastWeb - fwlua.com - nianhua﻿
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :﻿
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.﻿
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.﻿
// ## Additional Terms for Commercial Use﻿
// This software is licensed for personal, educational, and non - commercial use.
// For commercial use or use within a company, organization, or institution, a
// separate commercial license is required.To obtain a commercial license,
// please contact
// EMail：1585346868@qq.com
// Mobile：17367918735
// QQ：1585346868

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

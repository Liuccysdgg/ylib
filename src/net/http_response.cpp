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

#include "net/http_response.h"
#if USE_NET_HTTP_WEBSITE
#include "HPSocket/HPSocket.h"
#include "util/file.h"
#include "util/strutils.h"
#include "util/time.h"
#include "util/system.h"
#include "util/codec.h"

#include "net/util.h"
#include "net/http_server.h"
#include "net/http_center.h"
#include "net/http_reqpack.h"
#include "net/http_router.h"
#include "net/http_website.h"
#include <sys/stat.h>
#define HPSERVER ((IHttpServer*)m_reqpack->server()->hpserver())
#ifdef MSVC_2010
#define HEADER_SET(NAME,VALUE) m_headers.insert(std::pair<std::string,std::string>(NAME,VALUE))
#else
#define HEADER_SET(NAME,VALUE) m_headers.emplace(NAME,VALUE)
#endif
ylib::network::http::response::response()
{
    m_reqpack = nullptr;
    m_response = false;
}

ylib::network::http::response::~response()
{
    if(m_response == true)
        return;
    if (!sjson.is_empty())
    {
        send(sjson);
    }
    else
    {
        send((std::string)"The server does not return any data to the browser", 500, "Internal Server Error");
    }
}

void ylib::network::http::response::init(reqpack* rp)
{
    this->m_reqpack = rp;
}
bool ylib::network::http::response::send(const char* buf, size_t buf_len, ushort stateNum, const std::string& stateDesc)
{


    ylib::buffer end_send;
    ylib::buffer view(buf, buf_len);

    char* send_data_p = nullptr;
    size_t send_data_length = 0;
#if 1
    // 发送前回调
    {
        auto sendbefore_callback = m_reqpack->website()->router()->m_callback_sendbefore;
        if (sendbefore_callback != nullptr)
        {

            sendbefore_callback(view,&end_send);
            if (end_send.length() != 0)
                view = end_send;
        }
    }
#endif
    if(m_response == true)
        return false;
    m_response = true;
    //合成THeader
    //Content-Type
    {
        HEADER_SET("Content-Type", "text/html");
        HEADER_SET("Content-Length", std::to_string((uint64)view.length()));
        // 设置默认编码
        {
            std::string value;
            auto iter = m_headers.find("Content-Type");
            if (iter == m_headers.end() && iter->second.empty())
                iter->second = "charset=utf8";
            else
            {
                if (value.find("charset") == -1)
                    iter->second += "; charset=utf8";
            }
        }
    }


    {
        THeader* headers = new THeader[m_headers.size()];
        uint32 idx = 0;
        for_iter(iter, m_headers)
        {
            headers[idx].name = iter->first.c_str();
            headers[idx].value = iter->second.c_str();
            idx++;
        }
        this->m_reqpack->server()->qps()->response(view.length());
        bool result = (bool)HPSERVER->SendResponse((CONNID)m_reqpack->connid(), stateNum, stateDesc.c_str(), headers, (int32)m_headers.size(), (const BYTE*)view.data(),(int)view.length());
        delete[] headers;
        return result;
    }

}

bool ylib::network::http::response::send(const ylib::buffer& value, ushort stateNum, const std::string& stateDesc)
{
    return send(value.data(), value.length(), stateNum, stateDesc);
}

bool ylib::network::http::response::send(const std::string& value, ushort stateNum, const std::string& stateDesc)
{
    //转码为UTF-8
    //std::string utf8str = GbkToUtf8(value.c_str());
    return send(value.c_str(), value.length(), stateNum, stateDesc);
}

bool ylib::network::http::response::send(const ylib::json& json, ushort stateNum, const std::string& stateDesc)
{
    HEADER_SET("Content-Type", "application/json");
    return send(json.to_string(), stateNum, stateDesc);
}

bool ylib::network::http::response::send_file(const std::string& filepath, int32 downbaud, ushort stateNum, const std::string& stateDesc)
{
    std::string filepath2;
    filepath2.append(filepath);
    if(m_response == true)
        return false;

    long filesize = 0;
    time_t last_modify_time = 0;

    //ylib::log->info(filepath2,"response");
    //取文件信息
    {
        struct stat statbuf;
        if (stat(filepath2.c_str(), &statbuf) != 0)
            return false;
        filesize = statbuf.st_size;
        last_modify_time = statbuf.st_mtime;
    }
    // 设置为已发送
    m_response = true;
    //if (filesize != 0)
    //{
    //    stateCode = 200;
    //    stateDesc = "OK";
    //}
    //else
    //{
    //    stateCode = 404;
    //    stateDesc = "Not Found";
    //}

    //合成必要头

    if(headers()->find("Content-Type") == headers()->end()){
        std::string extName;
        std::string content_type;
        extName = file::ext(filepath2);
        ylib::network::content_type(extName, content_type);
        headers()->insert(std::pair<std::string,std::string>("Content-Type", content_type));
    }
    //时间
    {
        std::string gmt;
        time::now_gmt(gmt);
        HEADER_SET("Date", gmt);
    }


    //判断缓存
    if (filecache(last_modify_time))
        return true;

    // 直接发送
    bool direct_send = filesize <= 1024;
    ylib::buffer send_data;

    //计算断点续传
    long start = 0, len = 0;
    if(direct_send == false){
        if (fileoffset(filesize, start, len))
            stateNum = 206;
        if (m_headers.find("Transfer-Encoding") == m_headers.end())
        {
            HEADER_SET("Content-Length", std::to_string((uint64)len));
        }

    }else{
        send_data = ylib::file::read(filepath2);
        if (m_headers.find("Transfer-Encoding") == m_headers.end())
        {
            //GZIP
            if (m_reqpack->website()->config().gzip) {
                send_data = codec::gzip::en(send_data);
                HEADER_SET("Content-Encoding", "gzip");
                HEADER_SET("Content-Length", std::to_string((int64)send_data.length()));
            }
            else {
                HEADER_SET("Content-Length",std::to_string((int64)filesize));
            }
        }
    }


    {
        THeader* headers = new THeader[m_headers.size()];
        uint32 idx = 0;
        for_iter(iter, m_headers)
        {
            headers[idx].name = iter->first.c_str();
            headers[idx].value = iter->second.c_str();
            idx++;
        }
        if (!HPSERVER->SendResponse((CONNID)m_reqpack->connid(), stateNum, stateDesc.c_str(), headers, (int)m_headers.size()))
        {
            delete[] headers;
            return false;
        }
        delete[] headers;
    }

    if(direct_send){
        if (send_data.length() != 0)
        {
            this->m_reqpack->server()->qps()->response(0);
            return HPSERVER->Send((CONNID)m_reqpack->connid(), (const BYTE*)send_data.data(), (int)send_data.length());
        }

    }
    this->m_reqpack->server()->qps()->response(0);

    /***************计算发送文件**************/
    //读取块大小
    long blocksize = 0;

    if (downbaud == -1)
        blocksize = 4096;
    else
    {
        if (downbaud > 4096)
            blocksize = 4096;
        else
            blocksize = downbaud;
    }
    if (blocksize > len)
        blocksize = len;
    FILE* pFile = nullptr;
#ifdef _WIN32
    fopen_s(&pFile, filepath2.c_str(), "rb");
#else
    pFile = fopen(filepath2.c_str(), "rb");
#endif
    if (pFile == NULL)
        return false;
    fseek(pFile, start, SEEK_SET);

    char* read_buffer = new char[blocksize];
    size_t readlen = 0;
    timestamp begin_sec = time::now_sec();
    long read_size = 0;

    while ((readlen = fread(read_buffer, 1, blocksize, pFile)) > 0)
    {
        read_size += blocksize;
        if (HPSERVER->Send((CONNID)m_reqpack->connid(), (const BYTE*)read_buffer, (int)readlen, 0) == false)
        {
            fclose(pFile);
            delete[] read_buffer;
            return false;
        }
        //判断是否读够了
        if (read_size == len)
            break;
        if (len - read_size < blocksize)
        {
            blocksize = len - read_size;
        }
        //等待，压缩带宽
        if (downbaud != -1) {
            while ((time::now_sec()-begin_sec)*downbaud < read_size) {
                system::sleep_msec(100);
            }
        }
    }
    fclose(pFile);
    delete[] read_buffer;

    return true;
}
std::map<std::string,std::string>* ylib::network::http::response::headers()
{
    return &m_headers;
}
bool ylib::network::http::response::redirect(const std::string& filepath, bool MovedPermanently)
{
    if(m_response == true)
        return false;
    HEADER_SET("Location", filepath);
    return send(NULL, 0, MovedPermanently == true ? 301 : 302, "Found");
}

bool ylib::network::http::response::forward(const std::string& filepath)
{
    if(m_response == true)
        return false;
    m_response = true;
    network::http::reqpack* reqpack = new network::http::reqpack;
    reqpack->init(m_reqpack->url(),m_reqpack->host(), m_reqpack->data(), m_reqpack->connid(), m_reqpack->server());
    reqpack->filepath(filepath);
    m_reqpack->website()->router()->push(reqpack);
    return true;
}
bool ylib::network::http::response::filecache(const uint64& last_modify_time)
{
    std::string lastModified;
    ylib::time::to_gmt(last_modify_time, lastModified);
    //对比缓存
    LPCSTR lpszValue = nullptr;
    if (HPSERVER->GetHeader((CONNID)m_reqpack->connid(), "If-Modified-Since",&lpszValue))
    {
        if (lastModified == strutils::trim_end(strutils::F(lpszValue),{';'}))
        {
            //缓存一致 发送 304
            return  (bool)HPSERVER->SendResponse((CONNID)m_reqpack->connid(), 304, "Not Modified", nullptr, 0);
        }
    }
    // 最后修改时间
    HEADER_SET("Last-Modified", lastModified);
    // 设置缓存类型
    HEADER_SET("Cache-Control", "no-cache");
    return false;
}

bool ylib::network::http::response::fileoffset(long filesize, long& start, long& len)
{
    start = 0, len = filesize;

    LPCSTR lpszValue = nullptr;
    if (HPSERVER->GetHeader((CONNID)m_reqpack->connid(), "Range", &lpszValue) == FALSE)
        return false;
    std::string hValue =strutils::F(lpszValue);

    if (hValue.length() < 8)
        return false;
    hValue = hValue.substr(6, hValue.length() - 6);
    auto __arr = strutils::split(hValue,"-");
    if (__arr.size() == 2)
    {
        start = atol(__arr[0].c_str());
        len = atol(__arr[1].c_str());
        if (len == 0)
            len = filesize - start;
    }
    else
    {
        if (hValue[0] == '-')
        {
            //下载最后x字节
            len = atol(std::string(hValue.substr(1, hValue.length() - 2)).c_str());
            if (len > filesize)
                return false;
            start = filesize - len;
        }
        else if (hValue[hValue.length() - 1] == '-')
        {
            //从x字节开始下载
            start = atol(std::string(hValue.substr(0, hValue.length() - 2)).c_str());
            if (start > filesize)
                return false;
            len = filesize - start;
        }
    }
    if (start + len > filesize)
        return false;
    if (len < 0)
        return false;
    HEADER_SET("Content-Range", std::to_string((int64)start) + "-" + std::to_string((int64)len) + "/" + std::to_string((int64)filesize));
    return true;
}
#endif

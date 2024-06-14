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

// QQ：1585346868

#include "net/http_header.h"
#if USE_NET_HTTP_UTIL
#include "util/strutils.h"
ylib::network::http::header::header()
{
}

ylib::network::http::header::~header()
{
}

void ylib::network::http::header::clear()
{
    m_param.clear();
    m_values.clear();
    m_name = "";
}

void ylib::network::http::header::name(const std::string& value)
{
	m_name = value;
}
std::string ylib::network::http::header::name() const
{
    return m_name;
}


std::map<std::string, std::string> ylib::network::http::header::param()
{
    // TODO: 在此处插入 return 语句
    return m_param;
}

void network::http::header::parse(const std::string &body)
{
    m_values.clear();
    m_param.clear();
    std::string value = body;
    std::vector<std::string> param = strutils::split(value,";");
    if (param.size() == 0)
    {
        value = strutils::trim(value,{' '});
        auto arr = strutils::split(value,"=");
        if (arr.size() == 2)
            m_param[arr[0]] = arr[1];
    }
    else
    {
        for (uint32 i = 0; i < param.size(); i++)
        {
            param[i] = strutils::trim(param[i],{' '});
            auto arr = strutils::split(param[i],"=");
            if (arr.size() != 2)
                m_values.push_back(param[i]);
            else
                m_param[arr[0]] = arr[1];
        }
    }
}
void ylib::network::http::header::push(const std::string& value)
{
    m_values.push_back(value);
}

void network::http::header::push(const std::string &key, const std::string &value)
{
    m_param[key] = value;
}
bool ylib::network::http::header::exist(const std::string& name)
{
    
    if ((m_param.find(name) != m_param.end()) == false)
    {
        for (size_t i = 0; i < m_values.size(); i++)
        {
            if (m_values[i] == name)
                return true;
        }
        return false;
    }
    return true;
}

std::string ylib::network::http::header::to_string() const
{
    std::string value;
    for_iter(iter, m_values)
        value.append(*iter + "; ");

    for_iter(iter, m_param)
    {
        std::string line = std::string(iter->first + "=" + iter->second + ";");
        value.append(line);
    }


    
    return strutils::trim_end(value,{' ',';'});
}

ylib::network::http::header_list::header_list()
{

}

ylib::network::http::header_list::~header_list()
{
    clear();
}

bool ylib::network::http::header_list::exist(const std::string& name) const
{
    return m_headers.find(name) != m_headers.end();
}

ylib::network::http::header ylib::network::http::header_list::get(const std::string& name)
{
    auto iter = m_headers.find(name);
    if (iter == m_headers.end())
    {
        m_headers[name].name(name);
    }
    return m_headers[name];
}

void ylib::network::http::header_list::set(const std::string& name, const ylib::network::http::header& header)
{
    m_headers[name] = header;
}

void ylib::network::http::header_list::set(const std::string& name, const std::string& value)
{
    m_headers[name].parse(value);
}
void ylib::network::http::header_list::del(const std::string& name)
{
    auto iter = m_headers.find(name);
    if (iter == m_headers.end())
        return;
    m_headers.erase(iter);
}

size_t ylib::network::http::header_list::size()
{
    return m_headers.size();
}

void ylib::network::http::header_list::clear()
{
    m_headers.clear();
}

std::map<std::string,std::string> ylib::network::http::header_list::to() const
{
    std::map <std::string, std::string> result;
    for_iter(iter, m_headers)
    {
        result[iter->first] = iter->second.to_string();
        
    }
        
    return result;
}
#endif

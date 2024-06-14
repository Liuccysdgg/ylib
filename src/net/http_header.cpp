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

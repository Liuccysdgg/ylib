﻿/*Software License

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

#include "util/cache.h"
#include "util/file.h"
#include "util/system.h"
#include "util/time.h"
#include "util/codec.h"
ylib::cache::cache()
{
}

ylib::cache::~cache()
{
}

bool ylib::cache::start(const std::string& local_path)
{
    stop();
    // 遍历本地缓存
    if(local_path != "")
    {
#ifdef _WIN32
        ylib::file::create_dir(m_local_path, true);
        std::map<std::string, bool> values;
        ylib::file::list(m_local_path, values);
        for_iter(iter, values)
        {
            if (iter->first == "." || iter->first == "..")
                continue;
            if (iter->second == true)
                continue;
            std::string filepath = m_local_path + "\\" + iter->first;
            auto v = ylib::json::from(ylib::file::read(filepath).to_string());
            std::string name = v["name"].to<std::string>();
            std::string value = v["value"].to<std::string>();
            timestamp update_sec = v["update_sec"].to<uint64>();
            uint32 timeout_sec = v["timeout_sec"].to<uint32>();
            if (update_sec + timeout_sec < time::now_sec())
            {
                ylib::file::remove(filepath);
                continue;
            }
            write(name,value, update_sec,timeout_sec);
        }
#endif
    }
    return ::ithread::start();
}

void ylib::cache::stop()
{
    ::ithread::stop();
    ::ithread::wait();
    
    /*释放内存缓存*/
    {
        std::unique_lock<std::mutex> guard(m_mutex);
        for_iter(iter, m_cache)
            delete iter->second;
        m_cache.clear();
    }

    m_local_path.clear();
}

bool ylib::cache::read(const std::string& name, std::string& value)
{
    std::unique_lock<std::mutex> guard(m_mutex);
    auto iter = m_cache.find(name);
    if(iter == m_cache.end())
        return false;
    /*是否过期*/
    {
        if (iter->second->timeout_sec != -1)
        {
            if (iter->second->update_sec + iter->second->timeout_sec < time::now_sec())
            {
                m_cache.erase(iter);
                return false;
            }
        }
    }
    value = iter->second->value;
    return true;
}

bool ylib::cache::write(const std::string& name,const std::string& value, int32 timeout_sec)
{
    return write(name, value, time::now_sec(), timeout_sec);
}

bool ylib::cache::write(const std::string& name, const std::string& value, timestamp update_sec, int32 timeout_sec)
{
    std::unique_lock<std::mutex> guard(m_mutex);
    cache_info* info = nullptr;
    auto iter = m_cache.find(name);
    if (iter == m_cache.end())
    {
        info = new cache_info;
        m_cache.insert(std::pair<std::string, cache_info*>(name, info));
    }
    else
        info = iter->second;
    info->update = true;
    info->update_sec = update_sec;
    if (timeout_sec != -2)
    {
        info->timeout_sec = timeout_sec;
    }

    info->value = value;
    return true;
}

bool ylib::cache::exist(const std::string& name)
{
    return _exist(name, true);
}

int32 ylib::cache::expire(const std::string& name)
{
   std::unique_lock<std::mutex> guard(m_mutex);
    auto iter = m_cache.find(name);
    if (iter == m_cache.end())
        return -2;
    
    if (iter->second->timeout_sec == -1)
        return -1;
    timestamp now_time = time::now_sec();
    if (iter->second->update_sec + iter->second->timeout_sec < now_time)
    {
        m_cache.erase(iter);
        return -2;
    }
    return (int32)(iter->second->update_sec + iter->second->timeout_sec - now_time);

}

bool ylib::cache::remove(const std::string& name)
{
    std::unique_lock<std::mutex> guard(m_mutex);
    cache_info* info = nullptr;
    auto iter = m_cache.find(name);
    if (iter == m_cache.end())
        return false;
    m_cache.erase(iter);
    return true;
}

bool ylib::cache::update(const std::string& name)
{
    std::unique_lock<std::mutex> guard(m_mutex);
    auto iter = m_cache.find(name);
    if(iter == m_cache.end())
        return false;
    /*是否过期*/
    {
        if (iter->second->timeout_sec != -1)
        {
            if (iter->second->update_sec + iter->second->timeout_sec < time::now_sec())
            {
                m_cache.erase(iter);
                return false;
            }
        }
    }
    iter->second->update = true;
    iter->second->update_sec = time::now_sec();
    return true;
}

void ylib::cache::clear()
{
    std::unique_lock<std::mutex> guard(m_mutex);
    for_iter(iter, m_cache)
        delete iter->second;
    m_cache.clear();
}
std::map<std::string, ylib::cache::cache_info*>* ylib::cache::list()
{
    return &m_cache;
}
std::mutex* ylib::cache::mutex()
{
    return &m_mutex;
}
bool ylib::cache::_exist(const std::string& name, bool is_lock)
{
    std::unique_lock<std::mutex> guard(m_mutex);
    auto iter = m_cache.find(name);
    if(iter == m_cache.end())
        return false;
    /*是否过期*/
    {
        if (iter->second->timeout_sec != -1)
        {
            if (iter->second->update_sec + iter->second->timeout_sec < time::now_sec())
            {
                m_cache.erase(iter);
                return false;
            }
        }
    }
    return true;
}
bool ylib::cache::run()
{


    m_mutex.lock();
    std::list<std::string> remove_list;
    for_iter(iter, m_cache)
    {
        if (iter->second->timeout_sec == -1)
            continue;
        timestamp now_time = time::now_sec();
        if (iter->second->update_sec + iter->second->timeout_sec < now_time)
            remove_list.push_back(iter->first);
        else
        {
            if (iter->second->update && m_local_path.empty()==false)
            {
                ylib::json v;
                v["value"] = iter->second->value;
                v["update_sec"] = iter->second->update_sec;
                v["timeout_sec"] = iter->second->timeout_sec;
                v["name"] = iter->first;

                ylib::file::write(m_local_path+"\\"+codec::md5(iter->first)+".txt",v.to_string());
                iter->second->update = false;
            }
        }
    }
    for_iter(iter, remove_list)
    {
        auto itm = m_cache.find(*iter);
        if (itm != m_cache.end())
            m_cache.erase(itm);
    }
    m_mutex.unlock();




    for (uint32 i = 0; i <5; i++)
    {
        if (::ithread::m_state == 1)
            break;
        system::sleep_msec(500);
    }
    return true;
}

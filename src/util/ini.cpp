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

#include "util/ini.h"
#include "base/define.h"
#include "simpleini/SimpleIni.h"
#define INI_POINT ((CSimpleIniA*)m_point)
ylib::ini::ini()
{
    
}

ylib::ini::~ini()
{
    
}

bool ylib::ini::open(const std::string& filepath)
{
    m_filepath = filepath;
#ifndef _WIN321
    close();
    m_point = new CSimpleIniA();
    return INI_POINT->LoadFile(filepath.c_str()) >= 0;
#else
    return true;
#endif
}
void ylib::ini::close()
{
#ifndef _WIN321
    if(m_point != nullptr)
        delete INI_POINT;
    m_point = nullptr;
#else

#endif
}
std::string ylib::ini::read(const std::string& name, const std::string& key,const std::string& default_value) const
{
#ifndef _WIN321
    return INI_POINT->GetValue(name.c_str(), key.c_str(),default_value.c_str());
#else
    std::string value;
    char result[1024 * 10];
    auto resultLength = GetPrivateProfileStringA(name.c_str(), key.c_str(), "", result, 1024 * 10, m_filepath.c_str());
    if (resultLength > 1024 * 10)
        return default_value;
    if (resultLength == 0)
        return default_value;
    else
        value.append(result, resultLength);
    return value;
#endif
}

bool ylib::ini::write(const std::string& name, const std::string& key, const std::string& value)
{
#ifndef _WIN321
    if (INI_POINT->SetValue(name.c_str(), key.c_str(), value.c_str()) < 0)
        return false;
    
    return INI_POINT->SaveFile(m_filepath.c_str()) >= 0;
#else
    return WritePrivateProfileStringA(name.c_str(), key.c_str(), value.c_str(), m_filepath.c_str());
#endif
}
#ifndef _WIN321
bool ylib::ini::del(const std::string& name, const std::string& key)
{

    if (!INI_POINT->Delete(name.c_str(), key.c_str(), true))
        return false;
    return INI_POINT->SaveFile(m_filepath.c_str()) >= 0;
}

std::vector<std::string> ylib::ini::names()
{
    std::list<CSimpleIniA::Entry> list;
    INI_POINT->GetAllSections(list);
    std::vector<std::string> result;
    for_iter(iter, list) 
    {
        if (iter->pItem != NULL) 
            result.push_back(iter->pItem);
    }
    return result;
}
std::vector<std::string> ylib::ini::keys(const std::string& name)
{
    std::list<CSimpleIniA::Entry> list;
    INI_POINT->GetAllKeys(name.c_str(),list);
    std::vector<std::string> result;
    for_iter(iter, list)
    {
        if (iter->pItem != NULL)
            result.push_back(iter->pItem);
    }
    return result;
}
ylib::json ylib::ini::to_json()
{
    ylib::json result;
    auto nodes = this->names();
    for (size_t i = 0; i < nodes.size(); i++)
    {
        ylib::json keyValues;
        auto keys = this->keys(nodes[i]);
        for (size_t x = 0; x < keys.size(); x++)
            keyValues[keys[x]] = read(nodes[i],keys[x]);
        result[nodes[i]] = keyValues;
    }
    return result;
}
bool ylib::ini::exist_key(const std::string& name,const std::string& key)
{
    return INI_POINT->KeyExists(name.c_str(), key.c_str());
}
bool ylib::ini::exist_name(const std::string& name)
{
    return INI_POINT->SectionExists(name.c_str());
}
#endif
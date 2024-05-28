#include "net/http_parser.h"
#if USE_NET_HTTP_WEBSITE
#include "util/file.h"
#include "util/strutils.h"
network::http::parser::parser()
{
    m_method = network::http::ALL;
}

network::http::parser::~parser()
{

}

const ylib::json& ylib::network::http::parser::json()
{
    return m_json_param;
}

std::string ylib::network::http::parser::text()
{
    return m_data.to_string();
}

bool network::http::parser::init(const std::string &url,const network::http::method& method,const ylib::buffer &data, const std::string &content_type)
{
    m_url = url;
    m_data = data;
    m_content_type = content_type;
    m_method = method;


    /*解析URL*/
    parse_url(m_url,m_url_param);
    switch (m_method) {
    case network::http::GET:
        break;
    case network::http::POST:
        {
            if(content_type == "application/x-www-form-urlencoded")
            {
                parse_url(m_data.to_string(),m_body_param);
            }
            else if(content_type.find("application/json") != -1)
            {
                parse_json();
            }
            else if(content_type.find("multipart/form-data") !=-1)
            {
                parse_form();
            }
        }
        break;
    default:
        break;
    }
    return true;
}

bool network::http::parser::url_param(const std::string &name, std::string &value)
{
        return read_url_param(m_url_param,name,value);
}

bool ylib::network::http::parser::url_param_exist(const std::string& name)
{
    return m_url_param.find(name) != m_url_param.end();
}

std::vector<std::string> network::http::parser::url_param_names()
{
    std::vector<std::string> names;
    for_iter(iter,m_url_param)
        names.push_back(iter->first);
    return names;
}

bool network::http::parser::body_param(const std::string &name, std::string &value)
{
    if(m_method == POST)
    {
        if(m_content_type == "application/x-www-form-urlencoded")
        {
            return read_url_param(m_body_param,name,value);
        }
        else if(m_content_type == "application/json")
        {
            if(!m_json_param.exist(name))
                return false;
            if(m_json_param[name].is_number()){
                value = std::to_string(m_json_param[name].to<double>());
            }else{
                value = m_json_param[name].to<std::string>();
            }
            return true;
        }
        return false;
    }
    return false;
}

bool ylib::network::http::parser::body_param_exist(const std::string& name)
{
    if (m_method == POST)
    {
        if (m_content_type == "application/x-www-form-urlencoded")
        {
            return m_body_param.find(name) != m_body_param.end();
        }
        else if (m_content_type == "application/json")
        {
            return m_json_param.exist(name);
        }
    }
    return false;
}

std::vector<std::string> network::http::parser::body_param_names()
{

    std::vector<std::string> names;
    if(m_method == POST)
    {
        if(m_content_type == "application/x-www-form-urlencoded")
        {
            for_iter(iter,m_body_param)
                names.push_back(iter->first);
        }
        else if(m_content_type == "application/json")
        {
            return m_json_param.keys();
        }
    }
    return names;
}

network::http::form_parser *network::http::parser::form()
{
    return &m_form;
}

void network::http::parser::parse_url(const std::string &url, std::map<std::string,std::string> &map)
{
    map.clear();
    auto ps =  strutils::split(url,'&');
    for(size_t i=0;i<ps.size();i++)
    {
        auto pi = strutils::split(ps[i],'=');
        if(pi.size() == 2)
            map.insert(std::pair<std::string,std::string>(pi[0],pi[1]));
    }
}
void network::http::parser::parse_json()
{
    m_json_param.parse(m_data.to_string());
}

void network::http::parser::parse_form()
{
    m_form.parse(&m_data);
}

bool network::http::parser::read_url_param(const std::map<std::string,std::string> &map, const std::string &name, std::string &value)
{
    auto iter = map.find(name);
    if(iter == map.end())
        return false;
    value = iter->second;
    return true;
}

network::http::form_parser::form_parser()
{

}

network::http::form_parser::~form_parser()
{

}

std::vector<std::string> network::http::form_parser::names()
{
    std::vector<std::string> result;
    for_iter(iter,m_infos)
        result.push_back(iter->first);
    return result;
}

bool network::http::form_parser::get(const std::string &name, form_info &info)
{
    auto iter = m_infos.find(name);
    if(iter == m_infos.end())
        return false;
    info = iter->second;
    return true;
}

bool network::http::form_parser::write_file(const std::string &name, const std::string &filepath)
{
    auto iter = m_infos.find(name);
    if(iter == m_infos.end())
        return false;
    return ylib::file::write(filepath,(const char*)m_data->data()+iter->second.start,iter->second.length);
}

bool network::http::form_parser::parse(buffer *data)
{
    this->m_data = data;

    std::vector<uint32> starts;
    std::vector<uint32> lengths;
    parse_count(starts,lengths);


    for(size_t i=0;i<starts.size();i++)
        parse_form(starts[i],lengths[i]);

    return true;
}

void network::http::form_parser::parse_count(std::vector<uint32> &starts, std::vector<uint32> &lengths)
{


    ylib::buffer cut_flag;
    {
        int32 curr_idx = 0;
        while (char c = getchar(curr_idx++))
        {
            if (c == '\r' && getchar(curr_idx) == '\n')
            {
                curr_idx++;
                break;
            }
            else
                cut_flag.append<char>(c);
        }
    }

    auto idxVct = m_data->find_list(cut_flag);
    for (size_t i = 0; i < idxVct.size(); i++)
    {
        size_t startIdx = idxVct[i] + cut_flag.length() + 2;
        if (i + 1 != idxVct.size())
        {
            starts.push_back((uint32)startIdx);
            lengths.push_back((uint32)(idxVct[i + 1] - idxVct[i] - cut_flag.length() - 2));
        }
    }
}

void network::http::form_parser::parse_form(uint32 start,uint32 length)
{
    auto parse_paraminfo_secondkv=[](const ylib::buffer& buf)->ylib::KeyValue<std::string,std::string>
    {
        ylib::KeyValue<std::string,std::string> kv;
        buffer cutV;
        cutV.resize(1);
        cutV[0] = '=';
        auto cdIdx = buf.find(cutV, 0);
        if (cdIdx == -1)
            return kv;
        kv.name = buf.sub(0, cdIdx).to_string();
        kv.value = buf.sub(cdIdx + 2, buf.length() - 3 - cdIdx).to_string();
        return kv;
    };


    ylib::buffer form_data(m_data->data()+start,length);

    ylib::buffer fh;
    ylib::buffer fh2;
    ylib::buffer cs_ln;
    fh.resize(2);
    fh2.resize(2);
    cs_ln.resize(2);
    fh[0] = ';';
    fh[1] = ' ';
    fh2[0] = ':';
    fh2[1] = ' ';
    cs_ln[0] = '\r';
    cs_ln[1] = '\n';

    auto div = form_data.find_list(cs_ln);
    if (div.size() == -1)
        return;

    form_info forminfo;
    //数据
    if (div.size() == 3)
    {
        forminfo.start = div[1] + 2+start;
        forminfo.length = div[div.size() - 1] - 2 - div[1];
    }
    else if (div.size() > 3)
    {
        forminfo.start = div[2] + 2+start;
        forminfo.length = div[div.size() - 1] - 2 - div[2];

        auto ContentType = form_data.sub(div[0] + 2, div[1] - 2 - div[0]);
        // Content-Type
        auto cdIdx = ContentType.find(fh2);
        if (cdIdx == -1)
            return;
        forminfo.content_type = ContentType.sub(cdIdx + 2, ContentType.length() - 2 - cdIdx);
    }
    else
        return;

    //配置项
    auto info = form_data.sub(div[0]);
    std::string t = info;
    auto info_vct = info.find_list(fh);
    if (info_vct.size() == 0)
        return;

    // Content-Disposition
    auto contentDisposition = info.sub(info_vct[0]);
    auto cdIdx = contentDisposition.find(fh2);
    if (cdIdx == -1)
        return;

    forminfo.disposition = contentDisposition.sub(cdIdx + 2, contentDisposition.length() - 2 - cdIdx);



    if (info_vct.size() == 1)
    {
        auto kv = parse_paraminfo_secondkv(info.sub(info_vct[0] + 2, info.length() - info_vct[0] - 2));
        if (kv.name == "name")
            forminfo.name = kv.value;
        else if (kv.name == "filename")
            forminfo.filename = kv.value;
    }
    else
    {
        for (size_t i = 0; i < info_vct.size(); i++)
        {
            ylib::buffer info_sub;
            if (info_vct.size() == i + 1)
                info_sub = info.sub(info_vct[i] + 2, info.length() - info_vct[i] - 2);
            else
                info_sub = info.sub(info_vct[i] + 2, info_vct[i + 1] - info_vct[i] - 2);
            auto kv = parse_paraminfo_secondkv(info_sub);
            if (kv.name == "name")
                forminfo.name = kv.value;
            else if (kv.name == "filename")
                forminfo.filename = kv.value;
        }
    }
    m_infos.insert(std::pair<std::string,form_info>(forminfo.name,forminfo));
}

char network::http::form_parser::getchar(size_t index)
{
    if (index + 1 > m_data->length())
        throw 0;
    return *(m_data->data() + index);
}
#endif

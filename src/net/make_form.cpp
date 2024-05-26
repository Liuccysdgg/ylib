#include "net/make_form.h"
#if USE_NET_HTTP_UTIL
#include "util/time.h"
#include "util/system.h"
#include "util/file.h"
ylib::network::http::make_form::make_form()
{
}

ylib::network::http::make_form::~make_form()
{
	for_iter(iter,m_list)
		delete (*iter);
}

bool ylib::network::http::make_form::add(const std::string& name, const std::string& value)
{
    if(m_list.find([=](form_info* param, uint32 idx)->bool {
        return param->name == name;
        }))
        return false;
    form_info* param = new form_info;
	param->name = name;
    param->disposition = "form-data";
	param->data = value;
	m_list.push_back(param);
	return true;
}

bool ylib::network::http::make_form::add(const std::string& name, const std::string& filename, const std::string& content_type,const ylib::buffer& data)
{
    if(m_list.find([=](form_info* param, uint32 idx)->bool {
		return param->name == name;
    }))
        return false;
    form_info* param = new form_info;
	param->name = name;
    param->disposition = "form-data";
	param->data = data;
    param->content_type = content_type;
	param->filename = filename;
	m_list.push_back(param);
	return true;
}

bool ylib::network::http::make_form::make(ylib::buffer& data, std::string& boundary) const
{
	for (uint32 i = 0; i < 27; i++)
    {
        char num = (char)system::random(48, 57);
		boundary.append(&num,1);
	}
		

	boundary = "--------" + boundary;

    uint32 current_idx = 0;
    data.append(std::string("--" + boundary + "\r\n"));
	for_iter(iter,m_list)
	{
        if ((*iter)->content_type != "")
		{
            data.append(std::string("Content-Disposition: form-data; name=\""+ (*iter)->name+"\"; filename=\""+ (*iter)->filename+"\"\r\n"));
            data.append(std::string("Content-Type: ")+ (*iter)->content_type +std::string("\r\n\r\n"));
		}
		else
            data.append(std::string("Content-Disposition: form-data; name=\"" + (*iter)->name + "\"\r\n\r\n"));
		data.append((*iter)->data);
        data.append(std::string("\r\n"));
		if (current_idx + 1 == m_list.size())
            data.append(std::string("--"+ boundary +"--\r\n"));
        else
            data.append(std::string("--" + boundary + "\r\n"));
		current_idx++;
	}
	return true;
}
#endif

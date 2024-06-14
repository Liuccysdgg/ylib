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

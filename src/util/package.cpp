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

#include "util/package.h"
#include "util/codec.h"
#include "base/conversion.h"
#define CHECK_DATA_LENGTH(DATA,LENGTH) if (DATA->length() < (LENGTH))return false;
ylib::package::package()
{
}

ylib::package::~package()
{
	free();
}

bool ylib::package::parse(const ylib::buffer& data_param)
{
	free();
	ylib::buffer data;
	data = data_param;
	if (data.length() == 0)
		return false;
	uint32 idx = 0;
	while (true)
	{
		
		std::string name;
		ylib::buffer* signal = nullptr;
		{
			/*名称解析*/
			uint32 name_length = 0;
			if (data.length() < idx + 4)
				break;
			bytes::to_uint(name_length, data.data()+idx);
			idx += 4;
			if (data.length() < idx + name_length)
				break;
			name = data.sub(idx, name_length);
			idx += name_length;
		}
		{
			/*数据解析*/
			uint32 data_length = 0;
			if (data.length() < idx + 4)
				break;
			bytes::to_uint(data_length, data.data()+idx);
			idx += 4;
			if (data.length() < idx + data_length)
			{
				return m_parse_list.size() != 0;
			}
			signal = new ylib::buffer;
			*signal = data.sub(idx, data_length);
			idx += data_length;
		}
		m_parse_list.add(name,signal);
		if (idx == data.length())
			break;
	}
	return true;
}


bool ylib::package::add(const std::string& name, const ylib::buffer& data)
{
	if (m_list.exist(name))
		return false;
	packstruct* ps = new packstruct;
	ps->name = name;
	ps->data = data;
	m_list.add(name, ps);
	return true;
}


void ylib::package::free()
{
	/*释放合成资源*/
	{
		auto list = m_list.to_stl();
		for_iter(iter, list)
		{
			delete iter->second;
		}
		m_list.clear();
	}
	/*释放解析资源*/
	{
		auto list = m_parse_list.to_stl();
		for_iter(iter, list)
		{
			delete iter->second;
		}
		m_parse_list.clear();
	}
}

void ylib::package::to(const std::string& password, ylib::buffer& data)
{
	// 0=32整数 1=double浮点  2=文本型 3=字节流
	auto list  = m_list.to_stl();
	size_t count = list.size();
	data.clear();
	for_iter(iter, list)
	{
		ylib::buffer signal_name;
		ylib::buffer signal_data;
		// 填充名称
		{
			signal_name.append(bytes::to_buffer((int32)iter->second->name.length()));
			signal_name.append(iter->second->name);
		}

		signal_data.append<uchar>(3);
		signal_data.append(bytes::to_buffer((int32)iter->second->data.length()));
		signal_data.append(iter->second->data);
		// 打包加密
		{
			std::string iter_pwd;
			data.append(signal_name);
			data.append(bytes::to_buffer((int32)signal_data.length()));
			data.append(signal_data);
		}
	}
}
bool ylib::package::get(const std::string& name, ylib::buffer& data)
{
	ylib::buffer* signal = nullptr;
	if (m_parse_list.get(name, signal) == false)
		return false;
	uint32 idx = 0;
	CHECK_DATA_LENGTH(signal, idx + 1);
	// 0=32整数 1=double浮点  2=文本型 3=字节流
	uint32 type = (*signal)[idx];
	idx++;
	CHECK_DATA_LENGTH(signal, idx + 4);
	uint32 length = 0;
	bytes::to_uint(length, signal->data() + idx);
	idx += 4;
	if (type == 3)
	{
		CHECK_DATA_LENGTH(signal, idx + length);
		data = signal->sub(idx,length);
		return true;
	}
	return false;
}
void ylib::package::clear()
{
	free();
}
void ylib::package::___test__c_to_utf8()
{
	std::map<std::string, packstruct*> list;
	std::map<std::string, ylib::buffer*> parse_list;
	auto l = m_list.to_stl();
	auto l2 = m_parse_list.to_stl();
	for_iter(iter, l)
		list.emplace(codec::to_utf8(iter->first),iter->second);
	for_iter(iter, l2)
		parse_list.emplace(codec::to_utf8(iter->first), iter->second);
	*((std::map<std::string, packstruct*>*)&m_list) = list;
	*((std::map<std::string, ylib::buffer*>*) & m_parse_list) = parse_list;

}

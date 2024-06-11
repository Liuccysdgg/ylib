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

#include "util/localstorage.h"
#include "leveldb/db.h"
#include "util/time.h"
#include "util/codec.h"
ylib::local_storage::local_storage()
{
}

ylib::local_storage::~local_storage()
{
}

bool ylib::local_storage::open(const std::string& dirpath)
{
	close();
	leveldb::Options options;
	options.create_if_missing = true;  // 如果数据库不存在，则创建

	// 打开数据库
	leveldb::Status status = leveldb::DB::Open(options, dirpath, &m_db);
	if (status.ok() == false)
	{
		m_lastErrorDesc = status.ToString();
		return false;
	}
	return true;
}

void ylib::local_storage::close()
{
	if (m_db != nullptr)
		delete m_db;
	m_db = nullptr;
}

void ylib::local_storage::clear()
{
}

bool ylib::local_storage::write(const std::string& name, const std::string& value)
{
	return m_db->Put(leveldb::WriteOptions(), name, value).ok();
}

bool ylib::local_storage::read(const std::string& name, std::string& value)
{
	return m_db->Get(leveldb::ReadOptions(), name, &value).ok();
}

bool ylib::local_storage::del(const std::string& name)
{
	return m_db->Delete(leveldb::WriteOptions(), name).ok();
}

bool ylib::local_storage::exist(const std::string& name)
{
	std::string value;
	auto status = m_db->Get(leveldb::ReadOptions(), name, &value);
	if (status.ok()) {
		return true;
	}
	else if (status.IsNotFound()) {
		return false;
	}
	return false;
}

//std::string ylib::local_storage::test_make_guid()
//{
//	return codec::md5(std::to_string(time::now_msec()) + std::to_string(m_guid_counter.make()));
//}

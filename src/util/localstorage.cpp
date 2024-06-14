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

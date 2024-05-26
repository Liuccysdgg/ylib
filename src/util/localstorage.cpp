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
	return status.ok();
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

std::string ylib::local_storage::test_make_guid()
{
	return codec::md5(std::to_string(time::now_msec()) + std::to_string(m_guid_counter.make()));
}

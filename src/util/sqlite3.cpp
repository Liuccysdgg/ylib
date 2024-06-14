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
#include "sqlite3/sqlite3.h"
#include "util/sqlite3.h"
#include "util/codec.h"
#include "util/file.h"
#include "base/conversion.h"
#define DEBUG_PRINT_SQL 0

#define DB_PTR (::sqlite3*)m_db
ylib::sqlite3::sqlite3()
{
	m_db = nullptr;
}
ylib::sqlite3::~sqlite3()
{
	close();
}
bool ylib::sqlite3::open(const std::string& filepath, const std::string& username, const std::string& password)
{
	
	close();
	m_db = nullptr;
	if (sqlite3_open(codec::to_utf8(filepath).c_str(),(::sqlite3**)&m_db) != 0)
	{
		if (m_db != nullptr)
		{
			this->m_lastErrorDesc = sqlite3_errmsg(DB_PTR);
			close();
		}
		
		return false;
	}
	return true;
}

void ylib::sqlite3::close()
{
	if(m_db!= nullptr)
		sqlite3_close(DB_PTR);
	m_db = nullptr;
}

bool ylib::sqlite3::exec(const std::string& sql)
{
	if (m_db == nullptr)
		return false;
#if DEBUG_PRINT_SQL == 1
#ifdef _DEBUG
	std::cout << "[SQLITE3] :\t" << codec::to_gbk(sql) << std::endl;
#endif
#endif
	char* cErrMsg = nullptr;
	if (sqlite3_exec(DB_PTR, sql.c_str(), nullptr, nullptr, &cErrMsg) != SQLITE_OK)
	{
		m_lastErrorDesc = cErrMsg;
		return false;
	}
	return true;
}
static int QueryResult(void* param, int column, char** argv, char** azColName)
{
	std::vector<std::map<std::string, std::string>>* data = (std::vector<std::map<std::string, std::string>>*)param;
	std::map<std::string, std::string> line;
	for (int i = 0; i < column; i++)
	{
		line[azColName[i]] = argv[i] == NULL ? "" : argv[i];
	}
		
	data->push_back(line);
	return 0;
}
bool ylib::sqlite3::query(const std::string& sql, std::vector<std::map<std::string, std::string>>& data)
{
	if (m_db == nullptr)
		return false;
#if DEBUG_PRINT_SQL == 1
#ifdef _DEBUG
	std::cout << "[SQLITE3] :\t" << codec::to_gbk(sql) << std::endl;
#endif
#endif
	char* cErrMsg = nullptr;
	if (sqlite3_exec(DB_PTR, sql.c_str(), QueryResult, (void*)&data, &cErrMsg) != SQLITE_OK)
	{
		this->m_lastErrorDesc = cErrMsg;
		return false;
	}
	return true;
}

int64 ylib::sqlite3::count(const std::string& sql)
{

	SQLITE_RESULT results;
	if (query(sql, results) == false)
		return 0;
	if (results.size() != 1)
		return 0;
	return ylib::stoll(results[0].begin()->second);
}
int64 ylib::sqlite3::last_insert_id()
{
	if (m_db == nullptr)
		return 0;
	return sqlite3_last_insert_rowid(DB_PTR);
}
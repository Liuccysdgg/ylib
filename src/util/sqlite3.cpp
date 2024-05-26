#include "util/sqlite3.h"
#include "util/codec.h"
#include "util/file.h"
#include "base/conversion.h"
#define DEBUG_PRINT_SQL 0
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
	if (sqlite3_open(codec::to_utf8(filepath).c_str(), &m_db) != 0)
	{
		this->m_lastErrorDesc = sqlite3_errmsg(m_db);
		return false;
	}
	return true;
}

void ylib::sqlite3::close()
{
	if(m_db!= nullptr)
		sqlite3_close(m_db);
	m_db = nullptr;
}

bool ylib::sqlite3::exec(const std::string& sql)
{
#if DEBUG_PRINT_SQL == 1
#ifdef _DEBUG
	std::cout << "[SQLITE3] :\t" << codec::to_gbk(sql) << std::endl;
#endif
#endif
	return sqlite3_exec(m_db, sql.c_str(),nullptr,nullptr,nullptr) == SQLITE_OK;
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
#if DEBUG_PRINT_SQL == 1
#ifdef _DEBUG
	std::cout << "[SQLITE3] :\t" << codec::to_gbk(sql) << std::endl;
#endif
#endif
	char* cErrMsg = nullptr;
	if (sqlite3_exec(m_db, sql.c_str(), QueryResult, (void*)&data, &cErrMsg) != SQLITE_OK)
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
	return sqlite3_last_insert_rowid(m_db);
}
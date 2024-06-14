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

// QQ：1585346868
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
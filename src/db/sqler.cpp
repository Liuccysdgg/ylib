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

#include "db/sqler.h"
#include <numeric>
/// <summary>
/// 检查Any类型
/// </summary>
/// <param name="value"></param>
/// <returns></returns>
bool checkAnyType(const std::any& value)
{
	if (value.type() == typeid(int)) {
		return true;
	}
	else if (value.type() == typeid(unsigned int)) {
		return true;
	}
	else if (value.type() == typeid(std::string)) {
		return true;
	}
	else if (value.type() == typeid(int64)) {
		return true;
	}
	else if (value.type() == typeid(uint64)) {
		return true;
	}
	else if (value.type() == typeid(double)) {
		return true;
	}
	else if (value.type() == typeid(float)) {
		return true;
	}
	else if (value.type() == typeid(std::vector<uchar>)) {
		return true;
	}
	else {
		return false;
	}
}
/// <summary>
/// 置入插入数据
/// </summary>
/// <param name="ppst"></param>
/// <param name="index"></param>
/// <param name="value"></param>
void setInsetValue(mysql::prepare_statement*ppst, uint32 index, const std::any& value)
{
	if (value.type() == typeid(int32)) {
		ppst->set_int32(index,std::any_cast<int32>(value));
	}
	else if (value.type() == typeid(unsigned int)) {
		ppst->set_uint32(index, std::any_cast<uint32>(value));
	}
	else if (value.type() == typeid(std::string)) {
		ppst->set_string(index, std::any_cast<std::string>(value));
	}
	else if (value.type() == typeid(int64)) {
		ppst->set_int64(index, std::any_cast<int64>(value));
	}
	else if (value.type() == typeid(uint64)) {
		ppst->set_uint64(index, std::any_cast<uint64>(value));
	}
	else if (value.type() == typeid(double)) {
		ppst->set_double(index, std::any_cast<double>(value));
	}
	else if (value.type() == typeid(float)) {
		ppst->set_double(index, std::any_cast<float>(value));
	}
	else if (value.type() == typeid(std::vector<uchar>)) {
		auto byte_vct = std::any_cast<std::vector<uchar>>(value);
		ppst->set_blob(index,(char*)byte_vct.data(),byte_vct.size());
	}
	else {
		throw ylib::exception("Unsupported types("+std::string(value.type().name()) + ")");
	}
}
/// <summary>
/// 查询数据字段值转JSON
/// </summary>
/// <param name="field"></param>
/// <param name="result"></param>
/// <param name="json"></param>
void setQueryResultValuetoJson(const mysql::field& field,mysql::result* result,ylib::json& json) {
	if (field.type_name == "int")
		json = result->get_int32(field.index);
	else if (field.type_name == "varchar")
		json = result->get_string(field.index);
	else if (field.type_name == "int unsigned")
		json = result->get_uint32(field.index);
	else if (field.type_name == "bigint")
		json = result->get_int64(field.index);
	else if (field.type_name == "datetime")
		json = result->get_string(field.index);
	else if (field.type_name == "tinyint")
		json = result->get_int32(field.index);
}
ylib::select::select(mysql::conn* conn):m_conn(conn)
{
}

ylib::select::~select()
{
	if (m_conn != nullptr)
	{
		m_conn->pool()->recover(m_conn);
	}
}

ylib::select& ylib::select::table(const std::string& table_name)
{
	m_table_name = table_name;
	return *this;
}

ylib::select& ylib::select::field(const std::vector<std::string>& field)
{
	m_fields = field;
	return *this;
}
ylib::select& ylib::select::where(const std::string& name, const std::string& expression, const std::any& value)
{
	ylib::where w;
	w.name = name;
	w.expression = expression;
	w.value = value;
	w.type = 0;
	if (w.value.has_value() == false)
	{
		throw ylib::exception("value is null");
	}
	if (checkAnyType(w.value) == false)
	{
		throw ylib::exception("Unsupported types");
	}
	m_wheres.push_back(w);
	return *this;
}

ylib::select& ylib::select::where_like(const std::string& name, const std::string& value)
{
	ylib::where w;
	w.name = name;
	w.value = value;
	w.type = 1;
	m_wheres.push_back(w);
	return *this;
}

ylib::select& ylib::select::where(const std::string& expression)
{
	ylib::where w;
	w.expression = expression;
	w.type = 2;
	m_wheres.push_back(w);
	return *this;
}
ylib::select& ylib::select::page(uint32 page, uint32 count)
{
	m_limit.start = (page-1)*count;
	m_limit.count =count;
	return *this;
}

ylib::select& ylib::select::limit(uint32 start, uint32 count)
{
	m_limit.start = start;
	m_limit.count = count;
	return *this;
}

ylib::select& ylib::select::orderby(const std::string& field, sort sort)
{
	m_orderby.field = field;
	m_orderby.sort = sort;
	return *this;
}
uint64 ylib::select::count()
{
	if (m_table_name.empty())
		throw ylib::exception("not found table_name");

	std::string field_name;
	std::string where;
	std::string orderby;
	std::string limit;
	std::vector<std::any> insert_values;
	make_sql(field_name, where, orderby, limit, insert_values);


	std::string sql = "SELECT COUNT(1) FROM " + m_table_name + " " + where + " " + orderby + " " + limit;
	auto ppst = m_conn->setsql(sql);
	for (size_t i = 0; i < insert_values.size(); i++)
		setInsetValue(ppst, i + 1, insert_values[i]);
	

	auto result = ppst->query();
	result->next();
	return result->get_uint64(1);
}
ylib::mysql::result* ylib::select::query()
{
	if (m_table_name.empty())
		throw ylib::exception("not found table_name");
	
	std::string field_name;
	std::string where;
	std::string orderby;
	std::string limit;
	std::vector<std::any> insert_values;
	make_sql(field_name,where, orderby,limit,insert_values);


	std::string sql = "SELECT "+field_name+" FROM "+m_table_name+" " + where + " " + orderby + " " + limit;
	auto ppst = m_conn->setsql(sql);
	for (size_t i = 0; i < insert_values.size(); i++)
		setInsetValue(ppst, i + 1, insert_values[i]);
	return ppst->query();
}
void ylib::select::clear()
{
	m_wheres.clear();
	m_table_name = "";
	m_fields.clear();
	m_limit.count = -1;
	m_limit.start = -1;
	m_orderby.field = "";
}
void ylib::select::make_sql(std::string& field_name, std::string& where, std::string& orderby, std::string& limit, std::vector<std::any>& insert_values)
{
	if (m_fields.size() == 0)
		field_name = "*";
	else
		field_name = std::accumulate(m_fields.begin(), m_fields.end(), std::string(), [](std::string a, const std::string& b) {return std::move(a) + (a.empty() ? "" : ",") + b; });

	if (m_wheres.size() != 0)
	{
		where = " WHERE 1=1";
		for (size_t i = 0; i < m_wheres.size(); i++)
		{
			where += " ";
			switch (m_wheres[i].type)
			{
			case 0:
				where += " AND " + m_wheres[i].name + " " + m_wheres[i].expression + " ?";
				insert_values.push_back(m_wheres[i].value);
				break;
			case 1:
				where += " AND " + m_wheres[i].name + " LIKE ?";
				insert_values.push_back(m_wheres[i].value);
				break;
			case 2:
				where += " " + m_wheres[i].expression;
				break;
			default:
				break;
			}
		}
	}

	if (m_limit.count != -1 && m_limit.start != -1)
	{
		limit = " LIMIT ?,?";
		insert_values.push_back(m_limit.start);
		insert_values.push_back(m_limit.count);
	}
	if (m_orderby.field.empty() == false)
	{
		orderby = " ORDER BY " + m_orderby.field + " " + std::string(m_orderby.sort == DESC ? "DESC" : "ASC");
	}
}

ylib::update::update(mysql::conn* conn):m_conn(conn)
{
}

ylib::update::~update()
{
	if (m_conn != nullptr)
	{
		m_conn->pool()->recover(m_conn);
	}
}

ylib::update& ylib::update::table(const std::string& table_name)
{
	this->m_table_name = table_name;
	return *this;
}

ylib::update& ylib::update::set(const std::string& name, const std::any& value)
{
	struct update::set s;
	s.type = 0;
	s.name = name;
	s.value = value;
	m_sets.push_back(s);
	return *this;
}

ylib::update& ylib::update::set(const std::string& expression)
{
	struct update::set s;
	s.type = 1;
	s.expression = expression;
	m_sets.push_back(s);
	return *this;
}

ylib::update& ylib::update::where(const std::string& name, const std::string& expression, const std::any& value)
{
	ylib::where w;
	w.name = name;
	w.expression = expression;
	w.value = value;
	w.type = 0;
	if (w.value.has_value() == false)
	{
		throw ylib::exception("value is null");
	}
	if (checkAnyType(w.value) == false)
	{
		throw ylib::exception("Unsupported types");
	}
	m_wheres.push_back(w);
	return *this;
}

ylib::update& ylib::update::where_like(const std::string& name, const std::string& value)
{
	ylib::where w;
	w.name = name;
	w.value = value;
	w.type = 1;
	m_wheres.push_back(w);
	return *this;
}

ylib::update& ylib::update::where(const std::string& expression)
{
	ylib::where w;
	w.expression = expression;
	w.type = 2;
	m_wheres.push_back(w);
	return *this;
}

ylib::update& ylib::update::page(uint32 page, uint32 count)
{
	m_limit.start = (page - 1) * count;
	m_limit.count = count;
	return *this;
}

ylib::update& ylib::update::limit(uint32 start, uint32 count)
{
	m_limit.start = start;
	m_limit.count = count;
	return *this;
}

ylib::update& ylib::update::orderby(const std::string& field, sort sort)
{
	m_orderby.field = field;
	m_orderby.sort = sort;
	return *this;
}

uint64 ylib::update::exec()
{
	if (m_table_name.empty())
		throw ylib::exception("not found table_name");

	std::string sets;
	std::string where;
	std::string orderby;
	std::string limit;
	std::vector<std::any> insert_values;
	make_sql(sets, where, orderby, limit, insert_values);


	std::string sql = "UPDATE " + m_table_name +" SET " + sets + " " + where + " " + orderby + " " + limit;
	auto ppst = m_conn->setsql(sql);
	for (size_t i = 0; i < insert_values.size(); i++)
		setInsetValue(ppst, i + 1, insert_values[i]);
	return ppst->update();
}

void ylib::update::clear()
{
	m_wheres.clear();
	m_table_name = "";
	m_limit.count = -1;
	m_limit.start = -1;
	m_orderby.field = "";
	m_sets.clear();
}

void ylib::update::make_sql(std::string& set, std::string& where, std::string& orderby, std::string& limit, std::vector<std::any>& insert_values)
{
	{
		for (size_t i = 0; i < m_sets.size(); i++)
		{
			set += " ";
			switch (m_sets[i].type)
			{
			case 0:
				set += " "+ m_sets[i].name + "=" +  " ?";
				insert_values.push_back(m_sets[i].value);
				break;

			case 1:
				set += " " + m_sets[i].expression;
				break;
			default:
				break;
			}
			if (i + 1 != m_sets.size())
				set += " , ";
		}
	}
	if (m_wheres.size() != 0)
	{
		where = " WHERE 1=1";
		for (size_t i = 0; i < m_wheres.size(); i++)
		{
			where += " ";
			switch (m_wheres[i].type)
			{
			case 0:
				where += " AND " + m_wheres[i].name + " " + m_wheres[i].expression + " ?";
				insert_values.push_back(m_wheres[i].value);
				break;
			case 1:
				where += " AND " + m_wheres[i].name + " LIKE ?";
				insert_values.push_back(m_wheres[i].value);
				break;
			case 2:
				where += " " + m_wheres[i].expression;
				break;
			default:
				break;
			}
		}
	}
	if (m_limit.count != -1 && m_limit.start != -1)
	{
		limit = " LIMIT ?,?";
		insert_values.push_back(m_limit.start);
		insert_values.push_back(m_limit.count);
	}
	if (m_orderby.field.empty() == false)
	{
		orderby = " ORDER BY " + m_orderby.field + " " + std::string(m_orderby.sort == DESC ? "DESC" : "ASC");
	}
}

ylib::insert::insert(mysql::conn* conn):m_conn(conn)
{
}

ylib::insert::~insert()
{
	if (m_conn != nullptr)
	{
		m_conn->pool()->recover(m_conn);
	}
}

ylib::insert& ylib::insert::table(const std::string& table_name)
{
	m_table_name = table_name;
	return *this;
}

ylib::insert& ylib::insert::set(const std::string& name, const std::any& value)
{
	m_sets.push_back({name, value});
	return *this;
}

ylib::insert& ylib::insert::set_not_pret(const std::string& name, const std::string& value)
{
	m_sets.push_back({ name, value,1 });
	return *this;
}

uint64 ylib::insert::exec()
{
	std::string sql = "INSERT INTO "+m_table_name+" (";
	for(size_t i=0;i<m_sets.size();i++)
	{
		sql += m_sets[i].name;
		if (i + 1 != m_sets.size())
			sql += ",";
	}
	sql += ") VALUES(";
	for (size_t i = 0; i < m_sets.size(); i++)
	{
		if (m_sets[i].extra == 0)
		{
			sql += "?";
		}
		else
		{
			sql += " " + std::any_cast<std::string>(m_sets[i].value) + " ";
		}
		if (i + 1 != m_sets.size())
			sql += ",";
	}
	sql += ")";

	auto ppst = m_conn->setsql(sql);
	uint32 index = 0;
	for (size_t i = 0; i < m_sets.size(); i++)
	{
		if (m_sets[i].extra != 0)
			continue;
		index++;
		setInsetValue(ppst, index, m_sets[i].value);
	}
		
	return ppst->update();
}

void ylib::insert::clear()
{
	m_table_name = "";
	m_sets.clear();

}





ylib::delete_::delete_(mysql::conn* conn) :m_conn(conn)
{
}

ylib::delete_::~delete_()
{
	if (m_conn != nullptr)
	{
		m_conn->pool()->recover(m_conn);
	}
}

ylib::delete_& ylib::delete_::table(const std::string& table_name)
{
	this->m_table_name = table_name;
	return *this;
}

ylib::delete_& ylib::delete_::where(const std::string& name, const std::string& expression, const std::any& value)
{
	ylib::where w;
	w.name = name;
	w.expression = expression;
	w.value = value;
	w.type = 0;
	if (w.value.has_value() == false)
	{
		throw ylib::exception("value is null");
	}
	if (checkAnyType(w.value) == false)
	{
		throw ylib::exception("Unsupported types");
	}
	m_wheres.push_back(w);
	return *this;
}

ylib::delete_& ylib::delete_::where_like(const std::string& name, const std::string& value)
{
	ylib::where w;
	w.name = name;
	w.value = value;
	w.type = 1;
	m_wheres.push_back(w);
	return *this;
}

ylib::delete_& ylib::delete_::where(const std::string& expression)
{
	ylib::where w;
	w.expression = expression;
	w.type = 2;
	m_wheres.push_back(w);
	return *this;
}

ylib::delete_& ylib::delete_::page(uint32 page, uint32 count)
{
	m_limit.start = (page - 1) * count;
	m_limit.count = count;
	return *this;
}

ylib::delete_& ylib::delete_::limit(uint32 start, uint32 count)
{
	m_limit.start = start;
	m_limit.count = count;
	return *this;
}

ylib::delete_& ylib::delete_::orderby(const std::string& field, sort sort)
{
	m_orderby.field = field;
	m_orderby.sort = sort;
	return *this;
}

uint64 ylib::delete_::exec()
{
	if (m_table_name.empty())
		throw ylib::exception("not found table_name");

	std::string where;
	std::string orderby;
	std::string limit;
	std::vector<std::any> insert_values;
	make_sql(where, orderby, limit, insert_values);


	std::string sql = "DELETE FROM " + m_table_name + where + " " + orderby + " " + limit;
	auto ppst = m_conn->setsql(sql);
	for (size_t i = 0; i < insert_values.size(); i++)
		setInsetValue(ppst, i + 1, insert_values[i]);
	return ppst->update();
}

void ylib::delete_::clear()
{
	m_wheres.clear();
	m_table_name = "";
	m_limit.count = -1;
	m_limit.start = -1;
	m_orderby.field = "";
}

void ylib::delete_::make_sql(std::string& where, std::string& orderby, std::string& limit, std::vector<std::any>& insert_values)
{
	if (m_wheres.size() != 0)
	{
		where = " WHERE 1=1";
		for (size_t i = 0; i < m_wheres.size(); i++)
		{
			where += " ";
			switch (m_wheres[i].type)
			{
			case 0:
				where += " AND " + m_wheres[i].name + " " + m_wheres[i].expression + " ?";
				insert_values.push_back(m_wheres[i].value);
				break;
			case 1:
				where += " AND " + m_wheres[i].name + " LIKE ?";
				insert_values.push_back(m_wheres[i].value);
				break;
			case 2:
				where += " " + m_wheres[i].expression;
				break;
			default:
				break;
			}
		}
	}
	if (m_limit.count != -1 && m_limit.start != -1)
	{
		limit = " LIMIT ?,?";
		insert_values.push_back(m_limit.start);
		insert_values.push_back(m_limit.count);
	}
	if (m_orderby.field.empty() == false)
	{
		orderby = " ORDER BY " + m_orderby.field + " " + std::string(m_orderby.sort == DESC ? "DESC" : "ASC");
	}
}

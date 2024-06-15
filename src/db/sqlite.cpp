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

#include "db/sqlite.h"
#include "sqlite3/sqlite3.h"
#include "util/strutils.h"
#include <sstream>

namespace ylib::sqlite
{
    result::result(void* handle) : m_handle(handle)
    {
        // 初始化列信息
        m_field_count = sqlite3_column_count(static_cast<sqlite3_stmt*>(m_handle));
        for (uint32 i = 0; i < m_field_count; ++i) {
            field f;
            f.index = i;
            f.name = sqlite3_column_name(static_cast<sqlite3_stmt*>(m_handle), i);
            f.type_name = strutils::change_case(sqlite3_column_decltype(static_cast<sqlite3_stmt*>(m_handle), i), false);
            m_fields.push_back(f);
        }
    }

    result::~result()
    {

    }

    std::string result::field_name(uint32 index)
    {
        return m_fields[index].name;
    }

    std::string result::field_type(uint32 index)
    {
        return m_fields[index].type_name;
    }

    std::string result::field_type(const std::string& name)
    {
        for (const auto& field : m_fields) {
            if (field.name == name) {
                return field.type_name;
            }
        }
        return "";
    }

    uint32 result::field_count()
    {
        return m_field_count;
    }

    bool result::next()
    {
        return sqlite3_step(static_cast<sqlite3_stmt*>(m_handle)) == SQLITE_ROW;
    }

    int32 result::get_int32(uint32 index)
    {
        return sqlite3_column_int(static_cast<sqlite3_stmt*>(m_handle), index);
    }

    int32 result::get_int32(const std::string& name)
    {
        for (const auto& field : m_fields) {
            if (field.name == name) {
                return get_int32(field.index);
            }
        }
        return 0;
    }

    int64 result::get_int64(uint32 index)
    {
        return sqlite3_column_int64(static_cast<sqlite3_stmt*>(m_handle), index);
    }

    int64 result::get_int64(const std::string& name)
    {
        for (const auto& field : m_fields) {
            if (field.name == name) {
                return get_int64(field.index);
            }
        }
        return 0;
    }

    std::string result::get_string(uint32 index)
    {
        const unsigned char* text = sqlite3_column_text(static_cast<sqlite3_stmt*>(m_handle), index);
        return text ? reinterpret_cast<const char*>(text) : "";
    }

    std::string result::get_string(const std::string& name)
    {
        for (const auto& field : m_fields) {
            if (field.name == name) {
                return get_string(field.index);
            }
        }
        return "";
    }


    double result::get_double(uint32 index)
    {
        return sqlite3_column_double(static_cast<sqlite3_stmt*>(m_handle), index);
    }

    double result::get_double(const std::string& name)
    {
        for (const auto& field : m_fields) {
            if (field.name == name) {
                return get_double(field.index);
            }
        }
        return 0.0;
    }

    //ylib::buffer result::get_blob(uint32 index)
    //{
    //    const void* blob = sqlite3_column_blob(static_cast<sqlite3_stmt*>(m_handle), index);
    //    int size = sqlite3_column_bytes(static_cast<sqlite3_stmt*>(m_handle), index);
    //    return ylib::buffer(static_cast<const char*>(blob), size);
    //}

    //ylib::buffer result::get_blob(const std::string& name)
    //{
    //    for (const auto& field : m_fields) {
    //        if (field.name == name) {
    //            return get_blob(field.index);
    //        }
    //    }
    //    return ylib::buffer();
    //}

    prepare_statement::prepare_statement() = default;

    prepare_statement::~prepare_statement()
    {
        clear();
        if (m_handle != nullptr) {
            sqlite3_finalize(static_cast<sqlite3_stmt*>(m_handle));
            m_handle = nullptr;
        }
    }

    void prepare_statement::set_bigint(uint32 index, const std::string& value)
    {
        sqlite3_bind_text(static_cast<sqlite3_stmt*>(m_handle), index, value.c_str(), -1, SQLITE_TRANSIENT);
    }
    void prepare_statement::set_double(uint32 index, double value)
    {
        sqlite3_bind_double(static_cast<sqlite3_stmt*>(m_handle), index, value);
    }

    void prepare_statement::set_int32(uint32 index, int32 value)
    {
        sqlite3_bind_int(static_cast<sqlite3_stmt*>(m_handle), index, value);
    }
    void prepare_statement::set_int64(uint32 index, int64 value)
    {
        sqlite3_bind_int64(static_cast<sqlite3_stmt*>(m_handle), index, value);
    }
    void prepare_statement::set_null(uint32 index)
    {
        sqlite3_bind_null(static_cast<sqlite3_stmt*>(m_handle), index);
    }

    void prepare_statement::set_string(uint32 index, const std::string& value)
    {
        sqlite3_bind_text(static_cast<sqlite3_stmt*>(m_handle), index, value.c_str(), -1, SQLITE_TRANSIENT);
    }

    void prepare_statement::set_string(uint32 index, const char* data, int size)
    {
        sqlite3_bind_text(static_cast<sqlite3_stmt*>(m_handle), index, data, size, SQLITE_TRANSIENT);
    }


    void prepare_statement::clear()
    {
        sqlite3_clear_bindings(static_cast<sqlite3_stmt*>(m_handle));
        if (m_result != nullptr)
        {
            delete m_result;
            m_result = nullptr;
        }
    }

    uint64 prepare_statement::update()
    {
        if (sqlite3_step(static_cast<sqlite3_stmt*>(m_handle)) == SQLITE_DONE) {
            return sqlite3_changes(sqlite3_db_handle(static_cast<sqlite3_stmt*>(m_handle)));
        }
        return 0;
    }

    ylib::sqlite::result* prepare_statement::query()
    {
        clear();

        if (sqlite3_step(static_cast<sqlite3_stmt*>(m_handle)) == SQLITE_ROW) {
            m_result =  new ylib::sqlite::result(m_handle);
            return m_result;
        }
        return nullptr;
    }

    conn::conn()
    {

    }
    conn::~conn()
    {
        close();
    }

    bool conn::start(const ylib::sqlite::sqlite_conn_info& info)
    {
        close();
        if (sqlite3_open(info.database.c_str(), reinterpret_cast<sqlite3**>(&m_handle)) != SQLITE_OK) 
        {
            if(m_handle != nullptr)
            {
                this->m_lastErrorDesc = sqlite3_errmsg(reinterpret_cast<sqlite3*>(m_handle));
                close();
            }
            return false;
        }
        m_info = info;
        return true;
    }

    void conn::close()
    {
        clear();
        if (m_handle != nullptr) {
            sqlite3_close(static_cast<sqlite3*>(m_handle));
            m_handle = nullptr;
        }
    }

    void conn::clear()
    {
        if (m_ppst) {
            delete m_ppst;
            m_ppst = nullptr;
        }
    }

    ylib::sqlite::prepare_statement* conn::setsql(const std::string& sql)
    {
        if (m_handle == nullptr)
            ylib::exception("database not open");

        clear();
        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(static_cast<sqlite3*>(m_handle), sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            m_lastErrorDesc = sqlite3_errmsg(static_cast<sqlite3*>(m_handle));
            return nullptr;
        }
        m_ppst = new ylib::sqlite::prepare_statement();
        m_ppst->m_handle = stmt;
        return m_ppst;
    }

    uint64 conn::insert_id()
    {
        if (m_handle == nullptr)
            ylib::exception("database not open");

        return sqlite3_last_insert_rowid(static_cast<sqlite3*>(m_handle));
    }

    bool conn::begin(bool autocommit)
    {
        if (m_sw == 1)
        {
            m_lastErrorDesc = "do not open transactions repeatedly";
            return false;
        }
        setsql(autocommit ? "BEGIN TRANSACTION;" : "BEGIN;");
        if (m_ppst == nullptr)
            return false;
        m_ppst->update();
        m_sw = 1;
        return true;
    }

    bool conn::commit()
    {
        if (m_sw == 1) {
            setsql("COMMIT;");
            if (m_ppst == nullptr)
                return false;
            m_ppst->update();
            m_sw = 2;
            return true;
        }
        else
        {
            m_lastErrorDesc = "transaction not opened";
            return false;
        }
            
    }

    bool conn::rollback()
    {
        if (m_sw == 1) {
            setsql("ROLLBACK;");
            if (m_ppst == nullptr)
                return false;
            m_ppst->update();
            m_sw = 2;
            return true;
        }
        else
        {
            m_lastErrorDesc = "transaction not opened";
            return false;
        }
        
    }
}

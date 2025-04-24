/*Software License

Copyright(C) 2024 [liuyingjie]

License Terms
Usage Rights

Any individual or entity is free to use, copy, and distribute the binary form of this software without modification to the source code, without the need to disclose the source code.
If the source code is modified, the modifications must be open-sourced under the same license. This means that the modifications must be disclosed and accompanied by a copy of this license.

Future Versions Updates

From this version onwards, all future releases will be governed by the terms of the latest version of the license. This license will automatically be nullified and replaced by the new version.
Users must comply with the terms of the new license issued in future releases.

Liability and Disclaimer

This software is provided “as is”, without any express or implied warranties, including but not limited to the warranties of merchantability, fitness for a particular purpose, and non-infringement. In no event shall the author or copyright holder be liable for any claims, damages, or other liabilities, whether in an action of contract, tort, or otherwise, arising from, out of, or in connection with the software or the use or other dealings in the software.

Contact Information

If you have any questions, please contact us: 1585346868@qq.com Or visit our website fwlua.com.
*/

#include "db/mysql.h"

#if defined(_WIN64) || defined(__linux__)

#include <cassert>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>

#include "util/strutils.h"
#include "cppconn/connection.h"
#include "cppconn/exception.h"
#include "cppconn/metadata.h"
#include "cppconn/prepared_statement.h"
#include "cppconn/resultset.h"
#include "cppconn/resultset_metadata.h"
#include "mysql_driver.h"

namespace ylib {
    namespace mysql {

        // 内部统一的错误包装
        static std::string make_throw_error(const char* msg) {
#ifdef _DEBUG
            assert(false && "SQL exception occurred");
            return "";
#else
            return std::string("[ylib->mysql->throw]: ") + (msg ? msg : "Unknown error");
#endif
        }

        conn::conn()
            : m_handle(nullptr), m_ppst(nullptr), m_sw(0) {
        }

        conn::~conn() {
            close();
        }

        EXAMPLE_START_RESULT conn::start(const mysql_conn_info& info) {
            if (info.ipaddress.empty()) {
                m_lastErrorDesc = "address is empty";
                return SR_FAILED;
            }
            if (info.username.empty()) {
                m_lastErrorDesc = "username is empty";
                return SR_FAILED;
            }
            if (info.password.empty()) {
                m_lastErrorDesc = "password is empty";
                return SR_FAILED;
            }
            if (info.charset.empty()) {
                m_lastErrorDesc = "charset is empty";
                return SR_FAILED;
            }
            // 新增端口合法性校验
            if (info.port < 1 || info.port > 65535) {
                m_lastErrorDesc = "port out of range";
                return SR_FAILED;
            }

            // 以下不变…
            m_info = info;
            sql::Driver* driver = get_driver_instance();
            sql::ConnectOptionsMap opts;
            opts["hostName"] = info.ipaddress;
            opts["userName"] = info.username;
            opts["password"] = info.password;
            if (!info.database.empty()) opts["schema"] = info.database;
            opts["port"] = static_cast<int>(info.port);
            opts["OPT_CONNECT_TIMEOUT"] = 3;
            opts["OPT_CHARSET_NAME"] = info.charset;
            opts["OPT_SSL_MODE"] = sql::SSL_MODE_DISABLED;
            try {
                m_handle = driver->connect(opts);
                return SR_SUCCESS;
            }
            catch (const sql::SQLException& e) {
                m_lastErrorDesc = e.what();
                return (m_lastErrorDesc.find("timeout") != std::string::npos)
                    ? SR_TIMEOUT : SR_FAILED;
            }
        }

        void conn::close() {
            if (m_ppst) {
                delete m_ppst;
                m_ppst = nullptr;
            }
            if (!m_handle) return;
            try {
                auto cn = static_cast<sql::Connection*>(m_handle);
                cn->close();
            }
            catch (const std::exception& e) {
                std::cerr << make_throw_error(e.what()) << std::endl;
            }
            delete static_cast<sql::Connection*>(m_handle);
            m_handle = nullptr;
        }

        void conn::recover() {
            if (m_sw == 1) {
                rollback();
            }
            if (m_ppst) {
                delete m_ppst;
                m_ppst = nullptr;
            }
        }
        void conn::clear() {
            if (m_ppst) {
                delete m_ppst;
                m_ppst = nullptr;
            }
        }
        void conn::task_out() {
            try {
                auto cn = static_cast<sql::Connection*>(m_handle);
                if (m_sw != 0 || !cn || !cn->isValid()) {
                    close();
                    if (start(m_info) != SR_SUCCESS) {
                        throw exception(make_throw_error(m_lastErrorDesc.c_str()));
                    }
                    m_sw = 0;
                }
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }
        prepare_statement* conn::setsql(const std::string& sql) {
            if (!m_handle) throw exception("Invalid SQL connection");
            // 新增空字符串检查
            if (sql.empty()) throw exception("SQL string is empty");

            if (m_ppst) {
                delete m_ppst;
                m_ppst = nullptr;
            }
            m_ppst = new prepare_statement();
            try {
                auto cn = static_cast<sql::Connection*>(m_handle);
                m_ppst->m_handle = cn->prepareStatement(sql);
            }
            catch (const sql::SQLException& e) {
                delete m_ppst;
                m_ppst = nullptr;
                throw exception(make_throw_error(e.what()));
            }
            return m_ppst;
        }


        uint64 conn::insert_id() {
            if (!m_handle) throw exception("Invalid SQL connection");
            if (m_ppst) {
                delete m_ppst;
                m_ppst = nullptr;
            }
            m_ppst = new prepare_statement();
            uint64 lastId = 0;
            try {
                auto cn = static_cast<sql::Connection*>(m_handle);
                m_ppst->m_handle = cn->prepareStatement("SELECT LAST_INSERT_ID()");
                auto rs = m_ppst->query();
                if (rs->next()) {
                    lastId = rs->get_uint64(1);
                }
            }
            catch (const sql::SQLException& e) {
                delete m_ppst;
                m_ppst = nullptr;
                throw exception(make_throw_error(e.what()));
            }
            return lastId;
        }

        void conn::begin(bool autocommit) {
            if (!m_handle) throw exception("Invalid SQL connection");
            try {
                auto cn = static_cast<sql::Connection*>(m_handle);
                cn->setAutoCommit(autocommit);
                m_sw = 1;
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }

        void conn::commit() {
            if (!m_handle) throw exception("Invalid SQL connection");
            try {
                auto cn = static_cast<sql::Connection*>(m_handle);
                cn->commit();
                m_sw = 2;
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }

        void conn::rollback() {
            if (!m_handle) throw exception("Invalid SQL connection");
            try {
                auto cn = static_cast<sql::Connection*>(m_handle);
                cn->rollback();
                m_sw = 2;
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }

        void conn::setDatabase(const std::string& name) {
            if (!m_handle) throw exception("Invalid SQL connection");
            try {
                auto cn = static_cast<sql::Connection*>(m_handle);
                cn->setSchema(name);
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }


        // ---------- prepare_statement 实现 ----------

        prepare_statement::prepare_statement()
            : m_handle(nullptr), m_result(nullptr) {
        }

        prepare_statement::~prepare_statement() {
            if (m_result) {
                delete m_result;
                m_result = nullptr;
            }
            if (m_handle) {
                try {
                    auto ps = static_cast<sql::PreparedStatement*>(m_handle);
                    while (ps->getMoreResults()) {
                        auto rs = ps->getResultSet();
                        if (rs) rs->close();
                    }
                    ps->close();
                }
                catch (const sql::SQLException& e) {
                    std::cerr << make_throw_error(e.what()) << std::endl;
                }
                delete static_cast<sql::PreparedStatement*>(m_handle);
                m_handle = nullptr;
            }
        }

#define ENSURE_PS() if (!m_handle) throw exception("Invalid PreparedStatement")

        void prepare_statement::set_bigint(uint32 idx, const std::string& val) {
            ENSURE_PS();
            static_cast<sql::PreparedStatement*>(m_handle)->setBigInt(idx, val);
        }

        void prepare_statement::set_boolean(uint32 idx, bool val) {
            ENSURE_PS();
            static_cast<sql::PreparedStatement*>(m_handle)->setBoolean(idx, val);
        }

        void prepare_statement::set_datetime(uint32 idx, const std::string& val) {
            ENSURE_PS();
            auto ps = static_cast<sql::PreparedStatement*>(m_handle);
            if (val.empty())
                ps->setNull(idx, sql::DataType::DATE);
            else
                ps->setDateTime(idx, val);
        }

        void prepare_statement::set_double(uint32 idx, double val) {
            ENSURE_PS();
            static_cast<sql::PreparedStatement*>(m_handle)->setDouble(idx, val);
        }

        void prepare_statement::set_int32(uint32 idx, int32 val) {
            ENSURE_PS();
            static_cast<sql::PreparedStatement*>(m_handle)->setInt(idx, val);
        }

        void prepare_statement::set_uint32(uint32 idx, uint32 val) {
            ENSURE_PS();
            static_cast<sql::PreparedStatement*>(m_handle)->setUInt(idx, val);
        }

        void prepare_statement::set_int64(uint32 idx, int64 val) {
            ENSURE_PS();
            static_cast<sql::PreparedStatement*>(m_handle)->setInt64(idx, val);
        }

        void prepare_statement::set_uint64(uint32 idx, uint64 val) {
            ENSURE_PS();
            static_cast<sql::PreparedStatement*>(m_handle)->setUInt64(idx, val);
        }

        void prepare_statement::set_null(uint32 idx) {
            ENSURE_PS();
            static_cast<sql::PreparedStatement*>(m_handle)
                ->setNull(idx, sql::DataType::VARBINARY);
        }

        void prepare_statement::set_string(uint32 idx, const std::string& val) {
            ENSURE_PS();
            static_cast<sql::PreparedStatement*>(m_handle)
                ->setString(idx, sql::SQLString(val));
        }

        void prepare_statement::set_string(uint32 idx, const char* data, int size) {
            ENSURE_PS();
            static_cast<sql::PreparedStatement*>(m_handle)
                ->setString(idx, sql::SQLString(data, size));
        }

        void prepare_statement::set_blob(uint32 idx, const char* data, int size) {
            ENSURE_PS();
            auto buf = std::make_shared<std::istringstream>(std::string(data, size));
            m_blobs.push(buf);
            static_cast<sql::PreparedStatement*>(m_handle)->setBlob(idx, buf.get());
        }

        void prepare_statement::clear() {
            if (m_result) {
                delete m_result;
                m_result = nullptr;
            }
            // 清空所有未释放的 blob 数据
            m_blobs = std::queue<std::shared_ptr<std::istringstream>>();
        }


        uint64 prepare_statement::update() {
            ENSURE_PS();
            clear();
            try {
                return static_cast<sql::PreparedStatement*>(m_handle)
                    ->executeUpdate();
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }

        ylib::mysql::result* prepare_statement::query() {
            ENSURE_PS();
            clear();
            try {
                auto rs = static_cast<sql::PreparedStatement*>(m_handle)
                    ->executeQuery();
                m_result = new result(rs);
                return m_result;
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }

#undef ENSURE_PS


        // ---------- result 实现 ----------

        result::result(void* handle)
            : m_handle(handle) {
            auto rs = static_cast<sql::ResultSet*>(m_handle);
            // 直接用普通指针获取，不做 delete
            sql::ResultSetMetaData* md = rs->getMetaData();
            if (!md) return;
            uint32 cnt = md->getColumnCount();
            for (uint32 i = 1; i <= cnt; ++i) {
                field f;
                f.name = md->getColumnLabel(i);
                f.type_name = strutils::change_case(
                    md->getColumnTypeName(i), false);
                f.index = i - 1;
                m_fields.push_back(std::move(f));
            }
        }
        result::~result() {
            if (m_handle) {
                auto rs = static_cast<sql::ResultSet*>(m_handle);
                try {
                    rs->close();
                }
                catch (const std::exception& e) {
                    std::cerr << make_throw_error(e.what()) << std::endl;
                }
                delete rs;
                m_handle = nullptr;
            }
        }

#define ENSURE_RS() if (!m_handle) throw exception("Invalid ResultSet")

        std::string result::field_name(uint32 idx) {
            if (idx < 1 || idx > m_fields.size()) return "";
            return m_fields[idx - 1].name;
        }

        std::string result::field_type(uint32 idx) {
            if (idx < 1 || idx > m_fields.size()) return "";
            return m_fields[idx - 1].type_name;
        }

        std::string result::field_type(const std::string& name) {
            for (auto& f : m_fields) {
                if (f.name == name) return f.type_name;
            }
            throw exception("Field not found: " + name);
        }

        uint32 result::field_count() {
            return static_cast<uint32>(m_fields.size());
        }

        size_t result::row_count() {
            ENSURE_RS();
            try {
                return static_cast<sql::ResultSet*>(m_handle)->rowsCount();
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }

        bool result::next() {
            ENSURE_RS();
            try {
                return static_cast<sql::ResultSet*>(m_handle)->next();
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }

        int32 result::get_int32(uint32 idx) {
            ENSURE_RS();
            try {
                return static_cast<sql::ResultSet*>(m_handle)->getInt(idx);
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }

        int32 result::get_int32(const std::string& name) {
            ENSURE_RS();
            try {
                return static_cast<sql::ResultSet*>(m_handle)
                    ->getInt(name.c_str());
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }

        uint32 result::get_uint32(uint32 idx) {
            ENSURE_RS();
            try {
                return static_cast<sql::ResultSet*>(m_handle)->getUInt(idx);
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }

        uint32 result::get_uint32(const std::string& name) {
            ENSURE_RS();
            try {
                return static_cast<sql::ResultSet*>(m_handle)
                    ->getUInt(name.c_str());
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }

        int64 result::get_int64(uint32 idx) {
            ENSURE_RS();
            try {
                return static_cast<sql::ResultSet*>(m_handle)->getInt64(idx);
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }

        int64 result::get_int64(const std::string& name) {
            ENSURE_RS();
            try {
                return static_cast<sql::ResultSet*>(m_handle)
                    ->getInt64(name.c_str());
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }

        uint64 result::get_uint64(uint32 idx) {
            ENSURE_RS();
            try {
                return static_cast<sql::ResultSet*>(m_handle)->getUInt64(idx);
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }

        uint64 result::get_uint64(const std::string& name) {
            ENSURE_RS();
            try {
                return static_cast<sql::ResultSet*>(m_handle)
                    ->getUInt64(name.c_str());
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }

        std::string result::get_string(uint32 idx) {
            ENSURE_RS();
            try {
                return static_cast<sql::ResultSet*>(m_handle)
                    ->getString(idx).c_str();
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }

        std::string result::get_string(const std::string& name) {
            ENSURE_RS();
            try {
                return static_cast<sql::ResultSet*>(m_handle)
                    ->getString(name.c_str()).c_str();
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }

        bool result::get_boolean(uint32 idx) {
            ENSURE_RS();
            try {
                return static_cast<sql::ResultSet*>(m_handle)->getBoolean(idx);
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }

        bool result::get_boolean(const std::string& name) {
            ENSURE_RS();
            try {
                return static_cast<sql::ResultSet*>(m_handle)
                    ->getBoolean(name.c_str());
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }

        double result::get_double(uint32 idx) {
            ENSURE_RS();
            try {
                return static_cast<sql::ResultSet*>(m_handle)->getDouble(idx);
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }

        double result::get_double(const std::string& name) {
            ENSURE_RS();
            try {
                return static_cast<sql::ResultSet*>(m_handle)
                    ->getDouble(name.c_str());
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }

        ylib::buffer result::get_blob(uint32 idx) {
            ENSURE_RS();
            try {
                auto stream = static_cast<sql::ResultSet*>(m_handle)->getBlob(idx);
                ylib::buffer buf;
                char ch;
                while (stream->get(ch)) buf.append<char>(ch);
                return buf;
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }

        ylib::buffer result::get_blob(const std::string& name) {
            ENSURE_RS();
            try {
                auto stream = static_cast<sql::ResultSet*>(m_handle)
                    ->getBlob(name.c_str());
                ylib::buffer buf;
                char ch;
                while (stream->get(ch)) buf.append<char>(ch);
                return buf;
            }
            catch (const sql::SQLException& e) {
                throw exception(make_throw_error(e.what()));
            }
        }

#undef ENSURE_RS

    } // namespace mysql
} // namespace ylib

#endif // _WIN64 || __linux__

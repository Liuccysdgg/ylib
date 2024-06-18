#pragma once

#include <vector>
#include <string>
#include <queue>
#include <memory>
#ifndef _WIN32 
#include <stddef.h>
#endif

#include "base/define.h"
#include "base/error.h"
#include "util/json.h"

namespace ylib::sqlite
{
    class result;
    class conn;
    class prepare_statement;

    struct field {
        uint32 index = 0;
        std::string name;
        std::string type_name;
    };

    struct sqlite_conn_info
    {
        sqlite_conn_info() = default;
        std::string database;
    };

    class result : public ylib::error_base
    {
    public:
        result(void* handle);
        ~result();
        // 列名
        std::string field_name(uint32 index);
        // 列类型
        std::string field_type(uint32 index);
        std::string field_type(const std::string& name);
        // 列数量 
        uint32 field_count();
        // 下一行
        bool next();
        int32 get_int32(uint32 index);
        int32 get_int32(const std::string& name);


        int64 get_int64(uint32 index);
        int64 get_int64(const std::string& name);


        std::string get_string(uint32 index);
        std::string get_string(const std::string& name);


        double get_double(uint32 index);
        double get_double(const std::string& name);

        //ylib::buffer get_blob(uint32 index);
        //ylib::buffer get_blob(const std::string& name);
    private:
        void* m_handle = nullptr;
        uint32 m_field_count = 0;
        std::vector<ylib::sqlite::field> m_fields;
    };

    class prepare_statement : public ylib::error_base {
    public:
        prepare_statement();
        ~prepare_statement();
        void set_bigint(uint32 index, const std::string& value);
        void set_double(uint32 index, double value);
        void set_int32(uint32 index, int32 value);
        void set_int64(uint32 index, int64 value);
        void set_null(uint32 index);
        void set_string(uint32 index, const std::string& value);
        void set_string(uint32 index, const char* data, int size);
        void clear();
        uint64 update();
        ylib::sqlite::result* query();

        friend class ylib::sqlite::conn;
    private:
        ylib::sqlite::result* m_result = nullptr;
        void* m_handle = nullptr;

        std::queue<std::shared_ptr<std::istringstream>> m_blobs;
    };

    class conn : public ylib::error_base
    {
    public:
        conn();
        ~conn();
        bool start(const ylib::sqlite::sqlite_conn_info& info);
        void close();
        void clear();
        ylib::sqlite::prepare_statement* setsql(const std::string& sql);
        uint64 insert_id();
        bool begin(bool autocommit = false);
        bool commit();
        bool rollback();
    private:
        void* m_handle = nullptr;
        ylib::sqlite::sqlite_conn_info m_info;
        ylib::sqlite::prepare_statement *m_ppst = nullptr;
        // 事务状态 0=未开启 1=已开启 2=执行完毕
        int m_sw = 0;
    };
}

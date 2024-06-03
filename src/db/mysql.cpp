#include "db/mysql.h"
#if defined(_WIN64) || defined(__linux__)
#include "cppconn/driver.h"
#include "cppconn/connection.h"
#include "cppconn/statement.h"
#include "cppconn/prepared_statement.h"
#include "cppconn/resultset.h"
#include "cppconn/metadata.h"
#include "cppconn/resultset_metadata.h"
#include "cppconn/exception.h"
#include "cppconn/warning.h"

#include "util/strutils.h"
#define CHECK_SQL_CONNECTION if(m_handle == nullptr){throw ylib::exception("current connection is invalid");}
#define CHECK_SQL_PPST if(m_handle == nullptr){throw ylib::exception("current preparedstatement is invalid");}
#define CHECK_SQL_RESULT if(m_handle == nullptr){throw ylib::exception("current resultset is invalid");}

#define CONNECTION ((sql::Connection*)m_handle)
#define PREPARE_STATEMENT ((sql::PreparedStatement*)m_handle)
#define RESULT_SET ((sql::ResultSet*)m_handle)
#define OPTS (*(sql::ConnectOptionsMap*)m_opts)



#define DEBUG_LOG_PPST_SET 1
#define DEBUG_LOG_SETSQL 1
ylib::mysql::conn::conn()
{
    m_handle = nullptr;
    m_ppst = nullptr;
}

ylib::mysql::conn::~conn()
{
    close();
}
EXAMPLE_START_RESULT ylib::mysql::conn::start(const mysql_conn_info &info)
{
    if (info.ipaddress.empty())
    {
        m_lastErrorDesc = "address is empty";
        return SR_FAILED;
    }
    if (info.username.empty())
    {
        m_lastErrorDesc = "username is empty";
        return SR_FAILED;
    }
    if (info.password.empty())
    {
        m_lastErrorDesc = "password is empty";
        return SR_FAILED;
    }
    if (info.charset.empty())
    {
        m_lastErrorDesc = "charset is empty";
        return SR_FAILED;
    }
    m_info = info;
    sql::Driver* driver = get_driver_instance();
    // Configure Connect info
    sql::ConnectOptionsMap opts;
    opts["hostName"] = m_info.ipaddress.c_str();
    opts["userName"] = m_info.username.c_str();
    opts["password"] = m_info.password.c_str();
    if(m_info.database.empty() == false)
        opts["schema"] = m_info.database.c_str();

    opts["port"] = (int)m_info.port;
    opts["OPT_CONNECT_TIMEOUT"] = 3;
    opts["OPT_CHARSET_NAME"] = m_info.charset.c_str();
    opts["OPT_SSL_MODE"] = sql::SSL_MODE_DISABLED;
    //    opts["OPT_RECONNECT"] = sql::ConnectPropertyVal(true);
    sql::Connection* conn = nullptr;
    try
    {
        conn = driver->connect(opts);
        m_handle = conn;
        return EXAMPLE_START_RESULT::SR_SUCCESS;
    }
    catch (const sql::SQLException& e)
    {
        if (conn != nullptr)
            delete conn;
        m_lastErrorDesc = e.what();
    }
    if (m_lastErrorDesc.find("timeout") == -1)
        return EXAMPLE_START_RESULT::SR_FAILED;
    else
        return EXAMPLE_START_RESULT::SR_TIMEOUT;
}

void ylib::mysql::conn::close()
{
    if (CONNECTION == nullptr)
        return;
    try
    {
        CONNECTION->close();
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    delete CONNECTION;
    m_handle = nullptr;

    
}

void ylib::mysql::conn::recover()
{
    if (m_sw == 1)
    {
        rollback();
    }
    clear();

}

void ylib::mysql::conn::task_out()
{
    try
    {
        if(m_sw != 0 || CONNECTION->isValid() == false) {
            close();
            if (start(m_info) == false)
            {
                throw ylib::exception(this->last_error());
            }
            m_sw = 0;
        }
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
    }
}

void ylib::mysql::conn::clear()
{
    if(this->m_ppst != nullptr)
        delete this->m_ppst;

    this->m_ppst = nullptr;
}

ylib::mysql::prepare_statement* ylib::mysql::conn::setsql(const std::string &sql)
{
#if DEBUG_LOG_SETSQL == 1
    std::cout << "setsql : " << sql << std::endl;
#endif
    CHECK_SQL_CONNECTION;
    clear();
    m_ppst = new prepare_statement;
    try
    {
        m_ppst->m_handle = CONNECTION->prepareStatement(sql.c_str());
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
    }
    return m_ppst;
}
uint64 ylib::mysql::conn::insert_id()
{
    CHECK_SQL_CONNECTION;
    clear();
    m_ppst = new prepare_statement;
    uint64 last_insert_id = 0;
    try
    {
        m_ppst->m_handle = CONNECTION->prepareStatement("SELECT LAST_INSERT_ID()");
        auto result = m_ppst->query();
        result->next();
        last_insert_id =  result->get_uint64(1);
    }
    catch (const sql::SQLException& e)
    {
        throw ylib::exception(e.what());
    }
    return last_insert_id;
}
void ylib::mysql::conn::begin(bool autocommit)
{
    try
    {
        CONNECTION->setAutoCommit(autocommit);
        m_sw = 1;
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
        //std::cout<<e.what()<<std::endl;
    }
}

void ylib::mysql::conn::commit()
{
    try
    {
        CONNECTION->commit();
        m_sw = 2;
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
        //std::cout<<e.what()<<std::endl;
    }
}

void ylib::mysql::conn::rollback()
{
    try
    {
        CONNECTION->rollback();
        m_sw = 2;
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
        //std::cout<<e.what()<<std::endl;
    }
}
void ylib::mysql::conn::setDatabase(const std::string& name)
{
    try
    {
        CONNECTION->setSchema(name);
    }
    catch (const sql::SQLException& e)
    {
        throw ylib::exception(e.what());
    }
}
ylib::mysql::prepare_statement::prepare_statement()
{
    m_result = nullptr;
    m_handle = nullptr;
}

ylib::mysql::prepare_statement::~prepare_statement()
{
    clear();
    if(PREPARE_STATEMENT != nullptr)
    {
        try
        {
            while (PREPARE_STATEMENT->getMoreResults()) {
                PREPARE_STATEMENT->getResultSet()->close();
            }
            PREPARE_STATEMENT->close();
            delete PREPARE_STATEMENT;
        }
        catch (const sql::SQLException & e)
        {
            std::cout<<e.what()<<std::endl;
        }
    }
}
#if 0
#define PRINT_DEBUG_SET std::cout<<__FUNCTION__<<"["<<index<<"]: "<<value<<std::endl
#define PRINT_DEBUG_SET_std::string std::cout<<__FUNCTION__<<"["<<index<<"]: "<<value.c_str()<<std::endl
#else
#define PRINT_DEBUG_SET
#define PRINT_DEBUG_SET_NSTRING
#endif
void ylib::mysql::prepare_statement::set_bigint(uint32 index, const std::string &value)
{

    CHECK_SQL_PPST;
    PRINT_DEBUG_SET_NSTRING;
    PREPARE_STATEMENT->setBigInt(index,value.c_str());
#if DEBUG_LOG_PPST_SET == 1
    std::cout << "set " << index << " = " << value << std::endl;
#endif
    //PREPARE_STATEMENT->insert
}

void ylib::mysql::prepare_statement::set_boolean(uint32 index, bool value)
{
    CHECK_SQL_PPST;
    PRINT_DEBUG_SET;
    PREPARE_STATEMENT->setBoolean(index,value);
#if DEBUG_LOG_PPST_SET == 1
    std::cout << "set " << index << " = " << value << std::endl;
#endif
}

void ylib::mysql::prepare_statement::set_datetime(uint32 index, const std::string &value)
{
    CHECK_SQL_PPST;
    PRINT_DEBUG_SET_NSTRING;
    if (value.empty())
    {
        PREPARE_STATEMENT->setNull(index, sql::DataType::DATE);
    }
    else
    {
        PREPARE_STATEMENT->setDateTime(index, value.c_str());
    }
#if DEBUG_LOG_PPST_SET == 1
    std::cout << "set " << index << " = " << value << std::endl;
#endif
}

void ylib::mysql::prepare_statement::set_double(uint32 index, double value)
{
    CHECK_SQL_PPST;
    PRINT_DEBUG_SET;
    PREPARE_STATEMENT->setDouble(index,value);
#if DEBUG_LOG_PPST_SET == 1
    std::cout << "set " << index << " = " << value << std::endl;
#endif
}

void ylib::mysql::prepare_statement::set_int32(uint32 index, int32 value)
{
    CHECK_SQL_PPST;
    PRINT_DEBUG_SET;
    PREPARE_STATEMENT->setInt(index,value);
#if DEBUG_LOG_PPST_SET == 1
    std::cout << "set " << index << " = " << value << std::endl;
#endif
}

void ylib::mysql::prepare_statement::set_uint32(uint32 index, uint32 value)
{
    CHECK_SQL_PPST;
    PRINT_DEBUG_SET;
    PREPARE_STATEMENT->setUInt(index,value);
#if DEBUG_LOG_PPST_SET == 1
    std::cout << "set " << index << " = " << value << std::endl;
#endif
}

void ylib::mysql::prepare_statement::set_int64(uint32 index, int64 value)
{
    CHECK_SQL_PPST;
    PRINT_DEBUG_SET;
    PREPARE_STATEMENT->setInt64(index,value);
#if DEBUG_LOG_PPST_SET == 1
    std::cout << "set " << index << " = " << value << std::endl;
#endif
}

void ylib::mysql::prepare_statement::set_uint64(uint32 index, uint64 value)
{
    CHECK_SQL_PPST;
    PRINT_DEBUG_SET;
    PREPARE_STATEMENT->setUInt64(index,value);
#if DEBUG_LOG_PPST_SET == 1
    std::cout << "set " << index << " = " << value << std::endl;
#endif
}

void ylib::mysql::prepare_statement::set_null(uint32 index)
{
    throw ylib::exception("Not supported temporarily");
    //PREPARE_STATEMENT->setNull(index,value);
}

void ylib::mysql::prepare_statement::set_string(uint32 index, const std::string &value)
{
    CHECK_SQL_PPST;
    PRINT_DEBUG_SET_NSTRING;
    PREPARE_STATEMENT->setString(index,value.c_str());
#if DEBUG_LOG_PPST_SET == 1
    std::cout << "set " << index << " = " << value << std::endl;
#endif
}
void ylib::mysql::prepare_statement::set_blob(uint32 index, const ylib::buffer& value)
{
    CHECK_SQL_PPST;
    PRINT_DEBUG_SET_NSTRING;
    std::istringstream binaryDataStream(value.to_string());
    PREPARE_STATEMENT->setBlob(index, &binaryDataStream);
#if DEBUG_LOG_PPST_SET == 1
    std::cout << "set " << index << " = " << value.length()<<"(BLOB)" << std::endl;
#endif
}

void ylib::mysql::prepare_statement::clear()
{
    if(m_result != nullptr)
    {
        delete m_result;
    }
    m_result = nullptr;
}

uint64 ylib::mysql::prepare_statement::update()
{
    CHECK_SQL_PPST;
    clear();
    try
    {
        return PREPARE_STATEMENT->executeUpdate();
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
    }
}

ylib::mysql::result *ylib::mysql::prepare_statement::query()
{
    CHECK_SQL_PPST;
    clear();
    try
    {
        m_result = new ylib::mysql::result(PREPARE_STATEMENT->executeQuery());
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
    }
    return m_result;
}

ylib::mysql::result::result(void* handle):m_handle(handle)
{
    for (uint32 i = 0; i < RESULT_SET->getMetaData()->getColumnCount(); i++)
    {
        ylib::mysql::field field;
        field.name = RESULT_SET->getMetaData()->getColumnName(i + 1).c_str();;
        field.type_name = strutils::change_case(RESULT_SET->getMetaData()->getColumnTypeName(i + 1).c_str(), false);
        field.index = i;
        m_fields.push_back(field);
    }
}

ylib::mysql::result::~result()
{
    if(RESULT_SET != nullptr)
    {
        try
        {
            while (RESULT_SET->next())
                RESULT_SET->close();
        }
        catch(const std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }

        delete RESULT_SET;
    }

}

std::string ylib::mysql::result::field_name(uint32 index)
{
    if (index>m_fields.size() || index < 1)
        return "";
    return m_fields[index-1].name;
}

std::string ylib::mysql::result::field_type(uint32 index)
{
    if (index  > m_fields.size() || index < 1)
        return "";
    return m_fields[index-1].type_name;
}

std::string ylib::mysql::result::field_type(const std::string& name)
{
    for (size_t i = 0; i < m_fields.size(); i++)
    {
        if (m_fields[i].name == name)
            return m_fields[i].type_name;
    }
    throw ylib::exception("not found field: "+name);
}

uint32 ylib::mysql::result::field_count()
{
    CHECK_SQL_PPST;
    try
    {
        return RESULT_SET->getMetaData()->getColumnCount();
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
    }
}

size_t ylib::mysql::result::row_count()
{
    CHECK_SQL_PPST;
    try
    {
        return RESULT_SET->rowsCount();
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
    }

}

bool ylib::mysql::result::next()
{
    CHECK_SQL_PPST;
    try
    {
        return RESULT_SET->next();
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
    }
}

int32 ylib::mysql::result::get_int32(uint32 index)
{
    CHECK_SQL_PPST;
    try
    {
        return RESULT_SET->getInt(index);
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
    }
}

int32 ylib::mysql::result::get_int32(const std::string &name)
{
    CHECK_SQL_PPST;
    try
    {
        return RESULT_SET->getInt(name.c_str());
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
    }
}

uint32 ylib::mysql::result::get_uint32(uint32 index)
{
    CHECK_SQL_PPST;
    try
    {
        return RESULT_SET->getUInt(index);
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
    }
}

uint32 ylib::mysql::result::get_uint32(const std::string &name)
{
    CHECK_SQL_PPST;
    try
    {
        return RESULT_SET->getUInt(name.c_str());
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
    }
}

int64 ylib::mysql::result::get_int64(uint32 index)
{
    CHECK_SQL_PPST;
    try
    {
        return RESULT_SET->getInt64(index);
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
    }
}

int64 ylib::mysql::result::get_int64(const std::string &name)
{
    CHECK_SQL_PPST;
    try
    {
        return RESULT_SET->getInt64(name.c_str());
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
    }
}

uint64 ylib::mysql::result::get_uint64(uint32 index)
{
    CHECK_SQL_PPST;
    try
    {
        return RESULT_SET->getUInt64(index);
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
    }
}

uint64 ylib::mysql::result::get_uint64(const std::string &name)
{
    CHECK_SQL_PPST;
    try
    {
        return RESULT_SET->getUInt64(name.c_str());
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
    }
}

std::string ylib::mysql::result::get_string(uint32 index)
{
    CHECK_SQL_PPST;
    try
    {
        return RESULT_SET->getString(index).c_str();
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
    }
}

std::string ylib::mysql::result::get_string(const std::string &name)
{
    CHECK_SQL_PPST;
    try
    {
        return RESULT_SET->getString(name.c_str()).c_str();
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
    }
}

bool ylib::mysql::result::get_boolean(uint32 index)
{
    CHECK_SQL_PPST;
    try
    {
        return RESULT_SET->getBoolean(index);
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
    }
}

bool ylib::mysql::result::get_boolean(const std::string &name)
{
    CHECK_SQL_PPST;
    try
    {
        return RESULT_SET->getBoolean(name.c_str());
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
    }
}

double ylib::mysql::result::get_double(uint32 index)
{
    CHECK_SQL_PPST;
    try
    {
        return RESULT_SET->getDouble(index);
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
    }
}

double ylib::mysql::result::get_double(const std::string &name)
{
    CHECK_SQL_PPST;
    try
    {
        return RESULT_SET->getDouble(name.c_str());
    }
    catch (const sql::SQLException & e)
    {
        throw ylib::exception(e.what());
    }
}

ylib::json ylib::mysql::result::to_json()
{
    CHECK_SQL_PPST;
    struct __Column
    {
        std::string name;
        std::string type_name;
    };
    std::map<int, __Column> column_names;
    ylib::json result;
    try
    {
        // 取列名称数组
        {
            for (uint32 i = 0; i < RESULT_SET->getMetaData()->getColumnCount(); i++)
            {
                __Column c;
                c.name = RESULT_SET->getMetaData()->getColumnName(i+1).c_str();;
                c.type_name = strutils::change_case(RESULT_SET->getMetaData()->getColumnTypeName(i+1).c_str(), false);
                column_names[i] = c;
                std::cout << c.name.c_str() << "\t" << c.type_name.c_str() << std::endl;
            }
                
        }
        for (uint32 i = 0; i < row_count(); i++)
        {
            ylib::json line;
            next();
            for (uint32 c = 0; c < field_count(); c++)
            {
                if (column_names[c].type_name == "int")
                    line[column_names[c].name] = get_int32(c+1);
                else if (column_names[c].type_name == "varchar")
                    line[column_names[c].name] = get_string(c+1);
                else if (column_names[c].type_name == "int unsigned")
                    line[column_names[c].name] = get_uint32(c+1);
                else if (column_names[c].type_name == "bigint")
                    line[column_names[c].name] = get_int64(c+1);
                else
                    abort();
            }
            result.push_back(line);
        }
        
        //return RESULT_SET->getDouble(name.c_str());
    }
    catch (const sql::SQLException& e)
    {
        throw ylib::exception(e.what());
    }
    return result;
}
#endif
#include "db/mssql.h"
#ifdef _WIN32
#include "util/time.h"
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include "util/strutils.h"
#define CHECK_SQL_CONNECTION if(m_handle == nullptr){throw ylib::exception("current connection is invalid");}
#define CHECK_SQL_PPST if(m_handle == nullptr){throw ylib::exception("current preparedstatement is invalid");}
#define CHECK_SQL_RESULT if(m_handle == nullptr){throw ylib::exception("current resultset is invalid");}

#define THROW_RET(RESULT,MSG)   \
{                                                                                                               \
    auto __ret = RESULT;      \
    if (__ret != SQL_SUCCESS && __ret != SQL_SUCCESS_WITH_INFO) \
    {                                                                                                      \
        throw ylib::exception(MSG); \
    }              \
}


#define THROW_RET_BIND(RESULT)  THROW_RET(RESULT,"SQLBindParameter Failed,code: " + std::to_string(RESULT))
#define THROW_RET_GET(RESULT)  THROW_RET(RESULT,"SQLGetData Failed,code: " + std::to_string(RESULT))



ylib::mssql::conn::conn()
{
    m_handle = nullptr;
    m_ppst = nullptr;
}

ylib::mssql::conn::~conn()
{
    close();
}
EXAMPLE_START_RESULT ylib::mssql::conn::start(const mssql_conn_info &info)
{
    if (info.odbcname.empty())
    {
        m_lastErrorDesc = "odbcname is empty";
        return SR_FAILED;
    }
    m_info = info;
    

    SQLHDBC hDbc = nullptr;
    SQLAllocHandle(SQL_HANDLE_DBC,  m_pool->env, &hDbc);
    SQLCHAR connStr[1024];
    sprintf((char*)connStr, "DSN=%s;UID=%s;PWD=%s;", m_info.odbcname.c_str(), m_info.username.c_str(), m_info.password.c_str());
    SQLRETURN ret = SQLDriverConnect(hDbc, NULL, connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        // 连接失败
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        return SR_FAILED;
    }
    m_handle = hDbc;
    return SR_SUCCESS;
}

void ylib::mssql::conn::close()
{
    if (m_handle == nullptr)
        return;
    SQLDisconnect(m_handle);
    SQLFreeHandle(SQL_HANDLE_DBC, m_handle);
}

void ylib::mssql::conn::recover()
{
    if (m_sw == 1)
    {
        rollback();
    }
    SQLSetConnectAttr(m_pool->env, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0);
    clear();
    
}

void ylib::mssql::conn::task_out()
{

}

void ylib::mssql::conn::clear()
{
    if(this->m_ppst != nullptr)
        delete this->m_ppst;

    this->m_ppst = nullptr;
}

ylib::mssql::prepare_statement* ylib::mssql::conn::setsql(const std::string &sql)
{
    CHECK_SQL_CONNECTION;
    clear();
    m_ppst = new prepare_statement;
    THROW_RET(SQLAllocHandle(SQL_HANDLE_STMT, m_handle, &m_ppst->m_handle), "SQLAllocHandle failed");
    THROW_RET(SQLPrepare(m_ppst->m_handle, (SQLCHAR*)sql.c_str(), SQL_NTS), "SQLPrepare failed.");
    return m_ppst;
}
void ylib::mssql::conn::begin()
{
    CHECK_SQL_CONNECTION;
    auto ret = SQLSetConnectAttr(m_handle, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, 0);
    THROW_RET(ret,"SQLSetConnectAttr failed,code: "+std::to_string(ret));
    m_sw = 1;
}

void ylib::mssql::conn::commit()
{
    auto ret = SQLTransact(m_pool->env, m_handle, SQL_COMMIT);
    THROW_RET(ret, "SQLSetConnectAttr failed,code: " + std::to_string(ret));
    m_sw = 0;
}

void ylib::mssql::conn::rollback()
{
    auto ret = SQLTransact(m_pool->env, m_handle, SQL_ROLLBACK);
    THROW_RET(ret, "SQLTransact failed,code: " + std::to_string(ret));
}
ylib::mssql::prepare_statement::prepare_statement()
{
    m_result = nullptr;
    m_handle = nullptr;
}

ylib::mssql::prepare_statement::~prepare_statement()
{
    clear();
    if(m_handle != nullptr)
    {
        SQLFreeHandle(SQL_HANDLE_STMT, m_handle);
        m_handle = nullptr;
    }
}
#if 0
#define PRINT_DEBUG_SET std::cout<<__FUNCTION__<<"["<<index<<"]: "<<value<<std::endl
#define PRINT_DEBUG_SET_std::string std::cout<<__FUNCTION__<<"["<<index<<"]: "<<value.c_str()<<std::endl
#else
#define PRINT_DEBUG_SET
#define PRINT_DEBUG_SET_NSTRING
#endif



void ylib::mssql::prepare_statement::set_boolean(uint32 index, bool value)
{
    CHECK_SQL_PPST;
    PRINT_DEBUG_SET;

    SQLLEN lenOrInd = 0;
    auto ret = SQLBindParameter(m_handle, index, SQL_PARAM_INPUT, SQL_C_BIT, SQL_BIT, 0, 0, &value, 0, &lenOrInd);
    THROW_RET_BIND(ret);
}

void ylib::mssql::prepare_statement::set_datetime(uint32 index, const std::string &value)
{
    auto sec = time::to_ts(value);
    ylib::time::datetime dt;
    time::format(sec, dt);
    
    SQL_TIMESTAMP_STRUCT mssdt;
    mssdt.year = dt.year;
    mssdt.month = dt.month;
    mssdt.day = dt.day;
    mssdt.hour = dt.hour;
    mssdt.minute = dt.minute;
    mssdt.second = dt.second;
    mssdt.fraction = 0;
    SQLLEN lenOrInd = 0;
    auto ret = SQLBindParameter(m_handle, index, SQL_PARAM_INPUT, SQL_C_TYPE_TIMESTAMP, SQL_TIMESTAMP, 0, 0, &mssdt, 0, &lenOrInd);
    THROW_RET_BIND(ret);
}

void ylib::mssql::prepare_statement::set_double(uint32 index, double value)
{
    CHECK_SQL_PPST;
    PRINT_DEBUG_SET;

    SQLLEN lenOrInd = 0;
    auto ret = SQLBindParameter(m_handle, index, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, &value, 0, &lenOrInd);
    THROW_RET_BIND(ret);
}

void ylib::mssql::prepare_statement::set_int32(uint32 index, int32 value)
{
    CHECK_SQL_PPST;
    PRINT_DEBUG_SET;
    SQLLEN lenOrInd = 0;

    auto ret = SQLBindParameter(m_handle, index, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &value, 0, &lenOrInd);
    THROW_RET_BIND(ret);
}

void ylib::mssql::prepare_statement::set_uint32(uint32 index, uint32 value)
{
    CHECK_SQL_PPST;
    PRINT_DEBUG_SET;
    

    SQLLEN lenOrInd = 0;
    auto ret = SQLBindParameter(m_handle, index, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &value, 0, &lenOrInd);
    THROW_RET_BIND(ret);
}

void ylib::mssql::prepare_statement::set_int64(uint32 index, int64 value)
{
    CHECK_SQL_PPST;
    PRINT_DEBUG_SET;
    
    SQLLEN lenOrInd = 0;

    auto ret = SQLBindParameter(m_handle, index, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, 0, 0, &value, 0, &lenOrInd);
    THROW_RET_BIND(ret);
}

void ylib::mssql::prepare_statement::set_uint64(uint32 index, uint64 value)
{
    CHECK_SQL_PPST;
    PRINT_DEBUG_SET;
    
    SQLLEN lenOrInd = 0;
    auto ret = SQLBindParameter(m_handle, index, SQL_PARAM_INPUT, SQL_C_UBIGINT, SQL_BIGINT, 0, 0, &value, 0, &lenOrInd);
    THROW_RET_BIND(ret);
}

void ylib::mssql::prepare_statement::set_null(uint32 index)
{
    //throw ylib::exception("Not supported temporarily");
    
    SQLLEN lenOrInd = SQL_NULL_DATA;
    auto ret = SQLBindParameter(m_handle, index, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, NULL, 0, &lenOrInd);
    THROW_RET_BIND(ret);
}

void ylib::mssql::prepare_statement::set_string(uint32 index, const std::string &value)
{
    CHECK_SQL_PPST;
    PRINT_DEBUG_SET_NSTRING;
    
    SQLLEN lenOrInd = SQL_NTS;  // SQL_NTS denotes string is null-terminated

    auto ret = SQLBindParameter(m_handle, index, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, value.length(), 0, (void*)value.c_str(), 0, &lenOrInd);

    THROW_RET_BIND(ret);

}
void ylib::mssql::prepare_statement::clear()
{
    if(m_result != nullptr)
    {
        delete m_result;
    }
    m_result = nullptr;
}

uint64 ylib::mssql::prepare_statement::update()
{
    CHECK_SQL_PPST;
    clear();
    auto ret = SQLExecute(m_handle);
    THROW_RET(ret,"SQLExecute failed,code: "+std::to_string(ret));
    SQLLEN numRowsAffected = 0;
    ret = SQLRowCount(m_handle, &numRowsAffected);
    THROW_RET(ret, "SQLRowCount failed,code: " + std::to_string(ret));
    return numRowsAffected;
}
std::string getODBCError(SQLSMALLINT handleType, SQLHANDLE handle) {
    SQLINTEGER i = 0;
    SQLINTEGER nativeError;
    SQLCHAR sqlState[7];
    SQLCHAR message[1024];
    SQLSMALLINT textLength;
    SQLRETURN ret;
    std::ostringstream errorMsg;

    while (true) {
        ret = SQLGetDiagRec(handleType, handle, ++i, sqlState, &nativeError, message, sizeof(message), &textLength);
        if (ret == SQL_NO_DATA) {
            if (errorMsg.str().empty()) {
                // 无错误信息可提供
                errorMsg << "No detailed error information available.";
            }
            break;
        }
        errorMsg << "Error " << i << ": SQLState=" << sqlState << ", NativeError=" << nativeError << ", Message=" << message << std::endl;
    }

    return errorMsg.str();
}
ylib::mssql::result *ylib::mssql::prepare_statement::query()
{
    CHECK_SQL_PPST;
    clear();
    auto ret = SQLExecute(m_handle);
    if (ret != 0)
    {
        throw ylib::exception(getODBCError(SQL_HANDLE_STMT,m_handle));
    }
    //THROW_RET(ret, "SQLExecute failed,code: " + std::to_string(ret));
    
    m_result = new ylib::mssql::result();
    m_result->m_handle = m_handle;

    // 列数量
    {
        short num = 0;
        auto ret = SQLNumResultCols(m_handle, &num);
        THROW_RET(ret, "SQLNumResultCols failed,code: " + std::to_string(ret));
        m_result->m_field_count = num;
    }
    // 行数量
    {
        SQLLEN numRowsAffected = 0;
        auto ret = SQLRowCount(m_handle, &numRowsAffected);
        THROW_RET(ret, "SQLRowCount failed,code: " + std::to_string(ret));
        m_result->m_row_count = numRowsAffected;
    }
    // 列名称对应索引位置
    {
        SQLCHAR colName[128];
        SQLSMALLINT colNameLen;
        SQLSMALLINT dataType;
        SQLULEN columnSize;
        SQLSMALLINT decimalDigits;
        SQLSMALLINT nullable;
        SQLRETURN ret;
        SQLSMALLINT targetCol = 0;  // 目标列的索引
        for (SQLSMALLINT i = 1; i <= m_result->m_field_count; i++) {
            ret = SQLDescribeCol(m_handle, i, colName, sizeof(colName), &colNameLen, &dataType, &columnSize, &decimalDigits, &nullable);
            if (SQL_SUCCEEDED(ret)) {
                m_result->m_field_name_index[reinterpret_cast<char*>(colName)] = i;
            }
        }
    }
    return m_result;
}

ylib::mssql::result::result()
{
    m_handle = nullptr;
}

ylib::mssql::result::~result()
{
    if(m_handle != nullptr)
    {
        m_handle = nullptr;
    }
}

uint32 ylib::mssql::result::field_count()
{
    return m_field_count;
}

size_t ylib::mssql::result::row_count()
{
    return m_row_count;
}

bool ylib::mssql::result::next()
{
    CHECK_SQL_PPST;
    return SQLFetch(m_handle) == SQL_SUCCESS;
}

int32 ylib::mssql::result::get_int32(uint32 index)
{
    CHECK_SQL_PPST;
    SQLINTEGER value;
    SQLLEN length_or_indicator;
    auto ret = SQLGetData(m_handle, 1, SQL_C_SLONG, &value, sizeof(value), &length_or_indicator);
    THROW_RET_GET(ret);
    return value;
}

int32 ylib::mssql::result::get_int32(const std::string &name)
{
    return get_int32(m_field_name_index[name]);
}

uint32 ylib::mssql::result::get_uint32(uint32 index)
{
    CHECK_SQL_PPST;
    SQLUINTEGER  value;
    SQLLEN length_or_indicator;
    auto ret = SQLGetData(m_handle, 1, SQL_C_ULONG, &value, sizeof(value), &length_or_indicator);
    THROW_RET_GET(ret);
    return value;
}

uint32 ylib::mssql::result::get_uint32(const std::string &name)
{
    return get_uint32(m_field_name_index[name]);
}

int64 ylib::mssql::result::get_int64(uint32 index)
{
    CHECK_SQL_PPST;
    SQLBIGINT   value;
    SQLLEN length_or_indicator;
    auto ret = SQLGetData(m_handle, 1, SQL_C_SBIGINT, &value, sizeof(value), &length_or_indicator);
    THROW_RET_GET(ret);
    return value;
}

int64 ylib::mssql::result::get_int64(const std::string &name)
{
    return get_int64(m_field_name_index[name]);
}

uint64 ylib::mssql::result::get_uint64(uint32 index)
{
    CHECK_SQL_PPST;
    SQLUBIGINT   value;
    SQLLEN length_or_indicator;
    auto ret = SQLGetData(m_handle, 1, SQL_C_UBIGINT, &value, sizeof(value), &length_or_indicator);
    THROW_RET_GET(ret);
    return value;
}

uint64 ylib::mssql::result::get_uint64(const std::string &name)
{
    return get_uint64(m_field_name_index[name]);
}
std::string ylib::mssql::result::get_string(uint32 index)
{
    CHECK_SQL_PPST;
    SQLCHAR buffer[1024]; // 初始缓冲区
    SQLLEN str_len_or_ind;
    // 第一次调用SQLGetData来获取所需缓冲区的大小
    SQLRETURN ret = SQLGetData(m_handle, 1, SQL_C_CHAR, buffer, sizeof(buffer), &str_len_or_ind);

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        if (str_len_or_ind == SQL_NULL_DATA) {
            return "";
        }
        else if (str_len_or_ind > sizeof(buffer) - 1 || ret == SQL_SUCCESS_WITH_INFO) {
            // 缓冲区太小，需要更大的缓冲区
            std::string data;
            data.resize(str_len_or_ind);  // 调整字符串大小以适应全部数据

            // 重新调用SQLGetData获取完整数据
            SQLGetData(m_handle, 1, SQL_C_CHAR, &data[0], data.size() + 1, &str_len_or_ind);
            return data;
        }
        else {
            // 缓冲区足够，数据已经被完整读取
            std::string data((char*)buffer);
            return data;
        }
    }
    throw ylib::exception("SQLGetData failed, code: "+std::to_string(ret));
}
std::string ylib::mssql::result::get_string(const std::string &name)
{
   return get_string(m_field_name_index[name]);
}

bool ylib::mssql::result::get_boolean(uint32 index)
{
    CHECK_SQL_PPST;
    SQLCHAR    value;
    SQLLEN length_or_indicator;
    auto ret = SQLGetData(m_handle, 1, SQL_C_BIT, &value, sizeof(value), &length_or_indicator);
    THROW_RET_GET(ret);
    return (bool)value;
}

bool ylib::mssql::result::get_boolean(const std::string &name)
{
    return get_boolean(m_field_name_index[name]);
}

double ylib::mssql::result::get_double(uint32 index)
{
    CHECK_SQL_PPST;
    SQLDOUBLE value;
    SQLLEN length_or_indicator;
    auto ret = SQLGetData(m_handle, 1, SQL_C_DOUBLE, &value, sizeof(value), &length_or_indicator);
    THROW_RET_GET(ret);
    return value;
}

double ylib::mssql::result::get_double(const std::string &name)
{
    return get_double(m_field_name_index[name]);
}

ylib::mssql::pool::pool()
{
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
    SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    SQLSetEnvAttr(env, SQL_ATTR_CONNECTION_POOLING, (void*)SQL_CP_ONE_PER_HENV, 0);
}
ylib::mssql::pool::~pool()
{
    SQLFreeHandle(SQL_HANDLE_ENV, env);
}
#endif
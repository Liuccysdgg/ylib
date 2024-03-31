
#include "ybase/log4.h"
#include <iostream>
#include <string.h>
#include <iomanip>
#include <memory>

#include "yutil/time.h"
#if LIB_LOG4CPLUS
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
#include "log4cplus/configurator.h"
#include "log4cplus/initializer.h"
#include "log4cplus/log4cplus.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/fileappender.h"
#endif
#define APPENDER (*(log4cplus::SharedAppenderPtr*)m_appender)
#define APPENDER_CONSOLE (*(log4cplus::SharedAppenderPtr*)m_appender_console)

#define APPENDER_LOG(NAME,VALUE,LEVEL)                      \
auto logger = log4cplus::Logger::getInstance(NAME);       \
    logger.addAppender(APPENDER);                                      \
    logger.addAppender(APPENDER_CONSOLE);                                      \
    LOG4CPLUS_MACRO_BODY(logger, VALUE, LEVEL);

ylib::log4::log4(const std::string& filepath)
{

#if LIB_LOG4CPLUS
    ylib::file::create_dir(ylib::file::pre_dir(filepath), true);
    log4cplus::Initializer();
    m_appender = new log4cplus::SharedAppenderPtr(new log4cplus::RollingFileAppender(filepath.c_str(), 1024 * 1024, 100));
    m_appender_console = new log4cplus::SharedAppenderPtr(new log4cplus::ConsoleAppender());
    APPENDER->setLayout(std::unique_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%D{%Y-%m-%d %H:%M:%S.%q} [%-5p] [%c] -  %m %n")));
    APPENDER_CONSOLE->setName("console");
    APPENDER_CONSOLE->setLayout(std::unique_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%D{%Y-%m-%d %H:%M:%S.%q} [%-5p] [%c] -  %m %n")));
#endif

}

ylib::log4::~log4()
{
#if LIB_LOG4CPLUS
    delete (log4cplus::SharedAppenderPtr*)m_appender;
    log4cplus::Logger::shutdown();
#endif
}
#if LIB_LOG4CPLUS
inline void append_log(const char* name,const std::string& value)
{
    printf("%s: %s [%s]\r\n", time::now_time().c_str(), name, value.c_str());
}
#else
inline void append_log(const char* name, const char* value)
{
    printf("%s: %s [%s]\r\n",time::now_time().c_str(), name, value);
}
#endif
ylib::log4& ylib::log4::warn(const std::string& value,const std::string& name)
{


#if LIB_LOG4CPLUS
    APPENDER_LOG(name.c_str(), value.c_str(), WARN_LOG_LEVEL);
#else
    append_log(name.c_str(), value.c_str());
#endif
    return *this;
}
ylib::log4& ylib::log4::info(const std::string& value,const std::string& name)
{
#if LIB_LOG4CPLUS
    APPENDER_LOG(name.c_str(), value.c_str(), INFO_LOG_LEVEL);
#else
    append_log(name.c_str(), value.c_str());
#endif
    return *this;
}
ylib::log4& ylib::log4::error(const std::string& value,const std::string& name)
{

#if LIB_LOG4CPLUS
    APPENDER_LOG(name.c_str(), value.c_str(), ERROR_LOG_LEVEL);
#else
    append_log(name.c_str(), value.c_str());
#endif
    return *this;
}
ylib::log4& ylib::log4::fatal(const std::string& value,const std::string& name)
{
#if LIB_LOG4CPLUS
    APPENDER_LOG(name.c_str(), value.c_str(), FATAL_LOG_LEVEL);
#else
    append_log(name.c_str(), value.c_str());
#endif
    return *this;
}
ylib::log4& ylib::log4::debug(const std::string& value,const std::string& name)
{
#if LIB_LOG4CPLUS
    APPENDER_LOG(name.c_str(), value.c_str(), DEBUG_LOG_LEVEL);
#else
    append_log(name.c_str(), value.c_str());
#endif
    return *this;
}
#if 0
void ylib::log4::exp(const std::string& name,const std::string& value,int line,const char* funcname)
{

    log4cplus::Logger::getInstance(name.c_str()).log(log4cplus::FATAL_LOG_LEVEL,value.c_str(),LOG4CPLUS_CALLER_FILE(),line,funcname);
    append_log(log4cplus::FATAL_LOG_LEVEL,name.c_str(),value.c_str());
    trw_str(value);
}
#endif

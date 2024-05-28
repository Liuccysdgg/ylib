#include "base/error.h"
void ylib::error::print(const std::string& value)
{
    std::string msg ="[ylib][error]:\t"+value;
    std::cout<<msg<<std::endl;
}

const std::string &ylib::error_base::last_error()
{
    return m_lastErrorDesc;
}

#include "base/exception.h"

ylib::exception::exception(const std::string &msg)
{
    m_msg = msg;
}

ylib::exception::~exception()
{

}

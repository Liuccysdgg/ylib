#include "FAVParent.h"

std::string FAVParent::codeToErrorDesc(int code)
{
    char error_desc[AV_ERROR_MAX_STRING_SIZE];
    av_strerror(code, error_desc, sizeof(error_desc));
    return error_desc;
}

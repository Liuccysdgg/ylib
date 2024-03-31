#pragma once
#include <string>
#include "ybase/error.h"
#include "ybase/define.h"
#include "FAVHeaders.h"
class FAVParent:public ylib::error_base
{
protected:
	std::string codeToErrorDesc(int code);
};


#pragma once

#include "ybase/define.h"
#include "ybase/buffer.h"
namespace ylib
{
    namespace img
    {
        //生成验证码
        ylib::buffer make_code(uint32 width,uint32 height,uint32 code_length, std::string& captcha);
    }
}

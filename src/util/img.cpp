// MIT License﻿
// Copyright(c) 2024 FastWeb - fwlua.com - nianhua﻿
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :﻿
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.﻿
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.﻿
// ## Additional Terms for Commercial Use﻿
// This software is licensed for personal, educational, and non - commercial use.
// For commercial use or use within a company, organization, or institution, a
// separate commercial license is required.To obtain a commercial license,
// please contact
// EMail：1585346868@qq.com
// Mobile：17367918735
// QQ：1585346868

#include "util/img.h"

#include "cimg/CImg.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <random>

#include "util/system.h"
#include "util/file.h"
ylib::buffer ylib::img::make_code(uint32 width, uint32 height, uint32 code_length,std::string& captcha)
{
    // 创建白色背景图像
    cimg_library::CImg<unsigned char> img(width, height, 1, 3, 255);

    // 初始化随机数生成器
    std::random_device rd;  // 随机数种子
    std::mt19937 gen(rd()); // 标准 mersenne_twister_engine
    std::uniform_int_distribution<> distrib(0, 35);

    // 生成随机验证码文本
    const std::string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (uint32 i = 0; i < code_length; ++i) {
        captcha += letters[distrib(gen)];
    }

    // 计算字体大小和位置，绘制文本
    int fontSize = height/2+5; 
    int startX = (width - fontSize * code_length) / 2;
    int startY = (height - fontSize) / 2 + fontSize / 4;

    for (uint32 i = 0; i < code_length; ++i) {
        uint32 x = startX + i * fontSize;
        img.draw_text((int)x, startY, std::string(1, captcha[i]).c_str(), "black", 0, 1, fontSize);
    }

    // 保存图像
    std::string tempfilepath = system::temp_path() 
#ifdef _WIN32
        + "\\img_code_" 
#else
        + "/img_code_"
#endif
        + std::to_string(system::random(9999999, 999999999));
    img.save_bmp(tempfilepath.c_str());
    ylib::buffer result;
    result = ylib::file::read(tempfilepath);
    ylib::file::remove(tempfilepath);
    return result;
}
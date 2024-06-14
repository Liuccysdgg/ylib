/*Software License

Copyright(C) 2024[liuyingjie]
License Terms
Usage Rights

Any individual or entity is free to use, copy, and distribute the binary form of this software without modification to the source code, without the need to disclose the source code.
If the source code is modified, the modifications must be open - sourced under the same license.This means that the modifications must be disclosed and accompanied by a copy of this license.
Future Versions Updates
From this version onwards, all future releases will be governed by the terms of the latest version of the license.This license will automatically be nullified and replaced by the new version.
Users must comply with the terms of the new license issued in future releases.
Liability and Disclaimer
This software is provided “as is”, without any express or implied warranties, including but not limited to the warranties of merchantability, fitness for a particular purpose, and non - infringement.In no event shall the author or copyright holder be liable for any claims, damages, or other liabilities, whether in an action of contract, tort, or otherwise, arising from, out of, or in connection with the software or the use or other dealings in the software.
Contact Information
If you have any questions, please contact us: 1585346868@qq.com Or visit our website fwlua.com.
*/

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
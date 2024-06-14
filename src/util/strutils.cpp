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

// QQ：1585346868

#include "util/strutils.h"
#include <algorithm>
#include <cctype>
#ifdef _WIN32
#include <Windows.h>
#endif
std::string ylib::strutils::remove(std::string str, const std::string &sub)
{
    size_t pos = 0;
    while ((pos = str.find(sub, pos)) != std::string::npos) {
        str.erase(pos, sub.length());
    }
    return str;
}

std::string ylib::strutils::change_case(const std::string &input, bool toUpperCase)
{
    std::string output = input;
    if (toUpperCase) {
        std::transform(output.begin(), output.end(), output.begin(),
                       [](unsigned char c) { return std::toupper(c); });
    } else {
        std::transform(output.begin(), output.end(), output.begin(),
                       [](unsigned char c) { return std::tolower(c); });
    }
    return output;
}

std::vector<std::string> ylib::strutils::split(const std::string &str, const std::string &delim)
{
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos - prev);
        if (!token.empty()) { // Skip empty tokens if they occur
            tokens.push_back(token);
        }
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());

    return tokens;
}
static inline bool is_trim_char(char c, const std::vector<char>& trim_chars) {
    return std::find(trim_chars.begin(), trim_chars.end(), c) != trim_chars.end();
}
std::string ylib::strutils::trim(std::string s, const std::vector<char> &trim_chars)
{
    return trim_end(trim_begin(s,trim_chars),trim_chars);
}

std::string ylib::strutils::trim_begin(std::string s, const std::vector<char> &trim_chars)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [&trim_chars](char ch) {
                return !is_trim_char(ch, trim_chars);
            }));
    return s;
}

std::string ylib::strutils::trim_end(std::string s, const std::vector<char> &trim_chars, bool loop)
{

    //s.erase(std::find_if(s.rbegin(), s.rend(), [&trim_chars](char ch) {
    //            return !is_trim_char(ch, trim_chars);
    //        }).base(), s.end());

//    if (loop) {

        
   // }
    if (s.length() < 1)
        return s;

    bool find = true;
    while (find) {
        find = false;
        for (size_t i = 0; i < trim_chars.size(); i++)
        {
            if (s[s.length() - 1] == trim_chars[i])
            {
                find = true;
                s = s.substr(0, s.length() - 1);
            }
        }
    }

    return s;
}

std::string ylib::strutils::left(const std::string &str, size_t len)
{
    if (len > str.size()) len = str.size(); // 如果 n 大于字符串长度，调整 n 为字符串长度
    return str.substr(0, len); // 返回从位置 0 开始、长度为 n 的子字符串
}

std::string ylib::strutils::right(const std::string &str, size_t len)
{
    if (len > str.size()) len = str.size(); // 如果 n 大于字符串长度，调整 n 为字符串长度
    return str.substr(str.size() - len, len); // 返回从位置 str.size() - n 开始、长度为 n 的子字符串
}

std::vector<std::string> ylib::strutils::split(const std::string &str, char delim)
{
    return split(str,std::string(1,delim));
}

bool ylib::strutils::is_num(const std::string &str)
{
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}
bool ylib::strutils::is_num_code(char ch)
{
    return ch >= '0' && ch <= '9';
}
bool ylib::strutils::is_en_code(char ch)
{
    return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}
std::string ylib::strutils::between(const std::string& str,const std::string &begin, const std::string &end)
{
    size_t start_pos = str.find(begin);
    if (start_pos == std::string::npos) {
        return ""; // 起始文本不在字符串中
    }
    start_pos += begin.length(); // 移动到起始文本之后的位置
    size_t end_pos = str.find(end, start_pos); // 从起始文本之后开始查找终止文本
    if (end_pos == std::string::npos) {
        return ""; // 终止文本不在字符串中
    }
    return str.substr(start_pos, end_pos - start_pos); // 提取中间文本
}
std::string ylib::strutils::replace(const std::string& str, const std::string& search, const std::string& value)
{
    if (search.empty()) {
        return str;
    }
    std::string result = str;
    size_t startPos = 0;
    while ((startPos = result.find(search, startPos)) != std::string::npos) {
        result.replace(startPos, search.length(), value);
        startPos += value.length(); // 跳过已替换部分
    }
    return result;
}
std::string ylib::strutils::replace(const std::string& str, uchar search, uchar value)
{
    std::string result = str;
    for (size_t i = 0; i < result.length(); i++)
    {
        if (result[i] == search)
            result[i] = value;
    }
    return result;
}
std::string ylib::strutils::F(const char* value)
{
    if (value == NULL)
        return "";
    return std::string(value);
}
#ifdef _WIN32
std::wstring ylib::strutils::to_wstring(const std::string& value)
{
    int len;
    int slength = (int)value.length() + 1;
    len = MultiByteToWideChar(65001, 0, value.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(65001, 0, value.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}
std::string ylib::strutils::wto_string(const std::wstring& value)
{
    int len = WideCharToMultiByte(65001, 0, value.c_str(), value.size(), NULL, 0, NULL, NULL);
    char* buffer = new char[len + 1];
    memset(buffer, '\0', sizeof(char) * (len + 1));
    WideCharToMultiByte(65001, 0, value.c_str(), value.size(), buffer, len, NULL, NULL);
    std::string result(buffer);
    delete[] buffer;
    return result;
}
std::string ylib::strutils::pad_with_begin(const std::string& value, size_t fixed_length, char pad)
{
    if (value.length() >= fixed_length)
        return value;
    size_t append_length = fixed_length - value.length();
    std::string result = value;
    for(size_t i=0;i< append_length;i++)
        result = std::string(1,pad) + result;
    return result;
}
std::string ylib::strutils::pad_with_end(const std::string& value, size_t fixed_length, char pad)
{
    if (value.length() >= fixed_length)
        return value;
    size_t append_length = fixed_length - value.length();
    std::string result = value;
    for (size_t i = 0; i < append_length; i++)
        result = result+ std::string(1, pad);
    return result;
}
#endif
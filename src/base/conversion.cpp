#include "base/conversion.h"
#include "base/error.h"
#include <cmath>
#include <sstream>
#include <iomanip>
int32 ylib::stoi(const std::string &value)
{
    if(value.empty())
        return 0;
    try {
        return std::stoi(value.c_str());
    } catch (const std::exception& e) {
        ylib::error::print("std::stoi conversion failed, error:"+std::string(e.what()) + " str:" + value);
    }
    return 0;
}
int32 ylib::stoi(char value)
{
    return ylib::stoi(std::string(1,value));
}
uint64 ylib::stoull(const std::string &value)
{
    if(value.empty())
        return 0;
    try {
        return std::stoull(value.c_str());
    } catch (const std::exception& e) {
        ylib::error::print("std::stoull conversion failed, error:" + std::string(e.what()) + " str:"+value);
    }
    return 0;
}

int64 ylib::stoll(const std::string &value)
{
    if(value.empty())
        return 0;
    try {
        return std::stoll(value.c_str());
    } catch (const std::exception& e) {
        ylib::error::print("std::stoll conversion failed, error:" + std::string(e.what()) + " str:"+value);
    }
    return 0;
}

double ylib::stod(const std::string &value)
{
    if(value.empty())
        return 0.0f;
    try {
        return std::stod(value.c_str());
    } catch (const std::exception& e) {
        ylib::error::print("std::stod conversion failed, error:" + std::string(e.what()) + " str:"+value);
    }
    return 0.0f;
}

float ylib::stof(const std::string &value)
{
    if(value.empty())
        return 0.0f;
    try {
        return std::stof(value.c_str());
    } catch (const std::exception& e) {
        ylib::error::print("std::stof conversion failed, error:" + std::string(e.what()) + " str:"+value);
    }
    return 0.0f;
}
std::string ylib::to_fixed(double num, uint32 x)
{
    try
    {
        std::ostringstream out;
        out << std::fixed << std::setprecision(x) << num;
        return out.str();
    }
    catch (const std::exception&)
    {

    }
    return std::to_string(num);
}



void ylib::bytes::to_int(int32& dest, const char* src, bool reverse)
{
    char* dest_char = (char*)&dest;
    for (int i = 0; i < sizeof(dest); i++)
    {
        int idx = (reverse == true ? sizeof(dest) - 1 - i : i);
        dest_char[i] = src[idx];
    }

    //dest = atoi(src);
}
void ylib::bytes::to_uint(uint32& dest, const char* src, bool reverse)
{
    char* dest_char = (char*)&dest;
    for (int i = 0; i < sizeof(dest); i++)
        dest_char[i] = src[(reverse == true ? sizeof(dest) - 1 - i : i)];

    // dest = atoi(src);
}

void ylib::bytes::to_short(short& dest, const char* src, bool reverse)
{
    char* dest_char = (char*)&dest;
    for (int i = 0; i < sizeof(dest); i++)
        dest_char[i] = src[(reverse == true ? sizeof(dest) - 1 - i : i)];
}

void ylib::bytes::to_ushort(ushort& dest, const char* src, bool reverse)
{
    char* dest_char = (char*)&dest;
    for (int i = 0; i < sizeof(dest); i++)
        dest_char[i] = src[(reverse == true ? sizeof(dest) - 1 - i : i)];
}

void ylib::bytes::to_char(char* dest, short src, bool reverse)
{
    char* src_char = (char*)&src;
    for (int i = 0; i < sizeof(src); i++)
        dest[i] = src_char[(reverse == true ? sizeof(src) - 1 - i : i)];
}
void ylib::bytes::to_char(char* dest, int32 src, bool reverse)
{
    char* src_char = (char*)&src;
    for (int i = 0; i < sizeof(src); i++)
        dest[i] = src_char[(reverse == true ? sizeof(src) - 1 - i : i)];
}
void ylib::bytes::to_char(char* dest, int64 src, bool reverse)
{
    char* src_char = (char*)&src;
    for (int i = 0; i < sizeof(src); i++)
        dest[i] = src_char[(reverse == true ? sizeof(src) - 1 - i : i)];
}
ylib::buffer  ylib::bytes::to_buffer(short src, bool reverse)
{
    ylib::buffer result;
    result.resize(2);
    to_char((char*)result.data(),(short)src,reverse);
    return result;
}

ylib::buffer  ylib::bytes::to_buffer(int32 src, bool reverse)
{
    ylib::buffer result;
    result.resize(4);
    to_char((char*)result.data(), (int32)src, reverse);
    return result;
}

ylib::buffer  ylib::bytes::to_buffer(int64 src, bool reverse)
{
    ylib::buffer result;
    result.resize(8);
    to_char((char*)result.data(), (int64)src, reverse);
    return result;
}

ylib::buffer  ylib::bytes::to_buffer(uchar src)
{
    ylib::buffer result;
    result.resize(1);
    result[0] = src;
    return result;
}

void ylib::bytes::reverse(void* dest, size_t length)
{
    // 3
    char* cd = (char*)dest;
    for (int i = 0; i < (int)length / 2; i++)
    {
        char temp = cd[i];
        cd[i] = cd[length - 1 - i];
        cd[length - 1 - i] = temp;
    }
}



void ylib::bytes::to_uint64(uint64& dest, const char* src, bool reverse)
{
    char* dest_char = (char*)&dest;
    for (int i = 0; i < sizeof(dest); i++)
        dest_char[i] = src[(reverse == true ? sizeof(dest) - 1 - i : i)];
}

void ylib::bytes::to_int64(int64& dest, const char* src, bool reverse)
{
    char* dest_char = (char*)&dest;
    for (int i = 0; i < sizeof(dest); i++)
        dest_char[i] = src[(reverse == true ? sizeof(dest) - 1 - i : i)];
}


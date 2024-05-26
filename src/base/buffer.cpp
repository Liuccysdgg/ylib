#include "base/buffer.h"
#include <memory>
#include <exception>
#include <cstring>
ylib::buffer::buffer(size_t initial_length):m_data(initial_length)
{

}

ylib::buffer::buffer::buffer(char data, size_t len):m_data(len)
{
    memset(m_data.data(),(int)data,len);
    m_data.m_use_length = len;
}

ylib::buffer::buffer::buffer(const char *data, size_t len):m_data(len)
{
    append(data,len);
}

ylib::buffer::buffer(const buffer &data):m_data(data.length())
{
    append(data);
}

ylib::buffer::buffer(const std::string &data):m_data(data.length())
{
    append(data.c_str(),data.length());
}
ylib::buffer::buffer(std::initializer_list<uchar> char_list) :m_data(char_list.size())
{
    append(char_list);
}
ylib::buffer::~buffer()
{
    //clear();
}

void ylib::buffer::set(const char* data, size_t length)
{
    if (data == nullptr || length == 0)
    {
        throw ylib::exception("set data error, address:" + std::to_string((uint64)data) + "\tlength : " + std::to_string(length));
    }
        
    m_data.set((const uchar*)data,length);
}

void ylib::buffer::set(const ylib::buffer& data)
{
    set((const char*)data.data(), data.length());
}

void ylib::buffer::append(const std::string& data)
{
    append(data.c_str(),data.length());
}

void ylib::buffer::append(const char* data, size_t length)
{
    m_data.append((const uchar*)data,length);
}

void ylib::buffer::append(const ylib::buffer &data)
{
    append(data.data(),data.length());
}
void ylib::buffer::append(std::initializer_list<uchar> char_list)
{
    if (char_list.size() == 0)
        return;
    uchar* new_buf = (uchar*)malloc(char_list.size());
    size_t idx = 0;
    for (auto c : char_list)
    {
        new_buf[idx] = c;
        idx++;
    }
    this->append((char*)new_buf, idx);
    free(new_buf);
}


void ylib::buffer::clear()
{
    m_data.clear();
}

void ylib::buffer::resize(size_t length)
{
    if(length <= 0)
    {
        throw ylib::exception("resize length <= 0,length:"+std::to_string(length));
    }
    m_data.resize(length);
}

size_t ylib::buffer::find(const char *data, size_t len, size_t start_pos) const
{
    if (start_pos >= length())
        throw ylib::exception("Start position is out of buffer range. max:"+std::to_string(length()) + ", start_pos : "+std::to_string(start_pos));
    for (size_t i = start_pos; i <= length() - len; ++i) {
        const char* d = (const char*)m_data.data();
        if (std::memcmp(d + i, data, len) == 0) {
            return i;
        }
    }
    return std::string::npos;
}

size_t ylib::buffer::find(const buffer& data, size_t start_pos) const
{
    if(data.length() == 0)
        return -1;
    return find(data.data(),data.length(),start_pos);
}

std::vector<size_t> ylib::buffer::find_list(const buffer &value, size_t start) const
{
    std::vector<size_t> result;
    size_t idx = 0;
    while (idx != -1)
    {
        idx = find(value, start == 0 ? 0 : start);
        if (idx == -1)
            break;
        result.push_back(idx);
        start = idx + value.length();
        if (start >= length())
            break;
    }
    return result;
}

std::vector<ylib::buffer> ylib::buffer::split(const buffer &value, size_t start) const
{

    std::vector<ylib::buffer> result;
    auto idxs = find_list(value,start);
    for(size_t i=0;i<idxs.size();i++)
    {
        if(i == 0)
        {
            if(idxs[i] != 0){

                result.push_back(ylib::buffer(data(),idxs[i]));
            }
        }
        else
        {
            // OOBOOBOOOB00
            //   2  5   9
            result.push_back(
                ylib::buffer(data()+idxs[i-1]+value.length(),idxs[i]-idxs[i-1]-value.length())
            );
            if(i+1 == idxs.size() && idxs[i]+value.length() != length()){
                result.push_back(ylib::buffer(data()+idxs[i]+value.length(),length()-idxs[i]-value.length()));
            }
        }


    }
    return result;
}

std::string ylib::buffer::to_string() const
{
    if(empty())
        return std::string();
    else
        return std::string((const char*)m_data.data(),length());
}

bool ylib::buffer::empty() const
{
    return m_data.empty();
}

ylib::buffer ylib::buffer::left(size_t len) const
{
    if(len > length())
        return *this;
    return sub(0,len);
}

ylib::buffer ylib::buffer::right(size_t len) const
{
    if(len > length())
        return *this;
    return sub(length()-len,len);
}

ylib::buffer ylib::buffer::sub(size_t start, size_t len) const
{
    ylib::buffer result;
    if(start > length() || start + len > length())
        return result;
    result.append((const char*)m_data.data()+start,len);
    return result;
}

ylib::buffer ylib::buffer::sub(size_t start) const
{
    return  this->sub(start, length() - start);
}

ylib::buffer::operator std::string() const
{
    return to_string();
}

ylib::buffer ylib::buffer::operator+(const buffer &other) const
{
    ylib::buffer result;
    result.append(*this);
    result.append(other);
    return result;
}

ylib::buffer &ylib::buffer::operator=(const std::string &data)
{
    clear();
    append(data.c_str(),data.length());
    return *this;
}

unsigned char &ylib::buffer::operator[](size_t index) const
{
    if (index + 1 > m_data.m_use_length)
    {
        std::string msg = "ylib::buffer::operator[] index="+std::to_string(index) + " but data length is "+std::to_string(m_data.m_use_length);
        throw ylib::exception(msg.c_str());
    }
    return m_data.data()[index];
}

const char *ylib::buffer::data() const{return (const char*)m_data.data();}

const size_t ylib::buffer::length() const{return m_data.length();}

ylib::buffer ylib::buffer::trim_end(char value)
{
    size_t idx = 0;
    for (size_t i = 0; i < length(); i++)
    {
        if (operator[](length() - i - 1) == value)
            idx++;
        else
            break;
    }
    return this->left(length() - idx);
}

ylib::buffer ylib::buffer::trim_begin(char value)
{
    size_t idx = 0;
    for (size_t i = 0; i < length(); i++)
    {
        if (operator[](i) == value)
            idx++;
        else
            break;
    }
    return this->right(length() - idx);
}

ylib::buffer ylib::buffer::trim(char value)
{
    return trim_begin(value).trim_end(value);
}
ylib::buffer &ylib::buffer::operator=(const ylib::buffer &data)
{
    if (data.empty())
    {
        clear();
        return *this;
    }
    if(this == &data)
        return *this;
    clear();
    append(data);
    return *this;
}
std::string ylib::buffer::to_hex()
{
    unsigned char highByte, lowByte;
    size_t length = this->length();
    std::string dest;
    dest.append(length * 2,'0');
    for (size_t i = 0; i < length; i++)
    {
        highByte = (*this)[i] >> 4;
        lowByte = (*this)[i] & 0x0f;

        highByte += 0x30;

        if (highByte > 0x39)
            dest[i * 2] = highByte + 0x07;
        else
            dest[i * 2] = highByte;

        lowByte += 0x30;
        if (lowByte > 0x39)
            dest[i * 2 + 1] = lowByte + 0x07;
        else
            dest[i * 2 + 1] = lowByte;
    }
    return dest;
}

mem_pool::mem_pool(size_t initial_length):m_length(initial_length),m_use_length(0)
{
    if(initial_length != 0){
        m_data = new uchar[initial_length];
        memset(m_data,0,initial_length);
    }
    else
        m_data = nullptr;
}

mem_pool::~mem_pool() 
{
    
    if(m_data != nullptr)
        delete[] m_data;
    m_data = nullptr;
   // clear();
}

void mem_pool::append(const uchar *data, size_t length)
{

    // CheckDataLength
    if(m_use_length + length > m_length)
    {
        renew((m_use_length+length)*2);
    }
    std::memcpy(m_data+m_use_length,data,length);
    m_use_length += length;
}

void mem_pool::clear()
{
    //m_length = 0;
    //m_data = nullptr;
    if(m_data != nullptr)
        std::memset(m_data,0,m_length);
    m_use_length = 0;
}

size_t mem_pool::length() const
{
    return m_use_length;
}

uchar *mem_pool::data() const
{
    return m_data;
}

void mem_pool::resize(size_t length)
{
    if(m_length < length){
        renew(length);
    }
    m_use_length = length;
}

void mem_pool::set(const uchar *data, size_t length)
{
    resize(length);
    std::memcpy(m_data,data,length);
}

bool mem_pool::empty() const
{
    return m_data == nullptr || m_use_length == 0;
}

void mem_pool::renew(size_t length)
{
    uchar* new_char = new uchar[length];
    memset(new_char,0,length);
    if(m_data != nullptr)
    {
        std::memcpy(new_char,m_data,m_use_length);
        delete[] m_data;
    }
    m_data = new_char;
    m_length = length;
}


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

#include "util/file.h"

#include <cstdio>
#include <filesystem>
#include <regex>
#ifdef _WIN32
#include <io.h>
#include <Windows.h>
#endif
#include "util/strutils.h"
#include "util/system.h"
bool ylib::file::remove(const std::string &filepath)
{
    return std::remove(filepath.c_str()) == 0;
}

bool ylib::file::create_dir(const std::string &dirpath,bool create_parent_dir)
{
    if (dirpath.empty())
        return false;
    std::filesystem::path path = dirpath;
    while(!create_parent_dir){
        auto parent = path.parent_path();
        if(parent.empty())
            break;
        if(std::filesystem::exists(parent) == false)
            return false;
    }
    try
    {
        return std::filesystem::create_directories(std::filesystem::path(dirpath));
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() <<"\tfilepath:"<< dirpath<<" create_parent_dir:"<< create_parent_dir << std::endl;
    }
    return false;
}

ylib::file_io::file_io()
{

}

ylib::file_io::~file_io()
{
    close();
}

bool ylib::file_io::open(const std::string &filepath,bool only_read, bool auto_create)
{
    close();

    std::ios_base::openmode openmode = std::ios::in|std::ios::binary;
    if(!only_read)
        openmode |= std::ios::out;
    m_stream = new std::fstream(filepath,openmode);
    if(!m_stream->is_open()){
        if(only_read)
        {
            close();
            return false;
        }
        // 尝试创建文件，如果不存在的话
        std::ofstream outfile(filepath, std::ios::binary);
        outfile.close();
        // 再次尝试打开文件
        m_stream->open(filepath, std::ios::in | std::ios::out | std::ios::binary | std::ios::app);
        if (!m_stream->is_open()) {
            m_lastErrorDesc =  "Failed to open or create file: " + filepath;
            close();
            return false;
        }
    }
    m_only_read = only_read;
    m_filepath = filepath;
    return true;
}

void ylib::file_io::close()
{
    if(m_stream != nullptr){
        m_stream->close();
        delete m_stream;
        m_stream = nullptr;
    }
    m_filepath.clear();
    m_only_read = false;
}

bool ylib::file_io::appead(const char *data, int64 len)
{
    jump(0,std::ios::end);
    return write(data,len);
}

bool ylib::file_io::appead(const buffer &data)
{
    return appead(data.data(),data.length());
}

bool ylib::file_io::write(const char *data, int64 len)
{
    if(data == nullptr || len == 0)
        return true;
    m_stream->write(data,len);
    if(m_stream->fail())
    {
        m_lastErrorDesc = "write operation failed.";
        return false;
    }
    m_stream->flush();
    if(m_stream->fail())
    {
        m_lastErrorDesc = "flush operation failed.";
        return false;
    }
    return true;
}

bool ylib::file_io::write(const ylib::buffer &data)
{
    return write(data.data(),data.length());
}

ylib::buffer ylib::file_io::read(int64 size)
{
    ylib::buffer result;
    read(size,result);
    return result;
}

bool ylib::file_io::read(int64 size, buffer &data)
{
    if (size == 0)
        return false;
    data.resize((size_t)size);
    m_stream->read((char*)data.data(),size);
    if(m_stream->fail())
    {
        data.clear();
        m_lastErrorDesc ="read failed,size:"+std::to_string(size);
        return false;
    }
    return true;
}

void ylib::file_io::jump(int64 offset, std::ios_base::seekdir way)
{
    // 清楚所有错误状态
    m_stream->clear();
    // 根据提供的方向参数way来确定如何应用偏移量offset
    if (way == std::ios::beg || way == std::ios::end) {
        // 如果参考点是文件的开头或结尾，直接应用偏移量
        m_stream->seekg(offset, way); // seekg是用来设置istream（输入流）的位置的。
        m_stream->seekp(offset, way); // seekp是用来设置ostream（输出流）的位置的。
    } else if (way == std::ios::cur) {
        // 如果参考点是当前位置，由于当前读位置和写位置可能不同，需要分别设置
        std::streampos currentPosition = m_stream->tellg(); // 获取当前读位置
        m_stream->seekg(currentPosition + offset, std::ios::beg); // 将读位置移动offset个单位
        currentPosition = m_stream->tellp(); // 获取当前写位置
        m_stream->seekp(currentPosition + offset, std::ios::beg); // 将写位置移动offset个单位
    }

    // 注意: 如果流是用于同时读写的，那么我们需要保持读取和写入指针同步。
    // 在某些情况下，你可能只需要移动读指针或写指针，这取决于你的用例。
}

std::streampos ylib::file_io::cur()
{
    return m_stream->tellg();
}

bool ylib::file_io::clear()
{
    if(m_only_read){
        throw ylib::exception("The file is read-only and cannot be clear");
        return false;
    }
    m_stream->close();
    m_stream->open(m_filepath, std::ios::out | std::ios::trunc | std::ios::binary);
    m_stream->close();
    m_stream->open(m_filepath, std::ios::in | std::ios::out | std::ios::ate | std::ios::binary);
    return true;
}

std::streamsize ylib::file_io::size()
{
    auto cur_pos = cur();
    jump(0,std::ios::end);
    auto result =  m_stream->tellg();
    jump(cur_pos,std::ios::beg);
    return result;
}

bool file_io::is_open()
{
    return m_stream->is_open();
}

ylib::buffer ylib::file::read(const std::string &filepath)
{
    ylib::file_io f;
    if(!f.open(filepath,true))
        return ylib::buffer();
    return f.read(f.size());
}
bool file::read(const std::string &filepath, buffer &data)
{
    ylib::file_io f;
    if(!f.open(filepath,true))
        return false;
    data =  f.read(f.size());
    return true;
}

bool ylib::file::write(const std::string &filepath, const buffer &data)
{
    return write(filepath,data.data(),data.length());
}
bool ylib::file::write(const std::string &filepath, const char *data, size_t len)
{
    ylib::file_io f;
    if(!f.open(filepath))
        return false;
    f.clear();
    return f.write(data,len);
}

#ifdef _WIN32
bool ylib::file::list(const std::string &rootPath, std::map<std::string, bool> &list)
{
    list.clear();
#ifdef _WIN32
    char dirNew[200];
    strcpy_s(dirNew, rootPath.c_str());
    strcat_s(dirNew, "\\*.*");

    intptr_t handle;
    _finddata_t findData;

    handle = _findfirst(dirNew, &findData);    // 查找目录中的第一个文件
    if (handle == -1)
    {
        list.insert(std::make_pair(rootPath,false));
    }
    else
    {
        do
        {
            list.insert(std::make_pair(findData.name, GetFileAttributesA(std::string(rootPath + "\\" + findData.name).c_str()) & FILE_ATTRIBUTE_DIRECTORY));
        } while (_findnext(handle, &findData) == 0);    // 查找目录中的下一个文件
    }
    _findclose(handle);    // 关闭搜索句柄
    return true;
#else
    return false;
#endif // _WIN32
}
#endif
bool ylib::file::remove_dir(const std::string &dirpath
#if _WIN32
    , bool recycle
#endif
)
{
#if _WIN32
    if (recycle)
    {
        SHFILEOPSTRUCT  shDelFile;
        memset(&shDelFile, 0, sizeof(SHFILEOPSTRUCT));
        shDelFile.fFlags |= FOF_SILENT;
        shDelFile.fFlags |= FOF_NOERRORUI;
        shDelFile.fFlags |= FOF_NOCONFIRMATION;

        shDelFile.wFunc = FO_DELETE;
        ylib::buffer path;
        path.append(dirpath);
        path.append('\0');
        path.append('\0');
        shDelFile.pFrom = (LPCSTR)path.data();
        shDelFile.pTo = NULL;
        shDelFile.fFlags |= FOF_ALLOWUNDO;
        BOOL bres = SHFileOperation(&shDelFile);
        return !bres;
    }
#endif
    try {
        // 删除指定的目录及其所有内容
        std::uintmax_t n = std::filesystem::remove_all(dirpath);
    } catch (const std::filesystem::filesystem_error& e) {
        std::cout << "remove_dir failed,error:" + std::string(e.what()) + ",dirpath:" + dirpath << std::endl;
        return false;
    }
    return true;
}

std::string ylib::file::ext(const std::string &path)
{
    // 查找最后一个点的位置
    std::size_t last_dot_pos = path.find_last_of(".");
    // 检查是否有扩展名并返回
    if (last_dot_pos != std::string::npos && last_dot_pos != path.length() - 1)
        return path.substr(last_dot_pos + 1);
    // 没有扩展名或点在末尾
    return "";
}

bool ylib::file::exist(const std::string &filepath)
{
    try
    {
        return std::filesystem::exists(filepath) && std::filesystem::is_regular_file(filepath);
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    return false;
}
bool ylib::file::exist_dir(const std::string& dirpath)
{
    try
    {
        return std::filesystem::exists(dirpath) && std::filesystem::is_directory(dirpath);
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    return false;
}
int64 ylib::file::size(const std::string &filepath)
{
    try
    {
        std::filesystem::path file_path(filepath);
        if (std::filesystem::exists(file_path)) {
            return std::filesystem::file_size(file_path);
        }
        else {
            return 0;
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "ylib::file::size exception: " << e.what() <<"\tpath: "<< filepath << std::endl;
        return 0;
    }
    
}


std::string ylib::file::parent_dir(const std::string &path)
{
    std::filesystem::path file_path(path);
    return file_path.parent_path().string();
}

std::string ylib::file::filename(const std::string& path, bool have_ext)
{
    std::filesystem::path fsPath(path);
    std::string fileName = fsPath.filename().string();
    if (!have_ext) {
        fileName = fsPath.stem().string();
    }
    return fileName;
}

bool ylib::file::copy(const std::string& src, const std::string& dst)
{
    try {
#ifdef _WIN32
        return CopyFileA(src.c_str(),dst.c_str(),false);
#else
        // 复制文件
        std::filesystem::copy(src, dst, std::filesystem::copy_options::overwrite_existing);
#endif
        return true;
    }
    catch (const std::exception& e) {
        std::cout << "copy exception, src:" << src << "\tdst:" << dst << ". error:" << e.what() << std::endl;
    }
    return false;
}
void ylib::file::copy_dir(const std::string& src, const std::string& dst)
{
    ylib::file::create_dir(dst, true);
    auto map = traverse(src,".*");
    for_iter(iter, map) {
        if (iter->second == IS_DIRECTORY)
            ylib::file::create_dir(dst+"/"+iter->first,true);
    }
    for_iter(iter, map) {
        if (iter->second == IS_FILE)
            ylib::file::copy(src+"/" + iter->first, dst + "/" + iter->first);
    }
}
std::map<std::string, ylib::FileType> ylib::file::traverse(const std::string& dirpath, const std::string& regex_pattern)
{
    std::map<std::string, ylib::FileType> files_and_directories;
    std::regex pattern(regex_pattern); // 创建正则表达式对象

    if (!std::filesystem::exists(dirpath) || !std::filesystem::is_directory(dirpath)) {
        std::cerr << "provided path is not a valid directory. dir:"<<dirpath << std::endl;
        return files_and_directories;
    }
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(dirpath)) {
            const auto& path = entry.path();
            // 检查是否是文件且符合正则表达式
            std::string path_string = strutils::right(path.string(), path.string().length() - dirpath.length());
            path_string =strutils::trim_begin(path_string, { '\\','/'});
            if (entry.is_regular_file() && std::regex_match(path.filename().string(), pattern)) {
                
                files_and_directories[path_string] = ylib::FileType::IS_FILE;
            }
            else if (entry.is_directory()) 
            {
                files_and_directories[path_string] = ylib::FileType::IS_DIRECTORY;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "error occurred: " << e.what() << std::endl;
    }
    return files_and_directories;
}
std::string ylib::file::temp_filepath()
{
    return system::temp_path() + SEPRATOR + std::to_string(system::random(99999999,999999999));
}
std::string ylib::file::format_separator(const std::string& filepath)
{
    std::string result = filepath;
    for (size_t i = 0; i < result.size(); i++)
    {
#ifdef _WIN32
        if (result[i] == '/')
            result[i] = '\\';
#else
        if (result[i] == '\\')
            result[i] = '/';
#endif
    }
    return result;
}

timestamp ylib::file::last_write_time(const std::string& filepath)
{
    try {
        // 获取文件最后修改时间
        auto ftime = std::filesystem::last_write_time(filepath);
        auto sctp = std::chrono::time_point_cast<std::chrono::seconds>(ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
        auto time_since_epoch = sctp.time_since_epoch();
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count();
        return seconds;
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return -1;
}

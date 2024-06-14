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

// QQ：1585346868
#include "util/sharedmem.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#endif
ylib::sharedmem::sharedmem(bool auto_free):m_auto_free(auto_free)
{
}

ylib::sharedmem::~sharedmem()
{
    destory();
}

bool ylib::sharedmem::create(const std::string& name, size_t size)
{
	destory();

	m_name = name;
	m_size = size;
	m_memory = nullptr;


#ifdef _WIN32
    m_handle = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, size, name.c_str());
    if (!m_handle) {
        m_lastErrorDesc = "Could not create file mapping.";
        return false;
    }
    m_memory = MapViewOfFile(m_handle, FILE_MAP_ALL_ACCESS, 0, 0, size);
    if (m_memory == nullptr) {
        CloseHandle(m_handle);
        m_lastErrorDesc = "Could not map view of file.";
        return false;
    }
#else
    m_handle = shm_open(name.c_str(), O_CREAT | O_RDWR, 0666);
    if (m_handle == -1) {
        m_lastErrorDesc = "Could not create shared memory.";
        return false;
    }
    if (ftruncate(m_handle, size) == -1) {
        close(m_handle);
        shm_unlink(name.c_str());
        m_lastErrorDesc =  "Could not set size of shared memory.";
        return false;
    }
    m_memory = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, m_handle, 0);
    if (m_memory == MAP_FAILED) {
        close(m_handle);
        shm_unlink(name.c_str());
        m_lastErrorDesc = "Could not map shared memory.";
        return false;
    }
#endif
	return true;
}

void ylib::sharedmem::destory()
{
#ifdef _WIN32
    if (m_memory) {
        UnmapViewOfFile(m_memory);
    }
    if (m_handle) {
        CloseHandle(m_handle);
    }
    m_handle = nullptr;
#else
    if (m_memory) {
        munmap(m_memory, m_size);
    }
    if (m_handle != -1) {
        close(m_handle);
    }
    shm_unlink(m_name.c_str());
    m_handle = -1;
#endif
    m_memory = nullptr;
    
}

uchar ylib::sharedmem::operator[](size_t index)
{
    return ((uchar*)m_memory)[index];
}

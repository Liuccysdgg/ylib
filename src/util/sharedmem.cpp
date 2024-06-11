
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
    shm_unlink(m_name_.c_str());
    m_handle = -1;
#endif
    m_memory = nullptr;
    
}

uchar ylib::sharedmem::operator[](size_t index)
{
    return ((uchar*)m_memory)[index];
}

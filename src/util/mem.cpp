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

#include "util/mem.h"

#include <string>
#include <map>

#if DEBUG_MEM == 1
static std::map<void*,size_t> mem_record;
#endif
void* ylib::mem::malloc(size_t size)
{

    if (size > __big_size) { 
        printf("[Warn] try to apply for too much memory, please note\r\n"); 
    }
    void* result = ::malloc(size);
#if DEBUG_MEM == 1
    mem_record[result] = size;
#endif
    return result;
}

void ylib::mem::free(void* src)
{

    if (src == nullptr)
        return;

#if DEBUG_MEM == 1
    auto iter = mem_record.find(src);
    if(iter == mem_record.end()){
        std::cout<<"free not malloc mem,address:"<<std::hex<< src << std::endl;
    }
    else
    {

    }
    mem_record.erase(iter);
#endif
    ::free(src);
}

void* ylib::mem::realloc(void* src,size_t length)
{
#if DEBUG_MEM == 1
    auto iter = mem_record.find(src);
    if(iter == mem_record.end())
    {
        //THROW_FORMAT("not find realloc mem,address:{:#x}",src);
    }
#endif
    void* result = ::realloc(src, length);
#if DEBUG_MEM == 1
    if(result != src){
        mem_record.erase(iter);
        mem_record[result] = length;
    }
#endif
    return result;
}

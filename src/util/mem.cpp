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

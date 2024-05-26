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

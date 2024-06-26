﻿/*Software License

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
#if 0
#include "util/coroution.h"
#include "HPSocket/HPSocket.h"
#include "util/system.h"
/// <summary>
/// 协程线程池
/// </summary>
struct co_thread_pool_param {
    void* param = nullptr;
    ylib::co::scheduler* scheduler = nullptr;
    std::function<void()> callback;
};


VOID __HP_CALL co_thread_pool_callback(PVOID pvArg)
{
    auto cp = (co_thread_pool_param*)pvArg;
    cp->callback();
    delete cp;
}
class co_thread_pool
{
public:
    
public:
    co_thread_pool()
    {
        m_pool = HP_Create_ThreadPool();
    }
    ~co_thread_pool()
    {
        HP_Destroy_ThreadPool(m_pool);
    }
    bool start(uint32 thread_size)
    {
        stop();
        return m_pool->Start(thread_size, 0, TRP_WAIT_FOR);
    }
    void stop()
    {
        m_pool->Stop(INFINITE);
    }
    bool push(std::function<void()> callback, ylib::co::scheduler* sche)
    {
        co_thread_pool_param* cp = new co_thread_pool_param();
        cp->callback = callback;
        cp->param = nullptr;
        cp->scheduler = sche;
        return m_pool->Submit(co_thread_pool_callback,(PVOID)cp, INFINITE);
    }


    void on_worker_end(std::function<void(void*)> callback) { m_on_worker_end = callback; }
    friend class thread_pool_listener;
private:
    std::function<void(void*)> m_on_worker_end;
    IHPThreadPool* m_pool = nullptr;
};


ylib::co::coroutine co_function(const std::function<void(void*,ylib::co::scheduler*)> callback,void* param, ylib::co::scheduler* scheduler)
{
    callback(param, scheduler);
    co_await std::suspend_always{};
}

ylib::co::scheduler::scheduler()
{
    m_pool = new co_thread_pool();
}
ylib::co::scheduler::~scheduler()
{
    m_pool->stop();
    delete m_pool;
}
bool ylib::co::scheduler::start(uint32 thread_size)
{
    if (!m_pool->start(thread_size))
        return false;
    ::ithread::start();
    return true;
}
void ylib::co::scheduler::stop()
{
    m_pool->stop();
    ::ithread::stop();
    ::ithread::wait();
}
void ylib::co::scheduler::push(const task_info& info)
{
    m_queue.push(info);
}
void ylib::co::scheduler::push_t(std::function<void()> callback)
{
    m_pool->push(callback,this);
}
void ylib::co::scheduler::resume(std::coroutine_handle<>* continuation)
{
    task_info ti;
    ti.coco = continuation;
    m_queue.push(ti);
}
bool ylib::co::scheduler::run()
{
    // 处理任务
    while (m_queue.size() != 0)
    {
        exec_queue();
    }
    system::sleep_msec(1);
    return true;
}

void ylib::co::scheduler::exec_queue()
{
    task_info ti;
    while (m_queue.pop(ti))
    {
        if (ti.coco == nullptr)
        {
            //创建协程
            auto coro = co_function(ti.callback, ti.param, this);
            coro.coro.resume();
        }
        else
        {
            //唤醒协程
            ti.coco->resume();
        }
        
    }
}
#endif
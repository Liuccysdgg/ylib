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

#include "util/thread.h"
#include <string.h>
#include "util/system.h"
#include "util/system.h"
void ylib_thread_thread(void* lpParam)
	{
		auto t = (ylib::ithread*)lpParam;
		t->__thread_handle(lpParam);
    }
    ylib::ithread::ithread()
	{
		m_state = 2;
		m_thread = false;
	}
    ylib::ithread::~ithread()
	{

	}
    bool ylib::ithread::start()
	{
		if (this->m_state == 0)
			return false;
		else if (this->m_state == 1)
			return false;
		//struct NewobjThreadParam
		//{

		//};
		/*启动线程*/
		m_thread = true;

        std::thread t(ylib_thread_thread,this);
        t.detach();
		
		this->m_state = 0;
		return true;
	}
    void ylib::ithread::wait()
	{
		//timestamp start_time = sys::GetNowMSec();
		while (m_thread == true)
		{
			system::sleep_msec(1);
		}
		this->m_state = 2;
	}
    bool ylib::ithread::stop()
	{
		this->m_state = 1;
		return true;
	}
	bool ylib::ithread::wait_stop()
	{
		return this->m_state == 1;
	}
	void ylib::ithread::sleep(int32 msec)
	{
		// 10000 
		while(msec>=0)
		{
			system::sleep_msec(10);
			msec -= 10;
			if (this->m_state == 1)
				break;
		}
	}
    void* ylib::ithread::__thread_handle(void* param)
	{
		ylib::ithread* t = (ylib::ithread*)param;
		t->m_thread = true;
		while (true)
		{
			if (t->m_state == 1)
			{
				t->m_state = 2;
				break;
			}
			else
			{
				if (t->run() == false)
				{
					t->m_state = 2;
					break;
				}
			}
		}
		t->m_thread = false;
		return NULL;
	}
#if LIB_HPSOCKET == 1
	thread_pool::thread_pool()
	{
		m_hp_thread =  (void*)HP_Create_ThreadPool();
	}

	thread_pool::~thread_pool()
	{
		HP_Destroy_ThreadPool((IHPThreadPool*)m_hp_thread);
	}

	bool ylib::thread_pool::start(uint32 thread_size)
	{
		return false;
	}

	void ylib::thread_pool::stop()
	{
	}

	bool thread_pool::start(uint32 thread_count, uint32 max_queue_size)
	{
		return ((IHPThreadPool*)m_hp_thread)->Start(thread_count, max_queue_size, TRP_CALL_FAIL);
	}
	VOID __HP_CALL TaskProc_function_thread_pool(PVOID pvArg) {
		newobj_thread_pool_param* p = (newobj_thread_pool_param*)pvArg;
		p->pool->__thread_handle(p);
		delete p;
	}
	bool thread_pool::submit(void* param)
	{
		newobj_thread_pool_param* p = new newobj_thread_pool_param;
		p->param = param;
		p->pool = this;
		if (((IHPThreadPool*)m_hp_thread)->Submit(TaskProc_function_thread_pool, (PVOID)p) == false)
		{
			delete p;
			return false;
		}
		return true;
	}
	void thread_pool::__thread_handle(void* param)
	{
		((newobj_thread_pool_param*)param)->pool->run_pool(((newobj_thread_pool_param*)param)->param);
	}
#endif

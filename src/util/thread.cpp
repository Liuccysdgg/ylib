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

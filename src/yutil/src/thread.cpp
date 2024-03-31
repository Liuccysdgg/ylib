#include "yutil/thread.h"
#include <string.h>
#include "yutil/system.h"
#include "yutil/system.h"
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

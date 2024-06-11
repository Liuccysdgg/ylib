#pragma once
#include "base/define.h"
#include "base/error.h"
namespace ylib
{
	/// <summary>
	/// 共享内存
	/// </summary>
	class sharedmem:public ylib::error_base
	{
	public:
		/// <summary>
		/// 自动销毁
		/// </summary>
		/// <param name="auto_free"></param>
		sharedmem(bool auto_free = false);
		~sharedmem();
		/// <summary>
		/// 创建
		/// </summary>
		/// <param name="name"></param>
		/// <param name="size"></param>
		/// <returns></returns>
		bool create(const std::string& name,size_t size);
		/// <summary>
		/// 销毁
		/// </summary>
		void destory();


		void* mem() const { return m_memory; }


		uchar operator[](size_t );
	private:
		std::string m_name;
		size_t m_size = 0;
		void* m_memory = nullptr;
		bool m_auto_free = false;
#ifdef _WIN32
		void* m_handle = 0;
#else
		int m_handle = -1;
#endif
	};
}
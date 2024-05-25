//#pragma once
//#include <string>
//#include <functional>
//#include <Kaguya/kaguya.hpp>
//#include "ybase/buffer.h"
//#include "yutil/file.h"
//class NetLua
//{
//public:
//	NetLua();
//	~NetLua();
//	// 启动
//	bool start(
//		const std::string& dirpath,
//		std::function<void(kaguya::State* L)> regist_callback,
//		const std::string& init_lua_filepath,
//		std::string error_filepath
//
//	);
//	void close();
//private:
//	void addLuaPath();
//private:
//	kaguya::State *L = nullptr;
//	ylib::file_io m_error;
//	std::string m_dirpath;
//};
//

#pragma once
#include <QtWidgets/qwidget.h>
#include <string>
#include <functional>
#include <Kaguya/kaguya.hpp>
#include "ybase/buffer.h"
#include "yutil/file.h"
class QsLua
{
public:
	QsLua();
	~QsLua();
	// 启动
	bool start(
		std::function<void(kaguya::State* L)> regist_callback,
		const std::string& init_lua_filepath,
		std::string error_filepath

	);
	void close();
	// 制作LUA包
	ylib::buffer package();
	// 释放LUA包
	void unpack(const ylib::buffer& data);
private:
	void addLuaPath();
private:
	kaguya::State *L = nullptr;
	ylib::file_io m_error;
};


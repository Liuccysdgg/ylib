//#include "ynetlua/NetLua.h"
//#include "lua/lua.h"
//#include <fstream>
//#include "yutil/system.h"
//#include "yutil/file.h"
//#include "yutil/codec.h"
//#include "yutil/process.h"
//#include "yutil/time.h"
//
//#include "ynetlua/HttpCenter.h"
//
//NetLua::NetLua()
//{
//
//}
//
//NetLua::~NetLua()
//{
//    m_error.close();
//}
//bool NetLua::start(
//    const std::string& dirpath,
//    std::function<void(kaguya::State* L)> regist_callback,
//    const std::string& init_lua_filepath,
//    std::string error_filepath
//)
//{
//    m_dirpath = dirpath;
//    if (L != nullptr)
//        return false;
//    L = new kaguya::State();
//
//    
//    // 设置模块加载目录
//    addLuaPath();
//
//    // 导出错误信息到文件
//    {
//
//    }
//    // 注册功能
//    HttpCenter::regist(*L);
//    regist_callback(L);
//    if (m_error.open(error_filepath))
//    {
//        L->setErrorHandler([&](int statuscode, const char* message) {
//            std::string msg = time::now_time() + " [StatusCode]:" + std::to_string(statuscode) + "\r\n" + std::string(message) + "\r\n";
//            m_error.appead(msg);
//            std::cout << msg << std::endl;
//            });
//    }
//    return L->dofile(dirpath + "\\" + init_lua_filepath);
//}
//void NetLua::addLuaPath()
//{
//    // 获取当前的package.path
//    lua_getglobal(L->state(), "package");
//    lua_getfield(L->state(), -1, "path");
//    std::string currentPath = lua_tostring(L->state(), -1);
//    lua_pop(L->state(), 1);
//    // 添加新的路径 
//    currentPath.append(";");
//    currentPath.append(m_dirpath);
//    currentPath.append("\\?.lua");  // ?.lua是一个占位符，Lua将用文件名替换?
//    // 设置新的package.path
//    lua_pushstring(L->state(), currentPath.c_str());
//    lua_setfield(L->state(), -2, "path");
//    lua_pop(L->state(), 1);  // 弹出package表
//}
//void NetLua::close()
//{
//    if (L != nullptr)
//    {
//        delete L;
//    }
//    L = nullptr;
//}
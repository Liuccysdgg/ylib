#include "yqlua/QsLua.h"
#include "lua/lua.h"
#include <fstream>
#include "yutil/system.h"
#include "yutil/file.h"
#include "yutil/codec.h"
#include "yutil/process.h"
#include "yutil/time.h"

#include "yqlua/QsApp.h"
#include "yqlua/QsWidget.h"
#include "yqlua/QsButton.h"
#include "yqlua/QsLabel.h"
#include "yqlua/QsTreeWidget.h"
#include "yqlua/QsHeaderView.h"
#include "yqlua/QsSkin.h"
#include "yqlua/QsCheckBox.h"
#include "yqlua/QsImage.h"
#include "yqlua/QsTableWidget.h"
#include "yqlua/QsLineEdit.h"
#include "yqlua/QsProgressbar.h"
#include "yqlua/QsComBobox.h"
#include "yqlua/QsIcon.h"
#include "yqlua/QsScrollBar.h"
#include "yqlua/QsHBoxLayout.h"
#include "yqlua/QsVBoxLayout.h"
#include "yqlua/QsListWidget.h"
#include "yqlua/QsTimer.h"
#include "yqlua/QsMenu.h"
#include "yqlua/QsSystemTrayIcon.h"
#include "yqlua/QsMovie.h"



#include "yqlua/QsIni.h"
#include "yqlua/QsCodec.h"
#include "yqlua/QsFile.h"
#include "yqlua/QsSystem.h"
#include "yqlua/QsHttpClient.h"
#include "yqlua/QsBuffer.h"
#include "yqlua/QsProcess.h"


#ifdef _DEBUG
#define PATH_LUAC system::current_dir()+"\\luac"
#else
#define PATH_LUAC system::current_dir()+"\\lua"
#endif
#define PATH_LUA system::current_dir()+"\\lua"
QsLua::QsLua()
{
    
}

QsLua::~QsLua()
{
    m_error.close();
}
bool QsLua::start(
    std::function<void(kaguya::State* L)> regist_callback,
    const std::string& init_lua_filepath,
    std::string error_filepath
)
{
    if (L != nullptr)
        return false;
    L = new kaguya::State();
    // 设置模块加载目录
    addLuaPath();

    // 导出错误信息到文件
    {

    }
    // 注册功能
    QsApp::regist(*L);
    QsWidget::regist(*L);
    QsButton::regist(*L);
    QsLabel::regist(*L);
    QsTreeWidget::regist(*L);
    QsHeaderView::regist(*L);
    QsSkin::regist(*L);
    QsCheckBox::regist(*L);
    QsImage::regist(*L);
    QsTableWidget::regist(*L);
    QsLineEdit::regist(*L);
    QsProgressbar::regist(*L);
    QsComBobox::regist(*L);
    QsIcon::regist(*L);
    QsScrollBar::regist(*L);
    QsHBoxLayout::regist(*L);
    QsVBoxLayout::regist(*L);
    QsListWidget::regist(*L);
    QsTimer::regist(*L);
    QsSystemTrayIcon::regist(*L);
    QsMenu::regist(*L);
    QsAction::regist(*L);
    QsMovie::regist(*L);
    
    QsSystem::regist(*L);
    QsCodec::regist(*L);
    QsFile::regist(*L);
    QsHttpClient::regist(*L);
    QsBuffer::regist(*L);
    QsIni::regist(*L);
    QsProcess::regist(*L);

    regist_callback(L);
    if (m_error.open(error_filepath))
    {
        L->setErrorHandler([&](int statuscode, const char* message) {
            std::string msg = time::now_time() + " [StatusCode]:" + std::to_string(statuscode) + "\r\n" + std::string(message) + "\r\n";
            m_error.appead(msg);
            std::cout << msg << std::endl;
        });
    }
    return L->dofile(PATH_LUAC + "\\"+ init_lua_filepath);
}
ylib::buffer QsLua::package()
{

    // 创建LUAC目录
    ylib::file::create_dir(PATH_LUAC);
    ylib::process::create(system::current_dir() + "\\LuaEncryption.exe ", system::current_dir(), { PATH_LUA , PATH_LUAC }, true);
    std::map<std::string, bool> list;
    ylib::file::list(PATH_LUAC, list);
    ylib::json packJson;
    for_iter(iter, list)
    {
        if (iter->second)
            continue;
        if (ylib::file::ext(iter->first) != "lua")
            continue;
        packJson["lua"][iter->first]["name"] = iter->first;
        packJson["lua"][iter->first]["body"] = codec::base64::en(ylib::file::read(PATH_LUAC + "\\" + iter->first));
    }
    return codec::gzip::en(packJson.to_string());
}

void QsLua::unpack(const ylib::buffer& data)
{
    // 创建LUAC目录
    ylib::file::create_dir(PATH_LUAC);
    ylib::json packJson = ylib::json::from(codec::gzip::de(data));
    std::vector<std::string> luas = packJson["lua"].keys();
    for (size_t i = 0; i < luas.size(); i++)
    {
        std::string name = PATH_LUAC + "\\" + packJson["lua"][luas[i]]["name"].to<std::string>();
        ylib::buffer data = codec::base64::de(packJson["lua"][luas[i]]["body"].to<std::string>());
        ylib::file::write(name,data);
    }
        
}
void QsLua::addLuaPath()
{
    // 获取当前的package.path
    lua_getglobal(L->state(), "package");
    lua_getfield(L->state(), -1, "path");
    std::string currentPath = lua_tostring(L->state(), -1);
    lua_pop(L->state(), 1);
    // 添加新的路径 
    currentPath.append(";");
    currentPath.append(PATH_LUAC);
    currentPath.append("\\?.lua");  // ?.lua是一个占位符，Lua将用文件名替换?
    // 设置新的package.path
    lua_pushstring(L->state(), currentPath.c_str());
    lua_setfield(L->state(), -2, "path");
    lua_pop(L->state(), 1);  // 弹出package表
}
void QsLua::close()
{
    if (L != nullptr)
    {
        delete L;
    }
    L = nullptr;
}
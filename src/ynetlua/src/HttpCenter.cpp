//#include "ynetlua/HttpCenter.h"
//#include "ynet/http_center.h"
//#include "ynet/http_website.h"
//
//#include "ynet/http_router.h"
//#include "yutil/system.h"
//#include "yutil/strutils.h"
//
//#include "ynetlua/NetLua.h"
//static network::http::center s_center;
//HttpCenter::HttpCenter()
//{
//}
//
//HttpCenter::~HttpCenter()
//{
//} 
//
//void HttpCenter::regist(kaguya::State& T)
//{
//    T["HttpCenter"].setClass(kaguya::UserdataMetatable<HttpCenter>()
//        .addStaticFunction("start", &HttpCenter::start)
//        .addStaticFunction("stop", &HttpCenter::stop)
//        .addStaticFunction("create", &HttpCenter::create)
//    );
//}
//
//bool HttpCenter::start()
//{
//    return s_center.start();
//}
//
//void HttpCenter::stop()
//{
//    s_center.close();
//}
//
//bool HttpCenter::create(int port)
//{
//    stop();
//    ylib::json config = ylib::json::from("{\"website\":[{\"host\":[{\"host\":\"0.0.0.0:" + std::to_string(port) + "\",\"ssl\":false}],\"router\":{\"threadpool\":{\"size\":1,\"queuemax\":10000}},\"session\":{\"timeout_sec\":86400}}]}");
//    s_center.create(config);
//    auto website = s_center.website("0.0.0.0");
//    auto router = website->router();
//    website->router()->other([&](network::http::request* request, network::http::response* response) {
//        auto path = request->filepath();
//        if (strutils::left(path, 5) == "/api/")
//        {
//            execLua(request,response);
//            return;
//        }
//        if (path == "/")
//            path = "/index.html";
//        response->send_file(system::current_dir()+"\\www"+ path, -1, false);
//    });
//    return s_center.start();
//}
//
//std::string HttpCenter::last_error()
//{
//    return s_center.last_error();
//}
//
//void HttpCenter::execLua(network::http::request* request, network::http::response* response)
//{
//    std::string filepath = request->filepath();
//    std::string luapath = strutils::right(filepath, filepath.length() - 5) + ".lua";
//    filepath = system::current_dir() + "\\lua\\" + luapath;
//    if (ylib::file::exist(filepath) == false)
//    {
//        response->send("Not Found: "+filepath,404,"Not Found");
//        return;
//    }
//
//    NetLua lua;
//    // 启动
//    if (!lua.start(system::current_dir()+"\\lua", [&](kaguya::State* L) {
//        },
//        filepath,
//        "error_msg.txt"
//        ))
//    {
//        std::cout << "[lua engine] start failed";
//        return;
//    }
//    std::cout << "[lua engine] start success";
//    return;
//}

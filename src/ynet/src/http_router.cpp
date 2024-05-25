#include "ynet/http_router.h"
#if USE_NET_HTTP_WEBSITE
#include <regex>
#include "ynet/http_reqpack.h"
#include "ynet/http_request.h"
#include "ynet/http_response.h"
#include "ynet/http_server.h"
#include "ynet/http_center.h"
#include "ynet/http_controller.h"
#include "ynet/http_interceptor.h"
#include "ynet/http_website.h"
#include "ynet/http_agent.h"
#include "ynet/http_cache.h"
#include "ynet/http_cdn.h"
#include "HPSocket/HPSocket.h"
#include "yutil/system.h"
#if HTTP_LUA_ENGINE == 1
#include "httplua/Lrequest.h"
#include "httplua/Lresponse.h"
#endif
ylib::network::http::router::router()
{
    
    m_threadpool = nullptr;
    m_interceptor = new network::http::interceptor;
}

ylib::network::http::router::~router()
{
    close();
    delete m_interceptor;
}

bool network::http::router::start(const router_config &config)
{
    m_config = config;
    //启动线程池
    {
        if (m_config.threadpool.size > 1)
        {
            m_threadpool = HP_Create_ThreadPool();
            if (this->m_threadpool->Start(
                m_config.threadpool.size,
                m_config.threadpool.queuemax,
                m_config.threadpool.queuemax == 0 ? TRP_WAIT_FOR : TRP_CALL_FAIL, 0) == false)
            {
                //销毁Socket任务线程池
                HP_Destroy_ThreadPool(this->m_threadpool);
                this->m_threadpool = nullptr;
                m_lastErrorDesc =  "threadpool init failed";
                return false;
            }
        }
    }
    ::ithread::start();
    return true;

}

void network::http::router::close()
{
    ::ithread::stop();
    ::ithread::wait();
    if (this->m_threadpool != nullptr)
    {
        this->m_threadpool->Stop(-1);
        delete this->m_threadpool;
        this->m_threadpool = nullptr;
    }
    
}
network::http::interceptor* network::http::router::interceptor(){
    m_interceptor->center(center());
    return m_interceptor;
}

void network::http::router::subscribe(const std::string &path, network::http::method method, std::function<void(network::http::request*,network::http::response*)> callback)
{
#if HTTP_ROUTER_PRINT == 1
    ylib::log->info("[subscribe][func] express:"+path+" method:"+method_to_string(method),"router");
#endif
    network::http::subscribe_info *svie = new network::http::subscribe_info;
    svie->controller = false;
    svie->method = method;
    svie->express = std::regex(path.c_str());
    svie->callback = callback;
    m_subscribe.append(svie);
}
void network::http::router::subscribe(
    std::function<void* ()> create_controller_callback,
    network::http::HTTP_CTR_FUNCTION function,
    std::string path,
    network::http::method method
)
{
#if HTTP_ROUTER_PRINT == 1
    ylib::log->info("[subscribe][ctl] express:"+path+" method:"+method_to_string(method),"router");
#endif
    network::http::subscribe_info *svie = new network::http::subscribe_info;
    svie->controller = true;
    svie->method = method;
    svie->express = std::regex(path.c_str());
    svie->create_controller_callback = create_controller_callback;
    svie->controller_function = function;
    m_subscribe.append(svie);
}
#if HTTP_LUA_ENGINE == 1
void ylib::network::http::router::subscribe(const std::string& lua_filepath, std::string path, network::http::method method, std::function<void(sol::state& state)> init_state)
{
#if HTTP_ROUTER_PRINT == 1
    ylib::log->info("[subscribe][ctl][lua] express:" + path + " method:" + method_to_string(method), "router");
#endif
    network::http::subscribe_info* svie = new network::http::subscribe_info;
    svie->controller = false;
    svie->method = method;
    svie->express = std::regex(path.c_str());
    svie->lua_filepath = lua_filepath;
    svie->lua_init_state = init_state;
    m_subscribe.append(svie);
}
#endif
void network::http::router::other(std::function<void(network::http::request*,network::http::response*)> callback)
{
    this->m_callback_other = callback;
}
void ylib::network::http::router::on_recved(std::function<void(const ylib::buffer &begin, ylib::buffer* end)> callback)
{
    m_callback_recved = callback;
}
void ylib::network::http::router::on_sendbefore(std::function<void(const ylib::buffer &begin, ylib::buffer* end)> callback)
{
    m_callback_sendbefore = callback;
}
VOID __HP_CALL TaskProc_function(PVOID pvArg)
{
    network::http::router::thread_param_info* tp_info = (network::http::router::thread_param_info*)pvArg;
    tp_info->router->__thread_callback(tp_info->reqpack);

    delete tp_info->reqpack;
    delete tp_info;
}
void ylib::network::http::router::__thread_callback(reqpack* rp)
{
    
    /*======== ======CDN服务==============*/
    if (is_cdn(rp))
        return;
    /*rp->response()->send((nstring)"OK");
    return;*/
    /*======== ======缓存判断==============*/
    if(rp->website()->cache()->have_send(rp))
    {
        ylib::log->info("cached,"+rp->filepath(),"router");
        return;
    }
    /*======== ======代理请求==============*/
    if (is_proxy(rp))
        return;
    /*===============正常请求=================*/
    //拦截器过滤
    try
    {
        if (m_interceptor->trigger(rp->filepath(), rp) == false)
        {
            // 已拒绝继续执行
            return;
        }
    }
    catch (const std::exception& e)
    {
#if HTTP_ROUTER_PRINT == 1
        // 通用异常返回
        ylib::log->error("(interceptor)business processing exception:" + std::string(e.what()) + ", url:" + rp->filepath() + " ip:" + rp->request()->remote_ipaddress(true), "router");
#endif
        rp->response()->send((std::string)e.what(), 500, "Internal Server Error");
    }
    bool execed = false;
    for(size_t i=0;i<m_subscribe.m_count;i++){
        auto sub = m_subscribe.get(i);
        if (std::regex_match(rp->filepath().c_str(),sub->express) && (sub->method == rp->method() || sub->method == network::http::ALL))
        {
            execed = true;
            try
            {
                //开始回调
                if (sub->controller)
                {
                    //controller 类型回调
                    std::unique_ptr<network::http::controller> controller((network::http::controller*)sub->create_controller_callback());
                    controller->m_reqpack = rp;
                    controller->center(center());
                    //调用处理函数
                    (controller.get()->*sub->controller_function)();
                }
                else
                {
                    //普通回调
                    if (sub->callback != nullptr)
                    {
                        sub->callback(rp->request(), rp->response());
                    }
                    else
                    {
#if HTTP_LUA_ENGINE == 1
                        lua_engine(rp,*sub);
#endif
                    }
                }
#if HTTP_ROUTER_PRINT == 1
                ylib::log->info("["+rp->exec_msec()+" ms] controller url:"+rp->filepath()+" ip:"+rp->request()->remote_ipaddress(true),"router"); 
#endif
            }
            catch (const std::exception& e)
            {
#if HTTP_ROUTER_PRINT == 1
                // 通用异常返回
                ylib::log->error("business processing exception:" + std::string(e.what()) + ", url:" + rp->filepath()+" ip:"+rp->request()->remote_ipaddress(true),"router");
#endif
                rp->response()->send((std::string)e.what(), 500, "Internal Server Error");
            }
            break;

        }
        
    }
    if(execed == false){
        //其它回调
        if (m_callback_other != nullptr) {
            m_callback_other(rp->request(), rp->response());
        }else{ 
            std::string filepath = rp->request()->filepath();
            if(filepath == "/")
                filepath = "/index.html";
            if(rp->response()->send_file(filepath) == false){
                rp->response()->send((std::string)"404 Not found",404,"Not Found");
            }
        }
    }
}
size_t ylib::network::http::router::queue_size()
{
    return m_handle_queue.size();
}
void ylib::network::http::router::push(reqpack *rp)
{
    m_handle_queue.push(rp);
}


bool ylib::network::http::router::is_cdn(reqpack* rp)
{
    auto cdn = rp->website()->cdn();
    if(cdn->config().enable == false)
        return false;
    if(cdn->config().manager == false)
        return false;
    std::string move_url = cdn->host();
    if(move_url.empty()){
        rp->response()->send((std::string)"CDN: Currently no available nodes",503,"Service Unavailable");
        return true;
    }
    move_url.append(rp->filepath());
    rp->response()->redirect(move_url,true);
    return true;
}
#if HTTP_LUA_ENGINE == 1
void ylib::network::http::router::lua_engine(reqpack* rp, const network::http::subscribe_info& sub)
{
    sol::state lua;
    if (sub.lua_init_state != nullptr)
    {
        sub.lua_init_state(lua);
    }
    
    try {
        lua.open_libraries(sol::lib::base);
        sol::protected_function_result result = lua.script_file(sub.lua_filepath, sol::script_pass_on_error);  // 尝试加载并执行脚本
        if (!result.valid()) {
            sol::error err = result;
            throw ylib::exception(err.what());
        }
        Lrequest request(rp->request());
        Lresponse response(rp->response());
        request.regist(&lua);
        response.regist(&lua);

        result = lua["access"](request, response);
        if (!result.valid()) {
            sol::error err = result;
            throw ylib::exception(err.what());
        }
    }
    catch (const sol::error& err) {
        throw ylib::exception(err.what());
    }
}
#endif
bool ylib::network::http::router::is_proxy(reqpack* rp)
{
#if USE_NET_HTTP_AGENT == 1
    //获取请求首行，判断代理
    auto proxys = rp->website()->proxy();
    for (size_t i = 0; i < proxys->m_count; i++)
    {
        network::http::proxy* proxy = proxys->get(i);

        if (std::regex_match(rp->filepath().c_str(), proxy->src_express))
        {
            rp->server()->agent()->request(3000, rp, proxy);
            return true;
        }
    }
#endif
    return false;
}
bool ylib::network::http::router::run()
{
    //std::string time_output = time::now_time();
    //std::cout << "Debug(1):\t" << time_output << std::endl;
    reqpack* rp = nullptr;
    while (m_handle_queue.pop(rp))
    {
        if (m_threadpool == nullptr)
        {
#if 0
            ((IHttpServer*)rp->server()->hpserver())->SendResponse(rp->connid(), 200, "OK", nullptr, 0, (const BYTE*)"AAA", 3);
            return true;
#endif
            // 单线程
            this->__thread_callback(rp);
            delete rp;
        }
        else
        {
            //thread_param_info* tp_info = new thread_param_info();//m_threadparam_queue->create();
            thread_param_info* tp_info = new thread_param_info;
            tp_info->reqpack = rp;
            tp_info->router = this;
            if (m_threadpool->Submit(TaskProc_function, (PVOID)tp_info) == FALSE)
            {
                //线程池繁忙
                std::string ipaddress;
                ushort port = 0;
                tp_info->reqpack->server()->remote(rp->connid(), ipaddress, port);
#if HTTP_ROUTER_PRINT == 1
                ylib::log->error("Thread posting failed,Remote: "+ ipaddress+",Filepath: " + rp->filepath(),"router");
#endif
                // 发送繁忙回复
                ((IHttpServer*)tp_info->reqpack->server()->hpserver())->SendResponse((CONNID)rp->connid(), 503, "Service Unavailable");
                // 销毁资源
                // 
                delete tp_info->reqpack;
                delete tp_info;
            }
        } 
     
    }
    //std::cout << "Debug(2.1):\t" << time_output << std::endl;
    //Sleep(1000);
    ylib::system::sleep_msec(1);
    //std::cout << "Debug(2):\t" << time_output << std::endl;
    return true;
}
#endif
 
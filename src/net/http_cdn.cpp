#include "net/http_cdn.h"
#include "net/http_website.h"
#include "net/http_router.h"
#include "net/http_reqpack.h"
#include "net/http_response.h"
#include "net/http_request.h"
network::http::cnode::cnode(const cdn_config::node_config& config){

    m_reply_wait_msec = 0;
    m_send_bytes_sec = 0;
    m_recv_bytes_sec = 0;
    m_update_sec = 0;
    m_net_status = false;
    m_max_band = 0;


    m_mang_url = "http://"+ config.mang_domain+"/info";
}
network::http::cnode::~cnode(){}

int network::http::cnode::score(){
    if(m_net_status == false)
        return 0;
    return (int)((double)100 - ((double)m_send_bytes_sec / (double)m_max_band * 100));
}
bool network::http::cnode::update(){
    timestamp start_msec = time::now_msec();
    m_client.headers_request().set("key",m_config.key);
    if(m_client.get(m_mang_url) == false){
        m_net_status = false;
        m_net_errormsg = "connect failed";
        m_lastErrorDesc = m_net_errormsg;
        return false;
    }
    m_reply_wait_msec = time::now_msec() -  start_msec;
    ylib::json rep = ylib::json::from(m_client.response().to_string());
    //m_client.response().to_string().println();
//    std::cout<<rep.to_string(true).c_str()<<std::endl;
    if(rep["result"].to<bool>() == false){
        m_net_status = false;
        m_net_errormsg = rep["msg"].to<std::string>();
        m_lastErrorDesc = m_net_errormsg;
        return false;
    }
    m_recv_bytes_sec = rep["down_sec_byte"].to<uint64>();
    m_send_bytes_sec = rep["up_sec_byte"].to<uint64>();
    m_max_band = rep["max_band"].to<uint64>();
    m_update_sec = time::now_sec();
    m_net_status = true;
    return true;
}

network::http::cdn::cdn()
{
    m_cdn_node_max_band = 0;
}
network::http::cdn::~cdn()
{
    ::ithread::stop();
    ::ithread::wait();
    for(size_t i=0;i<m_nodes.size();i++)
    {
        delete m_nodes[i];
    }
}
bool network::http::cdn::start(const cdn_config& config){
    m_config = config;

    if(m_config.enable == false)
        return true;
    if(m_config.manager)
    {
        std::cout<<"CDN:Manager start"<<std::endl;
        for(size_t i=0;i< m_config.node.size();i++) 
        {
            auto n = new cnode(m_config.node[i]);
            m_nodes.push_back(n);
        }

    }
    else
    {
        std::cout<<"CDN:Node start"<<std::endl;
        website()->router()->subscribe("/info",network::http::GET,[&](network::http::request* request,network::http::response* response,void* extra){
            ylib::json rep;
            std::string value_key;
            if(request->header("key",value_key) == false || m_cdn_node_key != value_key){
                rep["result"] = false;
                rep["msg"] = "key is empty or incorrect";
                response->send(rep);
                return;
            }

            rep["result"] = true;
            rep["down_all"] = m_cdn_node_bandinfo.down_all;
            rep["down_sec_byte"] = m_cdn_node_bandinfo.down_sec_byte;
            rep["up_all"] = m_cdn_node_bandinfo.up_all;
            rep["up_sec_byte"] = m_cdn_node_bandinfo.up_sec_byte;
            rep["max_band"] = m_cdn_node_max_band;
            response->send(rep);
        });

    }
    ::ithread::start();
    return true;
}
void ylib::network::http::cdn::close()
{
    ::ithread::stop();
    ::ithread::wait();
    m_nodes.clear();
}
std::string network::http::cdn::host(){
    int32 score = 0;
    std::string result_host;
    for(size_t i=0;i<m_nodes.size();i++)    
    {
        if(m_nodes[i]->score() > score){
            result_host = m_nodes[i]->config().host;
            score = m_nodes[i]->score();
        }
    }
    return result_host;
}
bool network::http::cdn::run(){
    if(m_config.manager){
        for(size_t i=0;i<m_nodes.size();i++)
        {
            if(m_nodes[i]->update() == false){
                std::cout<<m_nodes[i]->last_error()<<std::endl;
            }
        }

    }
    else
    {
#ifndef _WIN32
        //m_cdn_node_bandinfo = system::bandinfo();
#endif
    }
    system::sleep_msec(1000);
    return true;
}


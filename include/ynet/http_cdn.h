#pragma once
#include "http_define.h"
#include "http_reqpack.h"
#include "http_interface.h"
#include "http_client_plus.h"

#include "yutil/thread.h"
namespace ylib::network::http
{
    class cnode:public ylib::error_base
    {
    public:
        cnode(const ylib::json& config);
        ~cnode();
        const std::string& host();
        int score();
        bool update();
    private:
        network::http::client_plus m_client;
    private:
        // 名称
        std::string m_name;
        // 域名
        std::string m_host;
        // 管理密钥
        std::string m_key;
        // 管理地址
        std::string m_mang_url;
        // 回复耗时
        timestamp m_reply_wait_msec;
        // 当前带宽上传
        uint64 m_send_bytes_sec;
        // 当前带宽下载
        uint64 m_recv_bytes_sec;
        // 更新时间
        timestamp m_update_sec;
        // 通信状态
        bool m_net_status;
        // 通信状态错误描述
        std::string m_net_errormsg;
        // 最大带宽
        uint64 m_max_band;
    };

    class cdn:public network::http::interface_,public ylib::error_base,private ylib::ithread
    {
    public:
        cdn();
        ~cdn();
        bool start(const ylib::json& config);
        std::string host();
        virtual bool run() override;
        inline bool enable(){return m_enable;};
        inline bool manager(){return m_manager;};
    private:
        bool m_enable;
        bool m_manager;
        std::vector<network::http::cnode*> m_nodes;


        std::string m_cdn_node_key;
        uint64 m_cdn_node_max_band;
        ylib::BandInfo m_cdn_node_bandinfo;
    };

}

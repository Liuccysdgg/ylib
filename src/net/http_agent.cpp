// MIT License﻿
// Copyright(c) 2024 FastWeb - fwlua.com - nianhua﻿
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :﻿
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.﻿
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.﻿
// ## Additional Terms for Commercial Use﻿
// This software is licensed for personal, educational, and non - commercial use.
// For commercial use or use within a company, organization, or institution, a
// separate commercial license is required.To obtain a commercial license,
// please contact
// EMail：1585346868@qq.com
// Mobile：17367918735
// QQ：1585346868

#include "net/http_agent.h"
#if USE_NET_HTTP_WEBSITE
#include <string.h>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

#include "HPSocket/HPSocket.h"
#include "HPSocket/HPSocket-SSL.h"

#include "util/system.h"
#include "util/time.h"
#include "util/map.hpp"

#include "util/file.h"
#include "util/strutils.h"
#include "net/http_website.h"
#include "net/http_server.h"
#include "net/http_request.h"
#include "net/util.h"
#include "net/http_cache.h"
#include "net/http_reqpack.h"
#include "net/http_define.h"
/*附加数据*/
class http_agent_extra
{
public:
	// 请求体
	struct request
	{
		request()
		{
		}
		~request()
		{
		}
        // 请求头
        std::vector<KeyValue<std::string,std::string>> request_headers;
        std::vector<KeyValue<std::string,std::string>> response_headers;
		// 请求路径
		std::string path;
		// 请求类型
		std::string method;
		// 请求数据
		ylib::buffer data;
		// HOST:PORT
		std::string url_host;
	};
	// 接收
	struct receive {
		receive() {
			transfer_encoding_length = -1;
		}
		// 回复数据
		ylib::buffer data;
		// check 类型数据头内容 默认=-1 即非该类型回复。
		int32 transfer_encoding_length;
	};
	http_agent_extra()
	{
		status_code = 0;
		connid = 0;
		server = nullptr;
		agent = nullptr;
	}

	void clear()
	{
		connid = 0;
		server = nullptr;
		agent = nullptr;
        cache.close();
        status_code = 0;
        status_desc.clear();
	}
	// server 客户连接ID
	uint64 connid;
	// 绑定server
	network::http::server* server;
	// 请求
	request req;
	// 接收
	receive recv;
	// 代理
	IHttpAgent* agent;
    // 缓存文件
    ylib::file_io cache;
    // 状态码
    uint32 status_code;
    // 状态描述
    std::string status_desc;
};
// 获取附加数据指针
inline http_agent_extra* __get_extra_to__(ITcpAgent* client, CONNID dwConnID){
    PVOID extra = 0;
    client->GetConnectionExtra(dwConnID, &extra);
    return (http_agent_extra*)extra;
}
#define GET_EXTRA __get_extra_to__(pSender,dwConnID)
#define SACONNID std::string("(S:"+std::to_string(GET_EXTRA->connid)+"|A["+std::to_string((uint64)pSender)+"]:"+std::to_string((uint64)dwConnID)+")")
class http_agent_listener :public IHttpAgentListener
{
	public:
	http_agent_listener()
	{
	}
	// 通过 IHttpAgentListener 继承
	virtual EnHttpParseResult OnMessageBegin(IHttpAgent* pSender, CONNID dwConnID) override
	{
	    #if HTTP_AGENT_DEBUG_PRINT == 1
	    ylib::log->info("OnMessageBegin "+SACONNID,"http_agent ");
	    #endif
        auto extra = GET_EXTRA;
		extra->recv.transfer_encoding_length = -1;
		extra->recv.data.clear();
		return HPR_OK;
	}
	virtual EnHttpParseResult OnRequestLine(IHttpAgent* pSender, CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszUrl) override
	{

//	    #if HTTP_AGENT_DEBUG_PRINT == 1
//	    ylib::log->info("OnRequestLine "+SACONNID,"http_agent ");
//	    #endif

		return HPR_OK;
	}
	virtual EnHttpParseResult OnStatusLine(IHttpAgent* pSender, CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc) override
	{
//	    #if HTTP_AGENT_DEBUG_PRINT == 1
//	    ylib::log->info("OnStatusLine "+SACONNID,"http_agent ");
//	    #endif
        http_agent_extra* extra = GET_EXTRA;
        extra->status_code = usStatusCode;
        extra->status_desc = lpszDesc;
		return HPR_OK;
	}
	virtual EnHttpParseResult OnHeader(IHttpAgent* pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue) override
	{
//	    #if HTTP_AGENT_DEBUG_PRINT == 1
//	    ylib::log->info("OnHeader "+SACONNID,"http_agent ");
//	    #endif

		return HPR_OK;
	}
	virtual EnHttpParseResult OnHeadersComplete(IHttpAgent* pSender, CONNID dwConnID) override
	{
	    #if HTTP_AGENT_DEBUG_PRINT == 1
	    ylib::log->info("OnHeadersComplete "+SACONNID,"http_agent ");
	    #endif
		http_agent_extra* extra = GET_EXTRA;
		IHttpServer* server = (IHttpServer*)extra->server->hpserver();
		THeader* local_header = nullptr;
		DWORD local_hsize = 0;
		// 取所有协议头
        pSender->GetAllHeaders(dwConnID, local_header, local_hsize);
        if (local_hsize == 0)
            return HPR_OK;
		
		// MEM Header Length
		size_t mem_header_length = local_hsize + extra->req.response_headers.size();
        local_header = new THeader[mem_header_length];
        pSender->GetAllHeaders(dwConnID, local_header, local_hsize);
		
		size_t push_index = local_hsize;
		for (size_t i = 0; i < extra->req.response_headers.size(); i++)
        {
            std::string response_key = strutils::change_case(extra->req.response_headers[i].name, false);
			bool find = false;
			for (size_t f = 0; f < local_hsize; f++)
			{
                if (response_key == strutils::change_case(local_header[f].name, false))
				{
					local_header[f].value = extra->req.response_headers[i].value.c_str();
					find = true;
					break;
				}
			}
			if (find == false)
			{
				local_header[push_index].name = extra->req.response_headers[i].name.c_str();
				local_header[push_index].value = extra->req.response_headers[i].value.c_str();
				push_index++;
			}
		}
		
		

		// 取请求方式
		server->SendResponse(
			(CONNID)extra->connid,
            extra->status_code,
            extra->status_desc.c_str(),
			local_header,
			(int)push_index);
        // 缓存
        {
            if(extra->cache.is_open() && extra->status_code == 200)
            {
                ylib::file_io header_file;
                header_file.open(extra->cache.filepath()+".header");
                for(size_t i=0;i< push_index;i++)
                {
                    header_file.write(local_header[i].name,strlen(local_header[i].name));
                    header_file.write("\r\n",2);
                    header_file.write(local_header[i].value,strlen(local_header[i].value));
                    header_file.write("\r\n",2);
                }
                header_file.close();
            }
        }
		delete[] local_header;
		return HPR_OK;
	}
	std::string dec2hex(int i) //将int转成16进制字符串
	{
		std::stringstream ioss; //定义字符串流
		std::string s_temp; //存放转化后字符
		ioss << std::setiosflags(std::ios::uppercase) << std::hex << i; //以十六制(大写)形式输出
		//ioss << resetiosflags(ios::uppercase) << hex << i; //以十六制(小写)形式输出//取消大写的设置
		ioss >> s_temp;
		return s_temp;
	}
	virtual EnHttpParseResult OnBody(IHttpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
	{
	    #if HTTP_AGENT_DEBUG_PRINT == 1
	    ylib::log->info("OnBody "+SACONNID,"http_agent ");
	    #endif
        http_agent_extra* extra = GET_EXTRA;
		IHttpServer* server = (IHttpServer*)extra->server->hpserver();
		
		
		// Chunk 数据
		if(extra->recv.transfer_encoding_length != -1){
		    extra->recv.data.append((const char*)pData,iLength);
		}else{
            if(extra->cache.is_open() && extra->status_code == 200)
                extra->cache.write((const char*)pData,iLength);
		    server->Send(
                     (CONNID)extra->connid,
                     pData,
                     iLength);
		}
		
        return HPR_OK;
	}
	virtual EnHttpParseResult OnChunkHeader(IHttpAgent* pSender, CONNID dwConnID, int iLength) override
	{
	    #if HTTP_AGENT_DEBUG_PRINT == 1
        ylib::log->info("OnChunkHeader "+to_string(iLength)+SACONNID,"http_agent ");
	    #endif
        http_agent_extra* extra = GET_EXTRA;
        extra->recv.transfer_encoding_length += iLength;
		return HPR_OK;
	}
	virtual EnHttpParseResult OnChunkComplete(IHttpAgent* pSender, CONNID dwConnID) override
	{
	    #if HTTP_AGENT_DEBUG_PRINT == 1
	    ylib::log->info("OnChunkComplete:"+SACONNID,"http_agent ");
	    #endif
		return HPR_OK;
	}
	virtual EnHttpParseResult OnMessageComplete(IHttpAgent* pSender, CONNID dwConnID) override
	{



	    http_agent_extra* extra = GET_EXTRA;

		#if HTTP_AGENT_DEBUG_PRINT == 1
	    ylib::log->info("OnMessageComplete:"+SACONNID,"http_agent ");
	    #endif



		if(extra->recv.transfer_encoding_length >= 0){
		    IHttpServer* server = (IHttpServer*)extra->server->hpserver();
		    
		    
		    std::string length = dec2hex(extra->recv.transfer_encoding_length+1)+"\r\n";   
		    // 分块长度
		    server->Send(
				(CONNID)extra->connid,
				(const BYTE*)length.c_str(),
				(int)length.length());
			// 数据包
		    server->Send(
				(CONNID)extra->connid,
                     (const BYTE*)extra->recv.data.data(),
				(int)extra->recv.data.length()
                     );
            // 结尾
		    server->Send(
				(CONNID)extra->connid,
                     (const BYTE*)"\r\n0\r\n\r\n",
                     7);
            if(extra->cache.is_open() && extra->status_code == 200)
            {
                extra->cache.write(length.c_str(),length.length());
                extra->cache.write((const char*)extra->recv.data.data(),extra->recv.data.length());
                extra->cache.write("\r\n0\r\n\r\n",7);
            }

		}

        if(extra->cache.is_open())
        {
            if(extra->status_code == 200){
                ylib::file::write(extra->cache.filepath()+".conf",std::to_string(time::now_sec())+"\r\nNone");
            }
            extra->cache.close();
        }
		return HPR_OK;
	}
	virtual EnHttpParseResult OnUpgrade(IHttpAgent* pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType) override
	{
		#if HTTP_AGENT_DEBUG_PRINT == 1
	    ylib::log->info("OnUpgrade:"+SACONNID,"http_agent ");
	    #endif

		return HPR_OK;
	}
	virtual EnHttpParseResult OnParseError(IHttpAgent* pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc) override
	{
#if HTTP_AGENT_DEBUG_PRINT == 1
	    ylib::log->info("OnParsrError:"+SACONNID,"http_agent ");
#endif

		return HPR_OK;
	}
	virtual EnHandleResult OnWSMessageHeader(IHttpAgent* pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen) override
	{
		return HR_OK;
	}
	virtual EnHandleResult OnWSMessageBody(IHttpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
	{
		return HR_OK;
	}
	virtual EnHandleResult OnWSMessageComplete(IHttpAgent* pSender, CONNID dwConnID) override
	{
		return HR_OK;
	}
	virtual EnHandleResult OnHandShake(ITcpAgent* pSender, CONNID dwConnID) override
	{
#if HTTP_AGENT_DEBUG_PRINT == 1
	    ylib::log->info("OnHandShake:"+SACONNID,"http_agent ");
#endif

		http_agent_extra* extra = GET_EXTRA;
		// 发送请求
        THeader* local_header = new THeader[extra->req.request_headers.size()];
        size_t i= 0;
        for_iter(iter,extra->req.request_headers){
            local_header[i].name = iter->name.c_str();
            local_header[i].value = iter->value.c_str();
            i++;
        }
		bool result = extra->agent->SendRequest(
			dwConnID,
			extra->req.method.c_str(),
			extra->req.path.c_str(),
			local_header,
			(int)extra->req.request_headers.size(),
			(const BYTE*)extra->req.data.data(),
			(int)extra->req.data.length());
		delete[] local_header;
		return HR_OK;
	}
	virtual EnHandleResult OnSend(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
	{
#if HTTP_AGENT_DEBUG_PRINT == 1
	    ylib::log->info("OnSend:"+SACONNID,"http_agent ");
#endif

		return HR_OK;
	}
	virtual EnHandleResult OnReceive(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
	{
#if HTTP_AGENT_DEBUG_PRINT == 1
	    ylib::log->info("OnReceive:"+SACONNID,"http_agent ");
#endif

		//ylib::log->info("OnReceive", "agent");
		return HR_OK;
	}
	virtual EnHandleResult OnReceive(ITcpAgent* pSender, CONNID dwConnID, int iLength) override
	{
		return HR_OK;
	}
	virtual EnHandleResult OnClose(ITcpAgent* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode) override
	{
	    #if HTTP_AGENT_DEBUG_PRINT == 1
	    ylib::log->info("OnClose "+SACONNID,"http_agent ");
	    #endif
	    
        http_agent_extra* extra = GET_EXTRA;
        //IHttpServer* server = (IHttpServer*)extra->server->hpserver();
        //server->Disconnect(extra->connid);
		delete extra;
		return HR_OK;
	}
	virtual EnHandleResult OnShutdown(ITcpAgent* pSender) override
	{
		return HR_OK;
	}
	virtual EnHandleResult OnPrepareConnect(ITcpAgent* pSender, CONNID dwConnID, SOCKET socket) override
	{
		return HR_OK;
	}
	virtual EnHandleResult OnConnect(ITcpAgent* pSender, CONNID dwConnID) override
	{
#if HTTP_AGENT_DEBUG_PRINT == 1
	    ylib::log->info("OnConnect:"+SACONNID,"http_agent ");
#endif

		return HR_OK;
	}
public:
	network::http::agent* m_agent;

};
bool ylib::network::http::agent::request(int32 wait_msec,reqpack* rp, network::http::proxy* proxy)
{
	
	http_agent_extra* extra = new http_agent_extra;

    extra->connid = rp->connid();
	extra->server = rp->server();
	 
    IHttpAgent* agent = nullptr;
    if(proxy->ssl)
        agent = (IHttpAgent*)m_agent_ssl;
    else
        agent = (IHttpAgent*)m_agent;

    extra->agent = agent;
    // 拼接地址
    {
		try
		{

            size_t right_length = rp->url().length() - ylib::file::parent_dir(proxy->src_str).length();
			std::string pjp;
			pjp.append(rp->url().c_str() + rp->url().length() - right_length, right_length);
			extra->req.path.append(proxy->dst);

			if (
				(proxy->dst.empty() || proxy->dst[proxy->dst.length()-1] != '/') 
				&& pjp[0] != '/') {
                extra->req.path +='/';
			}
			extra->req.path.append(pjp);
		}
		catch (const std::exception& e)
		{
            ylib::log->fatal(e.what());
		}
        
    }
	extra->req.data = rp->data();
	{
		extra->req.url_host.append(proxy->remote_ipaddress);
        extra->req.url_host.append(":");
        extra->req.url_host.append(std::to_string(proxy->remote_port));
		extra->req.url_host.append(extra->req.path);
	}
	// 构造请求内容
	{
        THeader* header = nullptr;
        DWORD header_size = 0;
        ((IHttpServer*)rp->server()->hpserver())->GetAllHeaders((CONNID)rp->connid(), header,header_size);
        if (header_size == 0){
#if HTTP_AGENT_PRINT == 1
            ylib::log->error("not found header,"+extra->req.path,"http_agent ");
#endif
			delete extra;
            return false;
        }
        header = new THeader[header_size];
        ((IHttpServer*)rp->server()->hpserver())->GetAllHeaders((CONNID)rp->connid(), header,header_size);
        // 浏览器真实IP
        std::string ipaddress;
        ushort port = 0;
        rp->server()->remote(rp->connid(),ipaddress,port);
		// 请求协议头
		{
            for (size_t i = 0; i < header_size; i++) {
                ylib::KeyValue<std::string,std::string> kv;
				kv.name = header[i].name;
				kv.value = header[i].value;
				// 要求协议头
				for_iter(iter, proxy->request_headers) {
					if (kv.name == iter->first) {
						kv.value = iter->second;
						break;
					}
				}
				extra->req.request_headers.push_back(kv);
			}
			for_iter(iter, extra->req.request_headers) {
				if (iter->name == "X-Real-IP") {
					if (iter->value == "{client_ipaddress}") {
						iter->value = ipaddress;
					}
				}
				else if (iter->name == "Host") {
					if (iter->value == "{host}") {
						iter->value = proxy->remote_ipaddress;
					}
				}
			}
		}
		// 回复协议头
		{
            for_iter(iter, proxy->response_headers) {
                ylib::KeyValue<std::string,std::string> kv;
				kv.name = iter->first;
				kv.value = iter->second;
				extra->req.response_headers.push_back(kv);
			}
		}

        delete[] header;
        // 取请求方式
		extra->req.method = ((IHttpServer*)rp->server()->hpserver())->GetMethod((CONNID)rp->connid());
	}
    // 符合缓存
    {
        auto cache = rp->website()->cache();
        if(cache->enable())
        {
            if(cache->find_ext(rp->filepath())){
                std::string cache_filepath = cache->make_cache_filepath(rp->filepath());
                extra->cache.close();
                if(extra->cache.open(cache_filepath) == false){
                    ylib::log->error("create open cache file failed! path:"+cache_filepath,"http_agent ");
                }else{
                    extra->cache.clear();
                    ylib::file::remove(cache_filepath+".header");
                    ylib::file::remove(cache_filepath+".conf");
                }
            }
        }

    }

#if 0
    std::cout<<"======================HEADER=========================="<<std::endl;
    for(size_t i=0;i<extra->req.headers.size();i++){
        std::cout<<extra->req.headers[i].name.c_str()<<":\t"<<extra->req.headers[i].value.c_str()<<std::endl;
    }
#endif
    CONNID hpcid = 0;
	if (extra->agent->Connect(proxy->remote_ipaddress.c_str(), proxy->remote_port, &hpcid, (PVOID)extra) == false)
	{
#if HTTP_AGENT_PRINT == 1
        ylib::log->error("connect failed.\t"+extra->req.path,"http_agent ");
#endif
		delete extra;
		return false;
	}
    // 临时附加数据
    {
        PVOID exts = 0;
        if (((IHttpServer*)rp->server()->hpserver())->GetConnectionExtra((CONNID)rp->connid(), &exts) == false){
            agent->Disconnect((CONNID)hpcid);
#if HTTP_AGENT_PRINT == 1
            ylib::log->error("get server extra failed,"+extra->req.path,"http_agent ");
#endif
            return false;
        }
        ((temp_recv*)exts)->agent_connid = hpcid;
        ((temp_recv*)exts)->agent_ssl = proxy->ssl;
    }
	return true;
}
void* ylib::network::http::agent::get()
{
    return m_agent;
}
ylib::network::http::agent::agent()
{
	m_agent = nullptr;
	m_agent_ssl = nullptr;
	m_listener = new http_agent_listener;
	m_listener->m_agent = this;
	m_agent_ssl = HP_Create_HttpsAgent(m_listener);
	m_agent = HP_Create_HttpAgent(m_listener);
} 
ylib::network::http::agent::~agent()
{
    stop();
    HP_Destroy_HttpAgent((IHttpAgent*)m_agent);
    HP_Destroy_HttpsAgent((IHttpAgent*)m_agent_ssl);
}
void ylib::network::http::agent::stop()
{
 	((IHttpAgent*)m_agent)->Stop();
	((IHttpAgent*)m_agent_ssl)->Stop();
	((IHttpAgent*)m_agent_ssl)->Wait();
	((IHttpAgent*)m_agent_ssl)->Wait();
}
bool ylib::network::http::agent::start()
{
	//return true;
	((IHttpAgent*)m_agent_ssl)->CleanupSSLContext();
	if (((IHttpAgent*)m_agent_ssl)->SetupSSLContext() == false)
    {
        m_lastErrorDesc =  "SetupSSLContextFailed, " + std::to_string((uint32)SYS_GetLastError());
		return false;
	}
    if(((IHttpAgent*)m_agent_ssl)->Start() == false){
        m_lastErrorDesc = "SSLHttpAgent start failed.";
        return false;
    }
    if(((IHttpAgent*)m_agent)->Start() == false){
        m_lastErrorDesc = "HttpAgent start failed.";
        ((IHttpAgent*)m_agent_ssl)->Stop();
        ((IHttpAgent*)m_agent_ssl)->Wait();
        return false;

    }
    return true;

}
void ylib::network::http::agent::disconnect(bool ssl,uint64 connid)
{
	if(ssl)
        ((IHttpAgent*)m_agent_ssl)->Disconnect((CONNID)connid,TRUE);
    else
        ((IHttpAgent*)m_agent)->Disconnect((CONNID)connid,TRUE);
}
#endif

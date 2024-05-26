#include "net/http_server_lst.h"
#if USE_NET_HTTP_WEBSITE
#include <algorithm>
#include <cstring>
#include "HPSocket/HPSocket.h"

#include "net/http_server.h"
#include "net/http_define.h"
#include "net/http_reqpack.h"
#include "net/http_router.h"
#include "net/http_center.h"
#include "net/http_website.h"
#include "net/http_agent.h"
#include "net/util.h"
#include "net/http_cache.h"
#include "util/codec.h"

#define BARE_HP 0
ylib::network::http::http_server_lst::http_server_lst(server* server)
{
	
}

ylib::network::http::http_server_lst::~http_server_lst()
{
	
}

EnHandleResult ylib::network::http::http_server_lst::OnPrepareListen(ITcpServer* pSender, SOCKET soListen)
{
	return HR_OK;
}

EnHandleResult ylib::network::http::http_server_lst::OnAccept(ITcpServer* pSender, CONNID dwConnID, UINT_PTR soClient)
{
	//std::cout << "[OnAccept] HttpServerConnect:" << pSender->GetConnectionCount() << std::endl;
#if BARE_HP == 0
	pSender->SetConnectionExtra(dwConnID, (PVOID)new temp_recv);
#endif
#if HTTP_SERVER_DEBUG_PRINT == 1
    ylib::log->info("OnAccept ("+std::to_string((uint64)dwConnID)+")","http_server");
#endif

	return HR_OK;
}

EnHandleResult ylib::network::http::http_server_lst::OnHandShake(ITcpServer* pSender, CONNID dwConnID)
{
#if HTTP_SERVER_DEBUG_PRINT == 1
     ylib::log->info("OnHandShake ("+std::to_string((uint64)dwConnID)+")","http_server");
#endif

	return HR_OK;
}

EnHandleResult ylib::network::http::http_server_lst::OnReceive(ITcpServer* pSender, CONNID dwConnID, int iLength)
{
	return HR_OK;
}

EnHandleResult ylib::network::http::http_server_lst::OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
#if HTTP_SERVER_DEBUG_PRINT == 1
     ylib::log->info("OnReceive ("+std::to_string((uint64)dwConnID)+")","http_server");
#endif

	return HR_OK;
}

EnHandleResult ylib::network::http::http_server_lst::OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
#if HTTP_SERVER_DEBUG_PRINT == 1
     ylib::log->info("OnSend ("+std::to_string((uint64)dwConnID)+")","http_server");
#endif

	m_server->qps()->response(iLength,true);
	return HR_OK;
}

EnHandleResult ylib::network::http::http_server_lst::OnShutdown(ITcpServer* pSender)
{
	return HR_OK;
}

EnHandleResult ylib::network::http::http_server_lst::OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
#if HTTP_SERVER_DEBUG_PRINT == 1
     ylib::log->info("OnClose ("+std::to_string((uint64)dwConnID)+")","http_server");
#endif
#if BARE_HP == 0
	PVOID extra = 0;
	if (pSender->GetConnectionExtra(dwConnID, &extra))
	{
		if (extra != 0)
		{
			temp_recv* tr = (temp_recv*)extra;
#if USE_NET_HTTP_AGENT == 1
			if (tr->agent_connid != 0)
			{
                m_server->agent()->disconnect(tr->agent_ssl,tr->agent_connid);
			}
#endif
			delete tr;
		}
			
	}
#endif
	return HR_OK;
}

EnHttpParseResult ylib::network::http::http_server_lst::OnMessageBegin(IHttpServer* pSender, CONNID dwConnID)
{
#if HTTP_SERVER_DEBUG_PRINT == 1
     ylib::log->info("OnMessageBegin ("+std::to_string((uint64)dwConnID)+")","http_server");
#endif
#if BARE_HP == 0
	PVOID extra = 0;
	if (pSender->GetConnectionExtra(dwConnID, &extra))
	{
		if (extra != 0)
        {
			((temp_recv*)extra)->data.clear();
		}
	}
#endif
	return HPR_OK;
}

EnHttpParseResult ylib::network::http::http_server_lst::OnRequestLine(IHttpServer* pSender, CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszUrl)
{
//#if HTTP_SERVER_DEBUG_PRINT == 1
//     ylib::log->info("OnRequestLine ("+nstring::from((uint64)dwConnID)+")","http_server");
//#endif

#if BARE_HP == 0
	PVOID extra = 0;
	if (pSender->GetConnectionExtra(dwConnID, &extra))
	{
		if (extra != 0)
		{
			((temp_recv*)extra)->url = lpszUrl;
		}
	}
#endif
	return HPR_OK;
}

EnHttpParseResult ylib::network::http::http_server_lst::OnStatusLine(IHttpServer* pSender, CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc)
{
//#if HTTP_SERVER_DEBUG_PRINT == 1
//     ylib::log->info("OnStatusLine ("+nstring::from((uint64)dwConnID)+")","http_server");
//#endif

	return HPR_OK;
}

EnHttpParseResult ylib::network::http::http_server_lst::OnHeader(IHttpServer* pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue)
{
#if HTTP_SERVER_DEBUG_PRINT == 1
     ylib::log->info("OnHeader ("+std::to_string((uint64)dwConnID)+")"+lpszName+":"+lpszValue,"http_server");
#endif
#if BARE_HP == 0
    if(strcmp(lpszName,"Host") == 0){
        PVOID extra = 0;
        if (pSender->GetConnectionExtra(dwConnID, &extra))
        {
            if (extra != 0)
            {
                ((temp_recv*)extra)->host = lpszValue;
            }
        }
   
    }
#endif
	return HPR_OK;
}

EnHttpParseResult ylib::network::http::http_server_lst::OnHeadersComplete(IHttpServer* pSender, CONNID dwConnID)
{
	return HPR_OK;
}

EnHttpParseResult ylib::network::http::http_server_lst::OnChunkHeader(IHttpServer* pSender, CONNID dwConnID, int iLength)
{
#if HTTP_SERVER_DEBUG_PRINT == 1
     ylib::log->info("OnChuckHeader ("+std::to_string((uint64)dwConnID)+")","http_server");
#endif

	return HPR_OK;
}
EnHttpParseResult ylib::network::http::http_server_lst::OnChunkComplete(IHttpServer* pSender, CONNID dwConnID)
{
#if HTTP_SERVER_DEBUG_PRINT == 1
     ylib::log->info("OnChunkComplete ("+std::to_string((uint64)dwConnID)+")","http_server");
#endif

	return HPR_OK;
}

EnHttpParseResult ylib::network::http::http_server_lst::OnUpgrade(IHttpServer* pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType)
{
#if HTTP_SERVER_DEBUG_PRINT == 1
     ylib::log->info("OnUpgrade ("+std::to_string((uint64)dwConnID)+")","http_server");
#endif

	return HPR_OK;
}

EnHttpParseResult ylib::network::http::http_server_lst::OnBody(IHttpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
#if HTTP_SERVER_DEBUG_PRINT == 1
     ylib::log->info("OnBody ("+std::to_string((uint64)dwConnID)+")","http_server");
#endif

#if BARE_HP == 0
	PVOID extra = 0;
	if (pSender->GetConnectionExtra(dwConnID, &extra))
	{
		if (extra != 0)
            ((temp_recv*)extra)->data.append((char*)pData, iLength);
	}
#endif
	return HPR_OK;
}

EnHttpParseResult ylib::network::http::http_server_lst::OnMessageComplete(IHttpServer* pSender, CONNID dwConnID)
{
#if HTTP_SERVER_DEBUG_PRINT == 1
     ylib::log->info("OnMessageComplete ("+std::to_string((uint64)dwConnID)+")","http_server");
#endif
#if BARE_HP == 1
	pSender->SendResponse(dwConnID, 200, "OK", nullptr, 0, (const BYTE*)"ONLY_HP_OK", 3);
	return HPR_OK;
#endif
    reqpack* rp = nullptr;
    {
		temp_recv* tr = nullptr;
        PVOID extra = 0;
		if (pSender->GetConnectionExtra((CONNID)dwConnID, &extra))
		{
			if (extra != 0)
				tr = (temp_recv*)extra;
			else
				return HPR_OK;
		}
		else
			return HPR_OK;
		
		m_server->qps()->request(tr->data.length()); 

		rp = new reqpack;
        rp->init(tr->url,tr->host,tr->data,(uint64)dwConnID,m_server);
    }
	//ylib::log->info("OnRecv:"+codec::to_gbk(rp->data()->to_string()));
    std::string size_name;
    {
        double size_ld = 0;
        if(!rp->data().empty()){
            size_ld = (double)rp->data().length();
        }
        size_name = ylib::network::size_name(size_ld,2);
        /*if(nstring(pSender->GetMethod(dwConnID)) == "GET" && rp->data()->length() != 0){
            ylib::log->warn("GET have body:"+rp->data()->to_string(),"http_server");
        }*/
    }
#if HTTP_SERVER_PRINT == 1
	// LOG
	std::string logstr;
	{
		logstr.append("[recv   ]\t");
		logstr.append(rp->remote());
        logstr.append("\t(");
        logstr.append(std::to_string(rp->connid()));
		logstr.append(")\t\t");
        logstr.append(pSender->GetMethod(dwConnID));
		logstr.append("\t");
		logstr.append(size_name);
		logstr.append("\t");
		logstr.append(rp->host());
		logstr.append(rp->url());
	}
#endif
	auto website = m_server->center()->website(rp->host());
	if (website == nullptr)
	{
	    pSender->SendResponse(dwConnID, 404, "Not Found", nullptr, 0, (const BYTE*)"No such site",12);
#if HTTP_SERVER_PRINT == 1
        logstr.append(" not website("+rp->host()+"):");
        ylib::log->error(logstr,"http_server");
#endif
		delete rp;
		return HPR_OK;
	}
#if HTTP_SERVER_PRINT == 1
    ylib::log->info(logstr,"http_server");
#endif
	rp->website(website);
#if 0
	rp->request();
	rp->response();
	reqpack::destory(rp);
	pSender->SendResponse(dwConnID, 200, "OK", nullptr, 0, (const BYTE*)"V24", 3);
	return HPR_OK;

#endif
	auto recvd_callback = website->router()->m_callback_recved;
	ylib::buffer end_data;
	if (recvd_callback != nullptr)
	{
		recvd_callback(rp->data(), &end_data);
		if (end_data.length() != 0)
			rp->data() = end_data;
	}
        


    website->router()->push(rp);
#if 0
    std::string filepath = pSender->GetUrlField((CONNID)dwConnID, EnHttpUrlField::HUF_PATH);

    filepath = "/home/nianhua/project/mysql_demo/www"+filepath;
    pSender->SendLocalFile(dwConnID,filepath.c_str(),HSC_OK);
#endif

	
	return HPR_OK;

}

EnHttpParseResult ylib::network::http::http_server_lst::OnParseError(IHttpServer* pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc)
{
#if HTTP_SERVER_DEBUG_PRINT == 1
     ylib::log->info("OnParseError ("+std::to_string((uint64)dwConnID)+")","http_server");
#endif

	PVOID extra = 0;
	if (pSender->GetConnectionExtra(dwConnID, &extra))
	{
		if (extra != 0)
		{
			((temp_recv*)extra)->data.clear();
		}
	}
	pSender->SetConnectionExtra(dwConnID,0);
	return HPR_OK;
}

EnHandleResult ylib::network::http::http_server_lst::OnWSMessageHeader(IHttpServer* pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)
{
	return HR_OK;
}

EnHandleResult ylib::network::http::http_server_lst::OnWSMessageBody(IHttpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	return HR_OK;
}

EnHandleResult ylib::network::http::http_server_lst::OnWSMessageComplete(IHttpServer* pSender, CONNID dwConnID)
{
	return HR_OK;
}
#endif

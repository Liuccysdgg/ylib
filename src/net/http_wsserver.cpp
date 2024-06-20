#include "net/http_wsserver.h"
#include "HPSocket/HPSocket-SSL.h"
#include "util/codec.h"

class wsserver_lst : public IHttpServerListener
{
public:

public:
    wsserver_lst(network::http::wsserver* server) :m_server(server) {}
    ~wsserver_lst() {}
private:
    EnHandleResult OnPrepareListen(ITcpServer* pSender, SOCKET soListen)
    {
        return HR_OK;
    }
    EnHandleResult OnAccept(ITcpServer* pSender, CONNID dwConnID, UINT_PTR soClient)
    {
        pSender->SetConnectionExtra(dwConnID, new ylib::buffer());
        return HR_OK;
    }
    EnHandleResult OnHandShake(ITcpServer* pSender, CONNID dwConnID)
    {
        if (m_server->m_callback_accept != nullptr)
        {
            if (m_server->m_callback_accept(dwConnID) == false)
                return HR_ERROR;
        }
        return HR_OK;
    }
    EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, int iLength)
    {
        return HR_OK;
    }
    EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
    {
        return HR_OK;
    }
    EnHandleResult OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
    {
        return HR_OK;
    }
    EnHandleResult OnShutdown(ITcpServer* pSender)
    {
        return HR_OK;
    }
    EnHandleResult OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
    {
        PVOID extra = nullptr;
        if (pSender->GetConnectionExtra(dwConnID, &extra))
        {
            delete static_cast<ylib::buffer*>(extra);
        }
        if (m_server->m_callback_close != nullptr)
        {
            m_server->m_callback_close(dwConnID);
        }
        return HR_OK;
    }

    EnHttpParseResult OnMessageBegin(IHttpServer* pSender, CONNID dwConnID)
    {
        return HPR_OK;
    }
    EnHttpParseResult OnRequestLine(IHttpServer* pSender, CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszUrl)
    {
        return HPR_OK;
    }
    EnHttpParseResult OnStatusLine(IHttpServer* pSender, CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc)
    {
        return HPR_OK;
    }
    EnHttpParseResult OnHeader(IHttpServer* pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue)
    {
        return HPR_OK;
    }
    EnHttpParseResult OnHeadersComplete(IHttpServer* pSender, CONNID dwConnID)
    {
        return HPR_OK;
    }
    EnHttpParseResult OnChunkHeader(IHttpServer* pSender, CONNID dwConnID, int iLength)
    {
        return HPR_OK;
    }
    EnHttpParseResult OnChunkComplete(IHttpServer* pSender, CONNID dwConnID)
    {
        return HPR_OK;
    }
    EnHttpParseResult OnUpgrade(IHttpServer* pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType)
    {
        if (enUpgradeType == HUT_WEB_SOCKET)
        {
            LPCSTR lpszValue = nullptr;
            if (pSender->GetHeader(dwConnID,"Sec-WebSocket-Key", &lpszValue) == false)
                return HPR_ERROR;
            std::string value = std::string(lpszValue)+ "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

            std::string accept_value = codec::base64::en(codec::sha1(value));


            THeader headers[3];
            headers[0].name = "Connection";
            headers[0].value = "Upgrade";
            headers[1].name = "Upgrade";
            headers[1].value = "WebSocket";
            headers[2].name = "Sec-WebSocket-Accept";
            headers[2].value = accept_value.c_str();
            pSender->SendResponse(dwConnID, 101, "Switching Protocols", headers,3);
            return HPR_OK;
        }
        else
            return HPR_ERROR;
    }
    EnHttpParseResult OnBody(IHttpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
    {
        return HPR_OK;
    }
    EnHttpParseResult OnMessageComplete(IHttpServer* pSender, CONNID dwConnID)
    {
        return HPR_OK;
    }
    EnHttpParseResult OnParseError(IHttpServer* pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc)
    {
        return HPR_OK;
    }

    EnHandleResult OnWSMessageHeader(IHttpServer* pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)
    {
        PVOID extra = nullptr;
        if (pSender->GetConnectionExtra(dwConnID, &extra))
        {
            static_cast<ylib::buffer*>(extra)->clear();
        }


        return HR_OK;
    }
    EnHandleResult OnWSMessageBody(IHttpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
    {

        PVOID extra = nullptr;
        if (pSender->GetConnectionExtra(dwConnID, &extra))
        {
            static_cast<ylib::buffer*>(extra)->append((const char*)pData, iLength);
        }


        return HR_OK;
    }
    EnHandleResult OnWSMessageComplete(IHttpServer* pSender, CONNID dwConnID)
    {
        PVOID extra = nullptr;
        ylib::buffer* data = nullptr;
        pSender->GetConnectionExtra(dwConnID, &extra);
        data = static_cast<ylib::buffer*>(extra);

        if (m_server->m_callback_recv != nullptr)
        {
            if (m_server->m_callback_recv(dwConnID, (const char*)data->data(), data->length()) == false)
                return HR_ERROR;
        }
        return HR_OK;
    }
private:
    // HPSERVER 指针
    network::http::wsserver* m_server = nullptr;
};
#define HPSERVER static_cast<::IHttpServer*>(m_server)
ylib::network::http::wsserver::wsserver()
{
}

ylib::network::http::wsserver::~wsserver()
{
    stop();
}

bool ylib::network::http::wsserver::start(ushort port, bool ssl, const std::string& ssl_key, const std::string& ssl_pem)
{
    stop();
    m_listener = new wsserver_lst(this);
    m_ssl = ssl;
    m_port = port;
    m_ssl_key = ssl_key;
    m_ssl_pem = ssl_pem;
    if (ssl)
    {
        m_server = HP_Create_HttpsServer(m_listener);
    }
    else
    {
        m_server = HP_Create_HttpServer(m_listener);
    }
    if (HPSERVER->Start("0.0.0.0",m_port) == false)
    {
#ifndef _WIN32
        this->m_lastErrorDesc = "start failed, error code:" + std::string(SYS_GetLastErrorStr());
#else
        this->m_lastErrorDesc = "start failed, error code:" + std::to_string((uint32)SYS_GetLastError());
#endif
        stop();
        return false;
    }
    return true;
}

void ylib::network::http::wsserver::stop()
{
    if (m_server == nullptr)
        return;
    //关闭HP HTTPSERVER
    if (HPSERVER->GetState() != SS_STOPPED)
    {
        if (HPSERVER->Stop())
            HPSERVER->Wait(-1);
    }
    //销毁释放
    if (m_ssl)
        HP_Destroy_HttpsServer(HPSERVER);
    else
        HP_Destroy_HttpServer(HPSERVER);
    if(m_listener != nullptr)
        delete m_listener;
    m_server = nullptr;
    m_listener = nullptr;
    m_callback_accept = nullptr;
    m_callback_close = nullptr;
    m_callback_recv = nullptr;
}

bool ylib::network::http::wsserver::send(int64 conn, std::string_view value)
{
    
    BOOL bFinal;
    BYTE iReserved;
    BYTE iOperationCode;
    if (HPSERVER->GetWSMessageState(conn,&bFinal,&iReserved,&iOperationCode,nullptr,nullptr,nullptr) == false)
        return false;
    return HPSERVER->SendWSMessage(conn,bFinal,iReserved,iOperationCode,(const BYTE*)value.data(),value.length());
}

void ylib::network::http::wsserver::disconn(int64 conn)
{
    HPSERVER->Disconnect(conn);
}

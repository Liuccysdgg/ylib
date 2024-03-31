#include "ynet/http_ssl.h"
#if USE_NET_HTTP_WEBSITE
#include "HPSocket/HPSocket.h"

#include "ynet/http_server.h"
#include "yutil/file.h"

#define HPSERVER ((IHttpServer*)m_server->hpserver())
ylib::network::http::ssl::ssl(network::http::server* server,ssl_verify_type verify_type, const std::string& pem_cert, const std::string& pem_key, const std::string& pem_ca, const std::string& password)
{
	m_verify_type = verify_type;
	m_pem_cert =  ylib::file::read(pem_cert).to_string();
	m_pem_key = ylib::file::read(pem_key).to_string();
	m_pem_ca = ylib::file::read(pem_ca).to_string();
	if (m_pem_cert.empty())
		ylib::log->warn("ssl cert is empty,filepath:"+pem_cert);
	if (m_pem_key.empty())
		ylib::log->warn("ssl key is empty,filepath:" + pem_key);

	m_pem_password = password;
	m_server = server;
	m_index = -1;
}          

ylib::network::http::ssl::~ssl()
{
}
bool ylib::network::http::ssl::bind(const std::string& host)
{
	return HPSERVER->BindSSLServerName(host.c_str(),m_index);
}
bool ylib::network::http::ssl::regist()
{
    if(m_server->m_init_ssl == false){
        m_server->m_init_ssl = true;
        if (HPSERVER->SetupSSLContextByMemory(
                                              (EnSSLVerifyMode)m_verify_type,
                                              m_pem_cert.c_str(),
                                              m_pem_key.c_str(),
                                              m_pem_password.c_str(),
											  m_pem_ca.c_str()) == false)
        {
            m_lastErrorDesc = "SetupSSLContextByMemory Failed, code:"+std::to_string((uint64)SYS_GetLastError());
            return false;
        }

    }
	m_index = HPSERVER->AddSSLContextByMemory((EnSSLVerifyMode)m_verify_type,
		m_pem_cert.c_str(),
		m_pem_key.c_str(),
		m_pem_password.c_str());
	if(m_index < 0)
	{
        m_lastErrorDesc = "AddSSLContextByMemory Failed, code:" + std::to_string((uint64)SYS_GetLastError());
		return false;
	}
	return true;
}
#endif

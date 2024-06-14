/*Software License

Copyright(C) 2024[liuyingjie]
License Terms
Usage Rights

Any individual or entity is free to use, copy, and distribute the binary form of this software without modification to the source code, without the need to disclose the source code.
If the source code is modified, the modifications must be open - sourced under the same license.This means that the modifications must be disclosed and accompanied by a copy of this license.
Future Versions Updates
From this version onwards, all future releases will be governed by the terms of the latest version of the license.This license will automatically be nullified and replaced by the new version.
Users must comply with the terms of the new license issued in future releases.
Liability and Disclaimer
This software is provided “as is”, without any express or implied warranties, including but not limited to the warranties of merchantability, fitness for a particular purpose, and non - infringement.In no event shall the author or copyright holder be liable for any claims, damages, or other liabilities, whether in an action of contract, tort, or otherwise, arising from, out of, or in connection with the software or the use or other dealings in the software.
Contact Information
If you have any questions, please contact us: 1585346868@qq.com Or visit our website fwlua.com.
*/

#include "net/http_ssl.h"
#if USE_NET_HTTP_WEBSITE
#include "HPSocket/HPSocket.h"

#include "net/http_server.h"
#include "util/file.h"

#define HPSERVER ((IHttpServer*)m_server->hpserver())
ylib::network::http::ssl::ssl(network::http::server* server, ssl_config config):m_config(config)
{

	m_pem_cert_data =  m_config.pem_cert;
	m_pem_key_data = m_config.pem_key;
	m_pem_ca_data = m_config.pem_ca;
	if (m_pem_cert_data.empty())
		ylib::log->warn("ssl cert is empty");
	if (m_pem_key_data.empty())
		ylib::log->warn("ssl key is empty");
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
                                              (EnSSLVerifyMode)m_config.type,
                                              m_pem_cert_data.c_str(),
                                              m_pem_key_data.c_str(),
                                              m_config.pem_password.c_str(),
											  m_config.pem_ca.c_str()) == false)
        {
            m_lastErrorDesc = "SetupSSLContextByMemory Failed, code:"+std::to_string((uint64)SYS_GetLastError());
            return false;
        }

    }
	m_index = HPSERVER->AddSSLContextByMemory((EnSSLVerifyMode)m_config.type,
		m_pem_cert_data.c_str(),
		m_pem_key_data.c_str(),
		m_config.pem_password.c_str());
	if(m_index < 0)
	{
        m_lastErrorDesc = "AddSSLContextByMemory Failed, code:" + std::to_string((uint64)SYS_GetLastError());
		return false;
	}
	return true;
}
#endif

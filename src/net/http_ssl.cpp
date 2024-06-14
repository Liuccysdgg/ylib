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

// QQ：1585346868

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

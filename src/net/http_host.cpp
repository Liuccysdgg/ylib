
#include "net/http_host.h"
#if USE_NET_HTTP_WEBSITE
ylib::network::http::host::host()
{
	m_port = 0;

}

ylib::network::http::host::~host()
{
}

void ylib::network::http::host::init(const std::string& host,ushort port, network::http::ssl* ssl)
{
	m_host = host;
	m_port = port;
	m_ssl = ssl;
	if (m_port == 443 || m_port == 80)
		m_return_host = host;
    else
        m_return_host = host + ":" + std::to_string(m_port);
}
#endif

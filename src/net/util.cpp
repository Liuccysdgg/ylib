#include "net/util.h"
#if USE_NET_HTTP_UTIL
#include "HPSocket/HPSocket.h"
#include <regex>
#ifdef _WIN32
#include <sys/stat.h>
#include <sys/timeb.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <tlhelp32.h>
#include <locale>
#include "psapi.h"
#include <tcpmib.h>
#include <IPHlpApi.h>
#include <WinInet.h>
#else
#include <arpa/inet.h> 
#include <unistd.h>
#include <netdb.h>
#endif
#include <cmath>
#include <string.h>

#include "util/strutils.h"
#ifdef _WIN32
#pragma warning(disable: 4996)
#endif
void ylib::network::content_type(const std::string& extName, std::string& type)
{
    if (extName == "html")
    {
        type = "text/html";
    }
    else if (extName == "htm")
    {
        type = "text/html";
    }
    else if (extName == "css")
    {
        type = "text/css";
    }
    else if (extName == "text")
    {
        type = "text/plain";
    }
    else if (extName == "icon")
    {
        type = "image/x-icon";
    }
    else if (extName == "jpeg")
    {
        type = "image/jpeg";
    }
    else if (extName == "mp3")
    {
        type = "audio/mpeg";
    }
    else if (extName == "pdf")
    {
        type = "application/pdf";
    }
    else if (extName == "swf")
    {
        type = "application/x-shockwave-flash";
    }
    else if (extName == "wav")
    {
        type = "audio/x-wav";
    }
    else if (extName == "zip")
    {
        type = "application/zip";
    }
    else if (extName == "mp4")
    {
        type = "video/mpeg4";
    }
    else if (extName == "png")
    {
        type = "image/png";
    }
    else if (extName == "rmvb")
    {
        type = "application/vnd.rn-realmedia-vbr";
    }
    else if (extName == "js")
    {
        type = "application/x-javascript";
    }
    else if (extName == "ttf")
    {
        type = "application/octet-stream";
    }
    else if (extName == "woff")
    {
        type = "application/x-font-woff";
    }
    else if (extName == "woff2")
    {
        type = "application/x-font-woff";
    }
    else
    {
        type = extName;
    }

}

bool ylib::network::parse_url(const std::string& url, std::string& httpType,std::string&host, std::string& ipaddress, ushort& port, std::string& urlField)
{
    std::string temp;

    if (url.length() < 9)
        return false;
    if (url.substr(0, 7) == "http://")
    {
        httpType = "http://";
        temp = url.substr(7, url.length() - 7);
    }
    else if (url.substr(0, 8) == "https://")
    {
        httpType = "https://";
        temp = url.substr(8, url.length() - 8);
    }
    else
    {
        temp = url;
        httpType = "http://";
    }

    auto __arr = strutils::split(temp,'/');
    if (__arr.size() == 2)
    {
        temp = __arr[0];
        urlField = __arr[1];
    }
    __arr = strutils::split(temp,':');
    if (__arr.size() == 2)
    {
        host = __arr[0];
        if (is_ipv4(host) == false)
            ipaddress = ylib::network::to_ip(__arr[0]);
        else
            ipaddress = host;
        port = (ushort)ylib::stoi(__arr[1]);
    }
    else
    {
        host = temp;
        ipaddress = temp;
        if(httpType == "https://")
            port = 443;
        else
            port = 80;
    }
    return true;
}
std::string ylib::network::to_ip(const std::string& url)
{
#ifdef WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return "";
    }

    struct addrinfo* addr = NULL;
    result = getaddrinfo("example.com", NULL, NULL, &addr);
    if (result != 0) {
        std::cerr << "getaddrinfo failed: " << result << std::endl;
        WSACleanup();
        return "";
    }
    std::string resultIpaddress;
    for (struct addrinfo* ptr = addr; ptr != NULL; ptr = ptr->ai_next) {
        char ipStr[INET6_ADDRSTRLEN];
        void* addrPtr = (ptr->ai_family == AF_INET)
            ? (void*)&((struct sockaddr_in*)ptr->ai_addr)->sin_addr
            : (void*)&((struct sockaddr_in6*)ptr->ai_addr)->sin6_addr;

        inet_ntop(ptr->ai_family, addrPtr, ipStr, sizeof(ipStr));
        resultIpaddress = ipStr;
    }

    freeaddrinfo(addr);
    WSACleanup();
    return resultIpaddress;
#else
    struct hostent *hptr;
    hptr = gethostbyname(url.c_str());
    if (hptr == nullptr)
        hstrerror(h_errno);
        return "";
    if (hptr->h_addrtype != AF_INET)
        return "";
    char **pptr = hptr->h_addr_list;
    if (*pptr == nullptr)
        return "";
    char str[INET_ADDRSTRLEN+1];
    memset(str,0,INET_ADDRSTRLEN+1);
    inet_ntop(hptr->h_addrtype, hptr->h_addr, str, sizeof(str));
    return str;
#endif	
}


void ylib::network::to_string(uint32 int_ip, std::string& ipaddress)
{
	unsigned char* pch = (unsigned char*)&int_ip;
	char szIPAddr[64] = { 0 };

	size_t index = 0;
	char number[4];
	for (uint32 i = 0; i < 4; i++)
	{
		memset(number, 0, 4);

		sprintf(number, "%d", *(pch + i));
		memcpy(szIPAddr + index, number, strlen(number));
		if (i == 3)
			break;
		index += strlen(number);
		szIPAddr[index] = '.';
		index++;
	}
	ipaddress = szIPAddr;
}

bool ylib::network::is_occupy(uint32 port)
{
#ifdef _WIN32
	WORD wVersionRequested = 0;
	WSADATA wsaData = {};
	int err = 0;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return true;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		WSACleanup();
		return true;
	}

	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(s, (LPSOCKADDR)&addr, sizeof(addr));
	bool result = false;
	if (WSAGetLastError() == WSAEADDRINUSE)
	{
		//端口已被占用
		result = true;
	}
	closesocket(s);
	WSACleanup();
	return result;
#else
	printf("linux not have is_occupy\r\n");
	return false;
#endif
}

std::string ylib::network::size_name(double size, uint32 fixe)
{
	double num = 0.00;
	std::string unit;
	if (size <= 1024)
	{
		unit = "B";
		num = size;
	}
	else if (size <= pow((double)1024, (double)2))
	{
		num = size / 1024;
		unit = "KB";
	}
	else if (size <= pow((double)1024, (double)3))
	{
		num = size / pow((double)1024, (double)2);
		unit = "MB";
	}
	else if (size <= pow((double)1024, (double)4))
	{
		num = size / pow((double)1024, (double)3);
		unit = "GB";
	}
	else if (size <= pow((double)1024, (double)5))
	{
		num = size / pow((double)1024, (double)4);
		unit = "TB";
	}
	else
        return "max";
    return ylib::to_fixed(num, fixe) + unit;
}


bool ylib::network::parse_url_host(const std::string &url, std::string &host)
{
    std::string temp;
    if (url.length() < 10)
        return false;
    if (url.substr(0, 7) == "http://")
        temp = url.substr(7, url.length() - 7);
    else if (url.substr(0, 8) == "https://")
        temp = url.substr(8, url.length() - 8);

    auto __arr = strutils::split(temp,'/');
    if (__arr.size() >= 1)
    {
        temp = __arr[0];
    }
    __arr = strutils::split(temp,':');
    if (__arr.size() == 2)
        host = __arr[0];
    else
        host = temp;
    return true;
}
bool ylib::network::is_ipv4(const std::string& value)
{
    std::regex ipv4Regex("^(?:[0-9]{1,3}\\.){3}[0-9]{1,3}$");
    return std::regex_match(value, ipv4Regex);
}
bool ylib::network::is_ipv6(const std::string& value)
{
    std::regex ipv4Regex("^(?:[A-Fa-f0-9]{1,4}:){7}[A-Fa-f0-9]{1,4}$");
    return std::regex_match(value, ipv4Regex);
}
bool ylib::network::is_domain(const std::string& value)
{
    return !is_ipv4(value) && !is_ipv6(value);
}
#endif



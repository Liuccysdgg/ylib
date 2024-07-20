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
#include <unordered_map>
#include "util/strutils.h"
#ifdef _WIN32
#pragma warning(disable: 4996)
#endif
void ylib::network::content_type(const std::string& extName, std::string& type)
{
    static const std::unordered_map<std::string, std::string> mimeTypes = {
         {"html", "text/html"},
         {"htm", "text/html"},
         {"css", "text/css"},
         {"text", "text/plain"},
         {"txt", "text/plain"},
         {"icon", "image/x-icon"},
         {"ico", "image/x-icon"},
         {"jpeg", "image/jpeg"},
         {"jpg", "image/jpeg"},
         {"mp3", "audio/mpeg"},
         {"pdf", "application/pdf"},
         {"swf", "application/x-shockwave-flash"},
         {"wav", "audio/x-wav"},
         {"zip", "application/zip"},
         {"mp4", "video/mpeg4"},
         {"png", "image/png"},
         {"gif", "image/gif"},
         {"bmp", "image/bmp"},
         {"svg", "image/svg+xml"},
         {"rmvb", "application/vnd.rn-realmedia-vbr"},
         {"js", "application/javascript"},
         {"json", "application/json"},
         {"xml", "application/xml"},
         {"ttf", "application/octet-stream"},
         {"woff", "application/font-woff"},
         {"woff2", "application/font-woff2"},
         {"eot", "application/vnd.ms-fontobject"},
         {"otf", "font/otf"},
         {"webp", "image/webp"},
         {"avi", "video/x-msvideo"},
         {"mov", "video/quicktime"},
         {"flv", "video/x-flv"},
         {"mkv", "video/x-matroska"},
         {"doc", "application/msword"},
         {"docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
         {"ppt", "application/vnd.ms-powerpoint"},
         {"pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
         {"xls", "application/vnd.ms-excel"},
         {"xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"}
    };

    auto it = mimeTypes.find(extName);
    if (it != mimeTypes.end())
    {
        type = it->second;
    }
    else
    {
        type = "application/octet-stream"; // default MIME type for unknown extensions
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
    {
        host = __arr[0]+":"+__arr[1];
    }
        
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
bool ylib::network::parse_multipart_form_data(const ylib::buffer& data, const std::string& boundary, std::vector<network::http::multipart>& parts)
{
    ylib::buffer bbd;
    bbd.append("--"+boundary+"\r\n");
    auto bbd_start_list = data.find_list(bbd);
    if (bbd_start_list.size() == 0)
        return true;
    ylib::buffer bbd2;
    bbd2.append("--" + boundary + "--\r\n");
    auto bbda_end = data.find(bbd2,bbd_start_list[bbd_start_list.size()-1]);
    for (size_t i = 0; i < bbd_start_list.size(); i++)
    {
        size_t bbd_end = bbd_start_list[i] + bbd.length();

        auto body_start = data.find("\r\n\r\n",4, bbd_start_list[i] + bbd.length());
        network::http::multipart mult;
        // 头部
        std::string header = data.sub(bbd_end, body_start - bbd_end);
        {
            header = strutils::replace(header,"\r\n",";");
            auto parr = strutils::split(header,";");
            for (size_t f = 0; f < parr.size(); f++)
            {
                auto aarr = strutils::split(parr[f], ": ");
                if (aarr.size() == 1)
                    aarr = strutils::split(parr[f], '=');

                std::string key, value;
                if (aarr.size() > 0)
                    key = strutils::trim(aarr[0], { ' ' });
                if (aarr.size() > 1)
                    value = strutils::trim(aarr[1], { ' ' });
                if (key == "name" || key == "filename")
                    value = strutils::trim(value, {'\"'});
                mult.param.emplace(key, value);
            }
        }
        body_start += 4;

        mult.offset = body_start;
        if (i + 1 == bbd_start_list.size())
        {
            mult.length = bbda_end - mult.offset - 2;
        }
        else
        {
            mult.length = bbd_start_list[i + 1] - mult.offset - 2;
        }
        body_start += mult.length;
        parts.push_back(mult);
    }
    

    return true;
}
#endif




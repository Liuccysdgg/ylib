﻿/*Software License

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

#include "net/ftp_client.h"
#if USE_NET_FTP
#include <iostream>
#define FTP_TYPE_WIN_API
//#define FTP_TYPE_FTPLIBCPP
#ifdef _WIN32
#include <Windows.h>
#include <WinInet.h>
#include <tcpmib.h>
#include <IPHlpApi.h>
#else
#endif
#include "util/strutils.h"

#ifdef _WIN32
ylib::network::ftp::client::client()
{
	m_ptr = nullptr;
	m_download = nullptr;
	m_cb_downloading = nullptr;
	m_cb_downloaded = nullptr;
}

ylib::network::ftp::client::~client()
{
	if (m_download != nullptr)
		delete m_download;
}

bool ylib::network::ftp::client::connect(const std::string& ipaddress, const std::string& username, const std::string& password, ushort port)
{
	close();
#ifdef FTP_TYPE_WIN_API
	HINTERNET hintInternetOpen = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	auto nCount = 0;
	HINTERNET    hConnect = InternetConnect(hintInternetOpen, ipaddress.c_str(), port, username.c_str(), password.c_str(), INTERNET_SERVICE_FTP, 0, 0);
	if (!hConnect)
		return false;
	m_ptr = hConnect;
	return true;
#else
	m_ptr = new ftplib();
	if (((ftplib*)m_ptr)->Connect(std::string(ipaddress + std::string::from(port)).c_str()) != 0)
		return false;
	return true;
#endif
	
}
void ylib::network::ftp::client::close()
{
#ifdef FTP_TYPE_WIN_API
	if(m_ptr!=nullptr)
		InternetCloseHandle(m_ptr);
#else
	((ftplib*)m_ptr)->Quit();
	delete ((ftplib*)m_ptr);
#endif
	m_ptr = nullptr;
}
bool ylib::network::ftp::client::upload(const std::string& local_filepath, const std::string& remote_filepath)
{
#ifdef FTP_TYPE_WIN_API
	
	auto hFTPFile = ::FtpOpenFile(m_ptr, remote_filepath.c_str(), GENERIC_WRITE, FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, NULL);
	HANDLE hFile = ::CreateFile(local_filepath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ |
		FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
		return FALSE;

	auto UploadDataSize = ::GetFileSize(hFile, NULL);
	auto pUploadData = new BYTE[UploadDataSize];
	DWORD dwRet = 0;
	DWORD bRet = 0;
	DWORD dwBytesReturn = 0;
	::ReadFile(hFile, pUploadData, UploadDataSize, &dwRet, NULL);
	UploadDataSize = dwRet;

	bRet = ::InternetWriteFile(hFTPFile, pUploadData, UploadDataSize, &dwBytesReturn);
	if (FALSE == bRet)
	{
		delete[]pUploadData;
		return FALSE;
	}
	delete[]pUploadData;
	return TRUE;
#else
	/*((ftplib*)m_ptr)->
	return false;*/
#endif


}
#define FTP_OPEN_1																																																								\
		auto hFTPFile = FtpOpenFile(m_ptr, remote_filepath.c_str(), GENERIC_READ, FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, NULL);						\
		if (hFTPFile == 0)																																																								\
		{																																																														\
			std::cout << "[FTP][CLIENT] open remote failed" << std::endl;																																				\
			return false;																																																									\
		}																																																														\
		ylib::file* file = new ylib::file;																																																\
		if (!file->open(local_filepath))																																																		\
		{																																																														\
			std::cout<<"[FTP][CLIENT] open local failed" <<std::endl;																												\
			InternetCloseHandle(hFTPFile);\
			delete file;																																																									\
			return false;																																																									\
		}
bool ylib::network::ftp::client::download(const std::string& local_filepath, const std::string& remote_filepath, size_t param, CALLBACK_FTPCLIENT_DOWNLOADING downloading, CALLBACK_FTPCLIENT_DOWNLOADED downloaded)
{
#ifdef FTP_TYPE_WIN_API
	auto hFTPFile = FtpOpenFile(m_ptr, remote_filepath.c_str(), GENERIC_READ, FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, NULL);						
	if (hFTPFile == 0)																																																								
		return false;																																																									
	ylib::file_io* file = new ylib::file_io;
	if (!file->open(local_filepath))																																																		
	{																																																														
		InternetCloseHandle(hFTPFile);
		delete file;																																																									
		return false;																																																									
	}
	
	::ylib::ithread::stop();
	::ylib::ithread::wait();
	if (m_download != nullptr)
		delete m_download;
	m_download = new network::ftp::DownInfo;
	m_download->local_path = local_filepath;
	m_download->remote_path = remote_filepath;
	m_download->downloaded = downloaded;
	m_download->downloading = downloading;
	m_download->fileptr = hFTPFile;
	m_download->file = file;
	m_download->param = param;

	ylib::ithread::start();
#else

#endif
	return true;
}
void ylib::network::ftp::client::wait()
{
	ylib::ithread::wait();
}
bool ylib::network::ftp::client::download(const std::string& local_filepath, const std::string& remote_filepath)
{
#ifdef FTP_TYPE_WIN_API
	auto hFTPFile = FtpOpenFile(m_ptr, remote_filepath.c_str(), GENERIC_READ, FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, NULL);						
	if (hFTPFile == 0)																																																								
	{									
		
		 
		std::cout << "[FTP][CLIENT] open remote failed >" << remote_filepath.c_str() << ":" << GetLastError() << std::endl;
		CHAR szError[256];
		DWORD dwErrorSize = 256;
		DWORD dwLastErrorMsg;
		InternetGetLastResponseInfo(&dwLastErrorMsg, szError, &dwErrorSize);
		std::string m_strTmperror = szError;
		std::cout<< m_strTmperror.c_str() << std::endl;
		return false;																																																									
	}																																																														
	ylib::file_io* file = new ylib::file_io;
	if (!file->open(local_filepath))																																																		
	{																																																														
		std::cout << "[FTP][CLIENT] open local failed" << std::endl;																												
		InternetCloseHandle(hFTPFile); 
		delete file;																																																									
		return false;																																																									
	}


	ylib::ithread::stop();
	ylib::ithread::wait();
	if (m_download != nullptr)
		delete m_download;
	m_download = new network::ftp::DownInfo;
	m_download->local_path = local_filepath;
	m_download->remote_path = remote_filepath;
	m_download->downloaded = nullptr;
	m_download->downloading = nullptr;
	m_download->fileptr = hFTPFile;
	m_download->file = file;
	m_download->param = 0;

	ylib::ithread::start();
	ylib::ithread::wait();
	return m_download->ok;
#else
#endif
}
bool ylib::network::ftp::client::create_dir(const std::string& remote_path)
{
#ifdef FTP_TYPE_WIN_API
	return FtpCreateDirectory(m_ptr,remote_path.c_str());
#else
	return false;
#endif
}
bool ylib::network::ftp::client::delete_dir(const std::string& remote_path)
{
#ifdef FTP_TYPE_WIN_API
	return FtpRemoveDirectory(m_ptr, remote_path.c_str());
#else
	return false;
#endif
}

bool ylib::network::ftp::client::delete_file(const std::string& remote_path)
{
#ifdef FTP_TYPE_WIN_API
	return FtpDeleteFile(m_ptr,remote_path.c_str());
#else
	return false;
#endif
}
bool ylib::network::ftp::client::run()
{
#ifdef FTP_TYPE_WIN_API
	m_download->ok = true;
	// 获取文件大小
	m_download->size = FtpGetFileSize(m_download->fileptr, NULL);
	// 申请动态内存
	const DWORD dwBufferSize = 4096;
	DWORD dwBytesReturn = 0;
	DWORD dwOffset = 0;
	auto pBuf = new BYTE[dwBufferSize];
	RtlZeroMemory(pBuf, dwBufferSize);
	// 接收数据
	do
	{
		auto bRet = InternetReadFile(m_download->fileptr, pBuf, dwBufferSize, &dwBytesReturn);
		if (FALSE == bRet)
		{
			m_download->ok = false;
			break;
		}
		dwOffset = dwOffset + dwBytesReturn;
		m_download->file->write(ylib::buffer((char*)pBuf, dwBytesReturn));
		if(m_download->downloading != nullptr)
			m_download->downloading(this, m_download->size, dwOffset,m_download->param);
	} while (m_download->size > dwOffset);
	if (m_download->downloaded != nullptr)
		m_download->downloaded(this, m_download->ok, m_download->param);

	m_download->file->close();
	
	// 将数据保存为文件
	//Ftp_SaveToFile(SavePath, pDownloadData, dwDownloadDataSize);
	// 释放内存
	delete[] pBuf;
	//pDownloadData = NULL;
	InternetCloseHandle(m_download->fileptr);
	return false;
#else
	return false;
#endif
}
#endif
#endif
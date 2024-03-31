#pragma once
#include "QtCore/qobject.h"
#include "QtCore/qtimer.h"
#include <string>
#include <Kaguya/kaguya.hpp>
#include "yutil/queue.hpp"
#include "ynet/http_client_plus.h"
#include "QsBuffer.h"
class QsHttpClient
{
public:
	enum MsgType
	{
		END,
		FAILED,
		DOWNING
	};
	struct MsgInfo
	{
		MsgType type;
		ylib::json data;
	};
public:
	QsHttpClient();
	~QsHttpClient();
	static void regist(kaguya::State& T);
private:
	bool get(const std::string& url,bool wait);
	bool post(const std::string& url,const std::string& json_data);
	void close();
	bool head(const std::string& url);
	void setHeader(const std::string& name,const std::string& value);
	std::string responseHeader(const std::string& name);
	QsBuffer response();
	void onDownEnd(kaguya::LuaFunction);
	void onDownFailed(kaguya::LuaFunction);
	void onDowning(kaguya::LuaFunction);
//signals:
//	void sigDowning(unsigned long long alldownsize,unsigned long long allsize);
//	void sigDownEnd();
//	void sigDownFailed();
private:
	kaguya::LuaFunction m_onDownEnd;
	kaguya::LuaFunction m_onDownFailed;
	kaguya::LuaFunction m_onDowning;
	timestamp m_pre_onDowning_callback_msec = 0;
	ylib::buffer m_response;
	network::http::client_plus m_client;
	ylib::queue<MsgInfo> m_msg_queue;
	QTimer m_msg_timer;
};


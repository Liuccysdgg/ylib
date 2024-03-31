#include "yqlua/QsHttpClient.h"
#include "yutil/time.h"
#include "yqlua/QsBuffer.h"

QsHttpClient::QsHttpClient()
{
	
	QObject::connect(&m_msg_timer, &QTimer::timeout, [&]() {
		QsHttpClient::MsgInfo info;
		if (m_msg_queue.pop(info))
		{
			switch (info.type)
			{
			case END:
				if (!m_onDownEnd.isNilref())
					m_onDownEnd();
				m_msg_timer.stop();
				break;
			case FAILED:
				if (!m_onDownFailed.isNilref())
					m_onDownFailed();
				m_msg_timer.stop();
				break;
			case DOWNING:
				if (!m_onDowning.isNilref())
					m_onDowning(info.data["alldownsize"].to<uint64>(), info.data["allsize"].to<uint64>());
				break;
			default:
				break;
			}
		}
	});
	
	m_client.on_down_end([&](network::http::client_plus& client) {
		m_msg_queue.push({ END,ylib::json() });
	});
	m_client.on_down_failed([&](network::http::client_plus& client) {
		m_msg_queue.push({ FAILED,ylib::json() });
	});
	m_client.on_down_ing([&](void* data, uint32 downsize, uint64 alldownsize, uint64 allsize, network::http::client_plus& client)->bool {
		m_response.append((const char*)data, downsize);
		if (m_pre_onDowning_callback_msec == 0)
		{
			m_pre_onDowning_callback_msec = time::now_msec();
			QsHttpClient::MsgInfo info;
			info.data["alldownsize"] = alldownsize;
			info.data["allsize"] = allsize;
			info.type = DOWNING;
			m_msg_queue.push(info);
			return true;
		}
		if (m_pre_onDowning_callback_msec + 1000 > time::now_msec())
			return true;
		m_pre_onDowning_callback_msec = time::now_msec();
		
		QsHttpClient::MsgInfo info;
		info.data["alldownsize"] = alldownsize;
		info.data["allsize"] = allsize;
		info.type = DOWNING;
		m_msg_queue.push(info);


		return true;
	});


	
}

QsHttpClient::~QsHttpClient()
{
}

void QsHttpClient::regist(kaguya::State& T)
{
	T["QsHttpClient"].setClass(kaguya::UserdataMetatable<QsHttpClient>()
		.setConstructors<QsHttpClient()>()
		.addFunction("get", &QsHttpClient::get)
		.addFunction("post", &QsHttpClient::post)
		.addFunction("response", &QsHttpClient::response)
		.addFunction("onDownEnd", &QsHttpClient::onDownEnd)
		.addFunction("onDownFailed", &QsHttpClient::onDownFailed)
		.addFunction("onDowning", &QsHttpClient::onDowning)
		.addFunction("close", &QsHttpClient::close)
		.addFunction("setHeader", &QsHttpClient::setHeader)
		.addFunction("responseHeader", &QsHttpClient::responseHeader)
		.addFunction("head", &QsHttpClient::head)
	);
}


bool QsHttpClient::get(const std::string& url, bool wait)
{
	m_msg_timer.start(100);
	m_msg_queue.clear();
	m_response.clear();
	bool result =  m_client.get(url, std::map<std::string, std::string>(), wait);
	return result;
}

bool QsHttpClient::post(const std::string& url, const std::string& json_data)
{
	m_msg_timer.start(100);
	m_msg_queue.clear();
	m_response.clear();
	bool result =  m_client.post(url, ylib::json::from(json_data));
	return result;
}

void QsHttpClient::close()
{
	m_msg_queue.clear();
	m_client.close();
}
bool QsHttpClient::head(const std::string& url)
{
	m_msg_timer.start(100);
	m_msg_queue.clear();
	m_response.clear();
	bool result = m_client.head(url);
	return result;
}
void QsHttpClient::setHeader(const std::string& name, const std::string& value)
{
	m_client.headers_request().set(name,value);

}
std::string QsHttpClient::responseHeader(const std::string& name)
{
	return m_client.headers_response().get(name).to_string();
}
QsBuffer QsHttpClient::response()
{
	QsBuffer buffer;
	((ylib::buffer*)&buffer)->append(m_response);
	return buffer;
}

void QsHttpClient::onDownEnd(kaguya::LuaFunction callback)
{
	m_onDownEnd = callback;
}

void QsHttpClient::onDownFailed(kaguya::LuaFunction callback)
{
	m_onDownFailed = callback;
}

void QsHttpClient::onDowning(kaguya::LuaFunction callback)
{
	m_onDowning = callback;
}

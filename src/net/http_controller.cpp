#include "net/http_controller.h"
#if USE_NET_HTTP_WEBSITE

#include "base/exception.h"
ylib::network::http::controller::controller()
{
	m_reqpack = nullptr;
}
ylib::network::http::controller::~controller()
{

}

std::string ylib::network::http::controller::qry_string(const std::string& name, bool exception)
{
	
	std::string value;
	bool result = request_param(name, value);

	if (!result)
	{
		if (exception == true)
			throw ylib::exception("The request parameter '" + name + "' was not found");
		else
			return "";
	}
	return value;
}
int32  ylib::network::http::controller::qry_int32(const std::string& name, bool exception)
{
	std::string _param_value_;
    bool result = request_param(name, _param_value_);
    int32 value = ylib::stoi(_param_value_);
	if (!result)
	{
		if (exception == true)
			throw ylib::exception("The request parameter '" + name + "' was not found");
		else
			return 0;
	}
    if (!strutils::is_num(_param_value_))
	{
		if (exception == true)
			throw ylib::exception("The request parameter '" + name + "' not int32 type");
		else
			return 0;
			
	}
	return value;
}
uint32  ylib::network::http::controller::qry_uint32(const std::string& name, bool exception)
{
	std::string _param_value_;
    bool result = request_param(name, _param_value_);
    uint32 value =ylib::stoi(_param_value_);
	if (!result)
	{
		if (exception == true)
			throw ylib::exception("The request parameter '" + name + "' was not found");
		else
			return 0;
	}
    if (!strutils::is_num(_param_value_))
	{
		if (exception == true)
			throw ylib::exception("The request parameter '" + name + "' not uint32 type");
		else
			return 0;
	}
	return value;
}
int64  ylib::network::http::controller::qry_int64(const std::string& name, bool exception)
{
	std::string _param_value_;
    bool result = request_param(name, _param_value_);
    int64 value = ylib::stoll(_param_value_);
	if (!result)
	{
		if (exception == true)
			throw ylib::exception("The request parameter '" + name + "' was not found");
		else
			return 0;
	}
    if (!strutils::is_num(_param_value_))
	{
		if (exception == true)
			throw ylib::exception("The request parameter '" + name + "' not int64 type");
		else
			return 0;
			
	}
	return value;
}
uint64  ylib::network::http::controller::qry_uint64(const std::string& name, bool exception)
{
	std::string _param_value_;
    bool result = request_param(name, _param_value_);
    uint64 value = ylib::stoull(_param_value_);
	if (!result)
	{
		if (exception == true)
			throw ylib::exception("The request parameter '" + name + "' was not found");
		else
			return 0;
	}
    if (!strutils::is_num(_param_value_))
	{
		if (exception == true)
			throw ylib::exception("The request parameter '" + name + "' not uint64 type");
		else
			return 0;
	}
	return value;
}
double  ylib::network::http::controller::qry_double(const std::string& name, bool exception)
{
	std::string _param_value_;
    bool result = request_param(name, _param_value_);
    double value = ylib::stod(_param_value_);
	if (!result)
	{
		if (exception == true)
			throw ylib::exception("The request parameter '" + name + "' was not found");
		else
			return 0.0f;
	}
	return value;
}
float  ylib::network::http::controller::qry_float(const std::string& name, bool exception)
{
	std::string _param_value_;
    bool result = request_param(name, _param_value_);
    float value = ylib::stof(_param_value_);
	if (!result)
	{
		if (exception == true)
			throw ylib::exception("The request parameter '" + name + "' was not found");
		else
			return 0.0f;
	}
	return value;
}
bool  ylib::network::http::controller::qry_empty(const std::string& name, bool exception)
{
	if (request()->parser()->url_param_exist(name) == false && request()->parser()->body_param_exist(name) == false)
		return false;
	return true;
}
bool ylib::network::http::controller::qry_bool(const std::string& name, bool exception)
{
	std::string _param_value_;
	bool result = request_param(name, _param_value_);
	if (!result)
	{
		if (exception == true)
			throw ylib::exception("The request parameter '" + name + "' was not found");
		else
			return false;
	}
	return _param_value_ == "true";
}
ylib::buffer ylib::network::http::controller::qry_buffer(const std::string& name, bool exception)
{
	network::http::form_info form_info;
	if (request()->parser()->form()->get(name, form_info) == false)
	{
		if (exception == true)
			throw ylib::exception("The request parameter '" + name + " was not found");
		else
			return ylib::buffer();
	}

	if (form_info.start != -1 && form_info.length != -1 && form_info.length != 0)
	{
		return m_reqpack->data().sub(form_info.start,form_info.length);
	}
	else
	{
		if (exception == true)
			throw ylib::exception("failed to parse buffer, start=" + std::to_string(form_info.start) + ", length=" + std::to_string(form_info.length));
		else
			return ylib::buffer();
	}
}
bool ylib::network::http::controller::request_param(const std::string& name, std::string& value)
{
	if (request()->parser()->url_param(name, value) == false)
		return request()->parser()->body_param(name, value);
	return true;
}
void ylib::network::http::controller::send(const ylib::json& data)
{
	response()->send(data);
}
network::http::request* ylib::network::http::controller::request()
{
	return m_reqpack->request();
}
network::http::response* ylib::network::http::controller::response()
{
	return m_reqpack->response();
}
#endif

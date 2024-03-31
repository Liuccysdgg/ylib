#include "ynet/http_controller.h"
#if USE_NET_HTTP_WEBSITE

#include "ybase/exception.h"
ylib::network::http::controller::controller()
{
	m_reqpack = nullptr;
}
ylib::network::http::controller::~controller()
{

}

std::string ylib::network::http::controller::qry_string(const std::string& name, uint32 pmin, uint32 pmax)
{
	
	std::string value;
	bool result = request_param(name, value);

	if (!result)
	{
        throw ylib::exception("The request parameter '"+name+"' was not found");
	}
	if (value.length() < pmin)
	{
        throw ylib::exception("The format of parameter '" + name + "'  does not meet the conditions, it should be >"+std::to_string(pmin));
	}
	if (value.length() > pmax)
	{
        throw ylib::exception("The format of parameter  '" + name + "'  does not meet the conditions, it should be <" + std::to_string(pmax));
	}
	return value;
}
int32  ylib::network::http::controller::qry_int32(const std::string& name, int32 pmin, int32 pmax)
{
	std::string _param_value_;
    bool result = request_param(name, _param_value_);
    int32 value = ylib::stoi(_param_value_);
	if (!result)
	{
        throw ylib::exception("The request parameter '" + name + "' was not found");
	}
    if (!strutils::is_num(_param_value_))
	{
        throw ylib::exception("The request parameter '" + name + "' not int32 type");
	}
	if (value < pmin)
	{
        throw ylib::exception("The format of parameter '" + name + "'  does not meet the conditions, it should be >" + std::to_string(pmin));
	}
	if (value > pmax)
	{
        throw ylib::exception("The format of parameter  '" + name + "'  does not meet the conditions, it should be <" + std::to_string(pmax));
	}
	return value;
}
uint32  ylib::network::http::controller::qry_uint32(const std::string& name, uint32 pmin, uint32 pmax)
{
	std::string _param_value_;
    bool result = request_param(name, _param_value_);
    uint32 value =ylib::stoi(_param_value_);
	if (!result)
	{
        throw ylib::exception("The request parameter '" + name + "' was not found");
	}
    if (!strutils::is_num(_param_value_))
	{
        throw ylib::exception("The request parameter '" + name + "' not uint32 type");
	}
	if (value < pmin)
	{
        throw ylib::exception("The format of parameter '" + name + "'  does not meet the conditions, it should be >" + std::to_string(pmin));
	}
	if (value > pmax)
	{
        throw ylib::exception("The format of parameter  '" + name + "'  does not meet the conditions, it should be <" + std::to_string(pmax));
	}
	return value;
}
int64  ylib::network::http::controller::qry_int64(const std::string& name, int64 pmin, int64 pmax)
{
	std::string _param_value_;
    bool result = request_param(name, _param_value_);
    int64 value = ylib::stoll(_param_value_);
	if (!result)
	{
        throw ylib::exception("The request parameter '" + name + "' was not found");
	}
    if (!strutils::is_num(_param_value_))
	{
        throw ylib::exception("The request parameter '" + name + "' not int64 type");
	}
	if (value < pmin)
	{
        throw ylib::exception("The format of parameter '" + name + "'  does not meet the conditions, it should be >" + std::to_string(pmin));
	}
	if (value > pmax)
	{
        throw ylib::exception("The format of parameter  '" + name + "'  does not meet the conditions, it should be <" + std::to_string(pmax));
	}
	return value;
}
uint64  ylib::network::http::controller::qry_uint64(const std::string& name, uint64 pmin, uint64 pmax)
{
	std::string _param_value_;
    bool result = request_param(name, _param_value_);
    uint64 value = ylib::stoull(_param_value_);
	if (!result)
	{
        throw ylib::exception("The request parameter '" + name + "' was not found");
	}
    if (!strutils::is_num(_param_value_))
	{
        throw ylib::exception("The request parameter '" + name + "' not uint64 type");
	}
	if (value < pmin)
	{
        throw ylib::exception("The format of parameter '" + name + "'  does not meet the conditions, it should be >" + std::to_string(pmin));
	}
	if (value > pmax)
	{
        throw ylib::exception("The format of parameter  '" + name + "'  does not meet the conditions, it should be <" + std::to_string(pmax));
	}
	return value;
}
double  ylib::network::http::controller::qry_double(const std::string& name,double pmin, double pmax)
{
	std::string _param_value_;
    bool result = request_param(name, _param_value_);
    double value = ylib::stod(_param_value_);
	if (!result)
	{
        throw ylib::exception("The request parameter '" + name + "' was not found");
	}
	if (value < pmin)
	{
        throw ylib::exception("The format of parameter '" + name + "'  does not meet the conditions, it should be >" + std::to_string(pmin));
	}
	if (value > pmax)
	{
        throw ylib::exception("The format of parameter  '" + name + "'  does not meet the conditions, it should be <" +std::to_string(pmax));
	}
	return value;
}
float  ylib::network::http::controller::qry_float(const std::string& name, float pmin, float pmax)
{
	std::string _param_value_;
    bool result = request_param(name, _param_value_);
    float value = ylib::stof(_param_value_);
	if (!result)
	{
        throw ylib::exception("The request parameter '" + name + "' was not found");
	}
	if (value < pmin)
	{
        throw ylib::exception("The format of parameter '" + name + "'  does not meet the conditions, it should be >" + std::to_string(pmin));
	}
	if (value > pmax)
	{
        throw ylib::exception("The format of parameter  '" + name + "'  does not meet the conditions, it should be <" + std::to_string(pmax));
	}
	return value;
}
bool  ylib::network::http::controller::qry_empty(const std::string& name)
{
	if (request()->parser()->url_param_exist(name) == false && request()->parser()->body_param_exist(name) == false)
		return false;
	return true;
}
bool ylib::network::http::controller::qry_bool(const std::string& name)
{
	std::string _param_value_;
	bool result = request_param(name, _param_value_);
	if (!result)
	{
        throw ylib::exception("The request parameter '" + name + "' was not found");
	}
	return _param_value_ == "true";
}
ylib::buffer ylib::network::http::controller::qry_buffer(const std::string& name)
{
	network::http::form_info form_info;
	if (request()->parser()->form()->get(name, form_info) == false)
	{
        throw ylib::exception("The request parameter '" + name + " was not found");
	}

	if (form_info.start != -1 && form_info.length != -1 && form_info.length != 0)
	{
		return m_reqpack->data().sub(form_info.start,form_info.length);
	}
	else
	{
        throw ylib::exception("failed to parse buffer, start="+std::to_string(form_info.start) + ", length="+std::to_string(form_info.length));
	}
}
bool ylib::network::http::controller::request_param(const std::string& name, std::string& value)
{
	if (request()->parser()->url_param(name, value) == false)
		return request()->parser()->body_param(name, value);
	return true;
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

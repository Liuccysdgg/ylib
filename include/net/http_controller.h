#pragma once
#include "http_define.h"
#if USE_NET_HTTP_WEBSITE
#include "http_request.h"
#include "http_response.h"
#include "http_reqpack.h"
#include "http_interface.h"
#define _DBL_MIN          2.2250738585072014e-308 // min positive value
#define _DBL_MAX          1.7976931348623158e+308 // max value
#define _INT_MIN     (-2147483647 - 1)
#define _INT_MAX       2147483647
#define _UINT_MAX      0xffffffff
#define _LONG_MIN    (-2147483647L - 1)
#define _LONG_MAX      2147483647L
#define _ULONG_MAX     0xffffffff
#ifndef _LLONG_MAX
#define _LLONG_MAX     9223372036854775807
#endif
#define _LLONG_MIN   (-9223372036854775807 - 1)
#define _ULLONG_MAX    0xffffffffffffffff
#define _INT8_MIN         (-127 - 1)
#define _INT16_MIN        (-32767 - 1)
#define _INT32_MIN        (-2147483647 - 1)
#define _INT64_MIN        (-9223372036854775807 - 1)
#define _INT8_MAX         127
#define _INT16_MAX        32767
#define _INT32_MAX        2147483647
#define _INT64_MAX        9223372036854775807
#define _UINT8_MAX        0xff
#define _UINT16_MAX       0xffff
#define _UINT32_MAX       0xffffffff
#define _UINT64_MAX       0xffffffffffffffff
#define _FLT_MIN          1.175494351e-38F        // min normalized positive value
#define _FLT_MAX          3.402823466e+38F        // max value
using namespace ylib::network::http;

// 成功回复
#define REPLY_SUCC rpjson()["code"] = 200;return RT_OK
// 自定义回复
#define REPLY(CODE,MSG) rpjson()["code"] = CODE;rpjson()["msg"]=MSG;return RT_OK
// 失败回复
#define REPLY_ERROR(MSG) rpjson()["code"] = -1;rpjson()["msg"]=MSG;return RT_OK
/**********************************************************
 * Class：Http控制器接口
 *********************************************************/
//#define check_qry_json(NAME) request()->parser()->json()[NAME].is_empty()
#define qry_json_string(NAME) request()->parser()->json()[NAME].to<std::string>(true)
#define qry_json_uint32(NAME) request()->parser()->json()[NAME].to<uint32>(true)
#define qry_json_uint64(NAME) request()->parser()->json()[NAME].to<uint64>(true)
#define qry_json_int32(NAME) request()->parser()->json()[NAME].to<int32>(true)
#define qry_json_double(NAME) request()->parser()->json()[NAME].to<double>(true)
#define qry_json_short(NAME) request()->parser()->json()[NAME].to<short>(true)
#define qry_json_bool(NAME) request()->parser()->json()[NAME].to<bool>(true)
namespace ylib
{
    namespace network
    {
        namespace http
        {
            class router;
            class controller :public network::http::interface_
            {
            public:
                controller();
                ~controller();
                const std::shared_ptr<network::http::request>& request();
                const std::shared_ptr<network::http::response>& response();

             

                // 获取回复JSON
                inline ylib::json& rpjson() { return response()->sjson["data"]; }
                inline ylib::json& rpcode() { return response()->sjson["code"]; }
                //inline ylib::json& rp() { return response()->sjson["code"]; }
                inline void rp(int code, const std::string& msg = "", const ylib::json& data = ylib::json())
                {
                    response()->sjson["code"] = code;
                    response()->sjson["msg"] = msg;
                    response()->sjson["data"] = data;
                };
                inline ylib::json rqjson() { return ylib::json::from(request()->body().to_string()); }
                void send(const ylib::json& data);

                friend class router;
            private:
                network::http::reqpack* m_reqpack = nullptr;
            };
        }
    }
}
#endif

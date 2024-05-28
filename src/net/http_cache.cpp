#include "net/http_cache.h"
#include "util/codec.h"
#include "util/file.h"
#include "util/system.h"
#include "util/time.h"
#include "util/strutils.h"
#include "net/http_reqpack.h"
#include "net/http_response.h"
#include "base/conversion.h"
ylib::network::http::cache::cache()
{
}
ylib::network::http::cache::~cache()
{

}
bool ylib::network::http::cache::start(const server_cache_config& config)
{
    m_config = config;
    ylib::file::create_dir(m_config.dirpath,true);
    return true;
}
bool ylib::network::http::cache::enable(){
    return m_config.enable;
}
std::string ylib::network::http::cache::make_key(const std::string& filepath)
{
    return codec::md5(filepath);
    //if (m_keys.size() > 1000000)
    //    m_keys.clear();
    //std::string key;
    //if (m_keys.get(filepath, key) == false)
    //{
    //    key = codec::md5(filepath);
    //    m_keys.set(filepath, key, true);
    //}
    //return key;
}
void ylib::network::http::cache::stop()
{

}

bool ylib::network::http::cache::find_ext(const std::string& filepath)
{
    std::string ext = ylib::file::ext(filepath);
    ext = strutils::change_case(ext,false);
    for(size_t i=0;i<m_config.exts.size();i++)
    {
        if(ext == m_config.exts[i]){
            return true;
        }
    }
    return false;
}
std::string ylib::network::http::cache::make_cache_filepath(const std::string& filepath)
{
    std::string key = make_key(filepath);
    std::string cache_path = m_config.dirpath;
    for(size_t i=0;i<1;i++)
    {
        cache_path += '/';
        cache_path += key[i];
    }
    if(ylib::file::exist(cache_path) == false)
        ylib::file::create_dir(cache_path,true);
    cache_path += '/';
    cache_path.append(strutils::right(key,key.length()-1));
    return cache_path;
}
bool ylib::network::http::cache::have_send(network::http::reqpack* rp)
{
    if(m_config.enable ==  false)
        return false;

    std::string cache_path = make_cache_filepath(rp->filepath());
    std::string cache_path_conf = cache_path + ".conf";
    std::string cache_path_header = cache_path + ".header";

    if(
        ylib::file::size(cache_path) != -1 &&
        ylib::file::exist(cache_path_conf) != 0 &&
        ylib::file::exist(cache_path_header) != 0
    ){
        std::string conf = ylib::file::read(cache_path_conf).to_string();
        auto conf_arr =strutils::split(conf,"\r\n");
        timestamp start_time_sec = ylib::stoull(conf_arr[0]);
        if(start_time_sec + m_config.timeout_min*60 < ylib::time::now_sec())
            return false;
        // 发送协议头
        {
            ylib::buffer header_value = ylib::file::read(cache_path_header);
            auto fheader = strutils::split(header_value,"\r\n");
            for(size_t i=0;i<fheader.size()/2;i++){
                if(fheader[i].empty()){
                    continue;
                }else{
                    (*rp->response()->headers())[fheader[i*2]] = fheader[i*2+1];
                }
            }
        }
        rp->response()->send_file(cache_path,-1);
        return true;
        
    }
    return false;
}

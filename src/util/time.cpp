// MIT License﻿
// Copyright(c) 2024 FastWeb - fwlua.com - nianhua﻿
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :﻿
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.﻿
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.﻿
// ## Additional Terms for Commercial Use﻿
// This software is licensed for personal, educational, and non - commercial use.
// For commercial use or use within a company, organization, or institution, a
// separate commercial license is required.To obtain a commercial license,
// please contact
// EMail：1585346868@qq.com
// Mobile：17367918735
// QQ：1585346868

#include "util/time.h"

#include <ctime>
#include <time.h>
#include <string.h>
#ifdef _WIN32
#pragma warning(disable:4996)
#include <sys/stat.h>
#include <sys/timeb.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <tlhelp32.h>
#else
#include<sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#endif
const static char* s_week[] = { "Sun, " ,"Mon, " ,"Tues, ","Wed, ","Thur, ","Fri, ","Sat, " };
const static char* s_moth[] = { "Jan " ,"Feb ","Mar ","Apr ","May ","Jun ","Jul " ,"Aug ","Sep " ,"Oct " ,"Nov " ,"Dec " };

timestamp ylib::time::network_msec()
{
    //int32 xit_err = -1;

    //uint64 xut_timev = 0ULL;
    ////======================================
    //// 常用的 NTP 服务器地址列表

    //std::vector< std::string > xvec_ntp_host;
    //xvec_ntp_host.push_back(nstring("1.cn.pool.ntp.org"));
    //xvec_ntp_host.push_back(nstring("2.cn.pool.ntp.org"));
    //xvec_ntp_host.push_back(nstring("3.cn.pool.ntp.org"));
    //xvec_ntp_host.push_back(nstring("0.cn.pool.ntp.org"));
    //xvec_ntp_host.push_back(nstring("cn.pool.ntp.org"));
    //xvec_ntp_host.push_back(nstring("tw.pool.ntp.org"));
    //xvec_ntp_host.push_back(nstring("0.tw.pool.ntp.org"));
    //xvec_ntp_host.push_back(nstring("1.tw.pool.ntp.org"));
    //xvec_ntp_host.push_back(nstring("2.tw.pool.ntp.org"));
    //xvec_ntp_host.push_back(nstring("3.tw.pool.ntp.org"));
    //xvec_ntp_host.push_back(nstring("pool.ntp.org"));
    //xvec_ntp_host.push_back(nstring("time.windows.com"));
    //xvec_ntp_host.push_back(nstring("time.nist.gov"));
    //xvec_ntp_host.push_back(nstring("time-nw.nist.gov"));
    //xvec_ntp_host.push_back(nstring("asia.pool.ntp.org"));
    //xvec_ntp_host.push_back(nstring("europe.pool.ntp.org"));
    //xvec_ntp_host.push_back(nstring("oceania.pool.ntp.org"));
    //xvec_ntp_host.push_back(nstring("north-america.pool.ntp.org"));
    //xvec_ntp_host.push_back(nstring("south-america.pool.ntp.org"));
    //xvec_ntp_host.push_back(nstring("africa.pool.ntp.org"));
    //xvec_ntp_host.push_back(nstring("ca.pool.ntp.org"));
    //xvec_ntp_host.push_back(nstring("uk.pool.ntp.org"));
    //xvec_ntp_host.push_back(nstring("us.pool.ntp.org"));
    //xvec_ntp_host.push_back(nstring("au.pool.ntp.org"));

    ////======================================

    //for (std::vector< std::string >::iterator itvec = xvec_ntp_host.begin(); itvec != xvec_ntp_host.end(); ++itvec)
    //{
    //    xut_timev = 0ULL;
    //    xit_err = ntp_get_time(itvec->c_str(), NTP_PORT, 5000, &xut_timev);
    //    if (0 == xit_err)
    //    {

    //        return xut_timev;
    //        //ntp_tmctxt_bv(xut_timev, &xnpt_timec);   // 转换成 年-月-日_时-分-秒.毫秒 的时间信息
    //        //ts_output(itvec->c_str(), &xnpt_timec);  // 输出时间信息
    //    }
    //    else
    //    {
    //        // 请求失败，可能是因为应答超时......
    //        printf("  %s return error code : %d\n", itvec->c_str(), xit_err);
    //    }
    //}

    ////======================================

    //return xit_err;
    return 0;
}
timestamp ylib::time::to_ts(const std::string& timestr, const std::string& formart)
{
    if (timestr.empty())
        return 0;
    struct tm tm;
    memset(&tm, 0, sizeof(tm));
#ifdef _WIN32
    sscanf_s
#else
    sscanf
#endif
    (timestr.c_str(), formart.c_str(),
        &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
        &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
    tm.tm_year -= 1900;
    tm.tm_mon--;
    return mktime(&tm);
}

timestamp  ylib::time::to_ts(struct ylib::time::datetime& datetime)
{
    timestamp result = 0;
    struct tm tm;
    memset(&tm, 0, sizeof(tm));
    tm.tm_year = datetime.year;
    tm.tm_mon = datetime.month;
    tm.tm_mday = datetime.day;
    tm.tm_hour = datetime.hour;
    tm.tm_min = datetime.minute;
    tm.tm_sec = datetime.second;

    tm.tm_year -= 1900;
    tm.tm_mon--;

    return mktime(&tm);

}

uint32 ylib::time::now_sec()
{
#ifdef _WIN32
    time_t rawtime;
    return (int)::time(&rawtime);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
#endif // _WIN32
}

timestamp ylib::time::now_msec()
{
#ifdef _WIN32
    struct timeb rawtime;
    ftime(&rawtime);
    return rawtime.time * 1000 + rawtime.millitm;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif // _WIN32
}

timestamp ylib::time::now_usec()
{
#ifdef _WIN32
#define EPOCHFILETIME   (116444736000000000UL)
    FILETIME ft;
    LARGE_INTEGER li;
    uint64 tt = 0;
    GetSystemTimeAsFileTime(&ft);
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
    // 从1970年1月1日0:0:0:000到现在的微秒数(UTC时间)
    tt = (li.QuadPart - EPOCHFILETIME) / 10;
    return tt;
#else
    return 0;
#endif

}

std::string ylib::time::now_time(const std::string& format)
{
    time_t t = ::time(0);
    char ch[128];
    tm tm;
#ifdef _WIN32
    tm = *_localtime64(&t);
#else
    tm = *localtime(&t);
#endif
    strftime(ch, sizeof(ch), format.c_str(), &tm); //年-月-日 时-分-秒
    return ch;
}

std::string   ylib::time::format(timestamp time, const std::string& format)
{
//    if (time > 9999999999L)
//        time = (uint64)nstring(nstring::from((int64)time).left(10)).to_int64();
//    else if (time < 0)
//        time = 0;

    time_t t = (time_t)time;
    char ch[128];
    tm tm;
#ifdef _WIN32
    tm = *_localtime64(&t);
#else
    tm = *localtime(&t);
#endif
    strftime(ch, sizeof(ch), format.c_str(), &tm); //年-月-日 时-分-秒
    return ch;
}

void  ylib::time::format(timestamp time, ylib::time::datetime& systime)
{
//    if (time > 9999999999L)
//        time = (uint64)nstring(nstring::from((int64)time).left(10)).to_uint64();
//    else if (time < 0)
//        time = 0;

    time_t t = (time_t)time + 60*60*8;
#ifdef _WIN32
    tm tm = *_gmtime64(&t);
#else
    tm tm = *gmtime(&t);
#endif

    systime.year = tm.tm_year + 1900;

    systime.month = tm.tm_mon + 1;
    systime.day = tm.tm_mday;
    systime.hour = tm.tm_hour;// +now_zone();
    systime.minute = tm.tm_min;
    systime.second = tm.tm_sec;
}
struct ylib::time::datetime ylib::time::now_time2()
{
    ylib::time::datetime result;
    tm tm;
    time_t tt = ::time(nullptr);
    tt += time::now_zone() * 60 * 60 ;
#ifdef WIN32
    tm = *_gmtime64(&tt);
#else
    gmtime_r(&tt, &tm);
#endif
    result.year = tm.tm_year + 1900;
    result.month = tm.tm_mon + 1;
    result.day = tm.tm_mday;
    result.hour = tm.tm_hour;
    result.minute = tm.tm_min;
    result.second = tm.tm_sec;
    return result;
}


int32  ylib::time::now_zone()
{
    time_t time_utc = 0;
    struct tm* p_tm_time = NULL;
    int time_zone = 0;
#ifdef _WIN32
    p_tm_time = _localtime64(&time_utc);
#else
    p_tm_time = localtime(&time_utc);
#endif
    return (p_tm_time->tm_hour > 12) ? (p_tm_time->tm_hour -= 24) : p_tm_time->tm_hour;
}

void  ylib::time::now_gmt(std::string& gmt)
{
    ylib::time::to_gmt(::time(nullptr), gmt);
}

void  ylib::time::to_gmt(const timestamp& timestamp, std::string& gmt)
{
    //static MemoryPool<char, 50> mempool;
    tm tm;
#ifdef WIN32
    tm = *_gmtime64((time_t*)&timestamp);
#else
    gmtime_r((time_t*)&timestamp, &tm);
#endif
    
    char int_string[10];
    memset(int_string, 0, 10);

    
    //char* mem_value = mempool.allocate();
    char mem_value[50];

    size_t index = 0;
    size_t length = 0;
    {
        length = strlen(s_week[tm.tm_wday]);
        memcpy(mem_value + index, s_week[tm.tm_wday], length);
        index += length;
    }
    {
        sprintf(int_string,"%d",tm.tm_mday);
        length = strlen(int_string);
        memcpy(mem_value + index, int_string, length);
        index += length;
    }
    {
        length = 1;
        memcpy(mem_value + index, " ", length);
        index += length;
    }
    {
        length = strlen(s_moth[tm.tm_mon]);
        memcpy(mem_value + index, s_moth[tm.tm_mon], length);
        index += length;
    }
    {
        sprintf(int_string,"%d",tm.tm_year+1900);
        length = strlen(int_string);
        memcpy(mem_value + index, int_string, length);
        index += length;
    }
    {
        length = 1;
        memcpy(mem_value + index, " ", length);
        index += length;
    }
    {
        sprintf(int_string,"%d",tm.tm_hour + now_zone());
        length = strlen(int_string);
        memcpy(mem_value + index, int_string, length);
        index += length;
    }
    {
        length = 1;
        memcpy(mem_value + index, ":", length);
        index += length;
    }

    {
        sprintf(int_string,"%d",tm.tm_min);
        length = strlen(int_string);
        memcpy(mem_value + index, int_string, length);
        index += length;
    }
    {
        length = 1;
        memcpy(mem_value + index, ":", length);
        index += length;
    }


    {
        sprintf(int_string,"%d",tm.tm_sec);
        length = strlen(int_string);
        memcpy(mem_value + index, int_string, length);
        index += length;
    }
    {
        length = 4;
        memcpy(mem_value + index, " GMT", length);
        index += length;
    }
    mem_value[index] = '\0';
    gmt.append(mem_value);
}

timestamp ylib::time::now_zero_sec()
{
    return time::to_ts(now_zero_time());
}

std::string ylib::time::now_zero_time()
{
    return time::format(time::now_sec(), "%Y-%m-%d");
}

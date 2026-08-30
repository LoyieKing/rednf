// ============================================================================
// df_game_r 还原 —— RestrictGeolocation 实现（原 GlobalData.cpp 局部镜像收敛，
// Main 批准：W6_ARAD；声明=权威 RestrictGeolocation.h）。
//
// ORIG 证据：docs/class_func_reports/RestrictGeolocation/*.md（地址见头文件注释）。
// 工程未链 libGeoIP，GeoIP_* 经 dlsym 动态查找【推断，保持原重建口径】。
// ============================================================================

#include "RestrictGeolocation.h"

#include "GlobalData.h"        // s_stream_pool / s_msgq_mgr
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "SigTypes.h"          // SIG_SAVE_GEO_REJECT
#include "GameTypes.h"         // cMyTrace
#include "LogManager.h"

#include <dlfcn.h>
#include <cstring>

// Taiwan 域 internal_stream（TaiwanQueries.h 权威声明；包类型枚举见该头）。
#include "TaiwanInternal.h"

extern CEnvironment* G_CEnvironment();

namespace {

typedef void* (*GeoIPNewFn)(int);
typedef void* (*GeoIPNew2Fn)(int, void*);   // ORIG GeoIP_new(GEOIP_STANDARD=0)
typedef void (*GeoIPDeleteFn)(void*);
typedef const char* (*GeoIPCountryByAddrFn)(void*, const char*);

static GeoIPNewFn get_geoip_new()
{
    return reinterpret_cast<GeoIPNewFn>(dlsym(RTLD_DEFAULT, "GeoIP_new"));
}
static GeoIPDeleteFn get_geoip_delete()
{
    return reinterpret_cast<GeoIPDeleteFn>(dlsym(RTLD_DEFAULT, "GeoIP_delete"));
}
static GeoIPCountryByAddrFn get_geoip_country_by_addr()
{
    return reinterpret_cast<GeoIPCountryByAddrFn>(
        dlsym(RTLD_DEFAULT, "GeoIP_country_code_by_addr"));
}

}  // namespace

RestrictGeolocation::RestrictGeolocation() : m_geoip(0), m_allowCountry(0xb)
{
    // ORIG 0x0817097c：编译器先调基类 ARAD::Singleton<RestrictGeolocation>::Singleton()
    // （报告 call 081713a6）→ m_geoip=0 → unordered_set 以 hash=0xb 构造
    // （报告 call 08171406，四参：this+4, 0xb, &equal_to, &allocator）。体为空。
}

RestrictGeolocation::~RestrictGeolocation()
{
    // ORIG 0x081709fe：closeGeolocation() + 集合析构（成员自动）。
    closeGeolocation();
}

bool RestrictGeolocation::initGeolocation()
{
    // ORIG 0x08170a46：m_geoip 为空则 GeoIP_new(0)；失败返回 0；随后 loadData()。
    if (m_geoip == 0)
    {
        GeoIPNewFn geoipNew = get_geoip_new();
        m_geoip = geoipNew ? geoipNew(0) : 0;
        if (m_geoip == 0)
        {
            return false;
        }
    }
    loadData();
    return true;
}

void RestrictGeolocation::closeGeolocation()
{
    // ORIG 0x08170a88。
    GeoIPDeleteFn geoipDelete = get_geoip_delete();
    if (geoipDelete)
    {
        geoipDelete(m_geoip);
    }
}

void RestrictGeolocation::clearAllowCountry()
{
    // ORIG 0x08170b16。
    m_allowCountry.clear();
}

void RestrictGeolocation::addAllowCountry(std::string country)
{
    // ORIG 0x08170a9e：find!=end 则跳过，否则 insert。
    if (m_allowCountry.find(country) == m_allowCountry.end())
    {
        m_allowCountry.insert(country);
    }
}

bool RestrictGeolocation::isAllowCountryCode(std::string country)
{
    // ORIG 0x0817109a：find(this+0x04, country) != end()。
    return m_allowCountry.find(country) != m_allowCountry.end();
}

void RestrictGeolocation::loadData()
{
    // ORIG 0x08170fec：Acquire → Taiwan::internal_stream(guard,4,-1)（报告
    // loadData.md：localtaiwan/System/RestrictGeolocation.cpp 行 0x8b）→
    // clearAllowCountry → DB_Q 投递。
    Stream* stream = GlobalData::s_stream_pool->Acquire(
        "localtaiwan/System/RestrictGeolocation.cpp", 0x8b);
    CStreamGuard guard(stream, true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(4), -1);
    clearAllowCountry();
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

bool RestrictGeolocation::isAllow(std::string ip)
{
    // ORIG 0x08170ecc：拷贝入参后转查国家码白名单。
    return isAllowCountryCode(ip);
}

void RestrictGeolocation::getCountryCode(char* out, unsigned int outLen, std::string ip)
{
    if (m_geoip == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x7e, 5);
        trace("[Taiwan, GeoIP] Geo instance is null.");
    }
    std::memset(out, 0, outLen);
    GeoIPCountryByAddrFn byAddr = get_geoip_country_by_addr();
    if (byAddr && byAddr(m_geoip, ip.c_str()) != 0)
    {
        const char* code = byAddr(m_geoip, ip.c_str());
        strcpy(out, code);
    }
}

bool RestrictGeolocation::isAllow(std::string account, std::string ip)
{
    // ORIG 0x08170b2c：GeoIP 空实例→log+true；否则解析国家码：
    //  - 允许表命中 → log Success → true；
    //  - 未命中且 server_group==3（Global）→ log Success Global → true；
    //  - 其余 → 记 SIG_SAVE_GEO_REJECT(0x11b,msg=-1) 入 DB 队列 → false。
    if (m_geoip == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x44, 5);
        trace("[Taiwan, GeoIP] Geo instance is null.");
        return true;
    }

    char countryCode[0x10];
    std::memset(countryCode, 0, sizeof(countryCode));
    GeoIPCountryByAddrFn byAddr = get_geoip_country_by_addr();
    if (byAddr && byAddr(m_geoip, ip.c_str()) != 0)
    {
        const char* code = byAddr(m_geoip, ip.c_str());
        strcpy(countryCode, code);
    }

    if (isAllowCountryCode(countryCode))
    {
        cMyTrace okTrace(__PRETTY_FUNCTION__, 0x4e, 0);
        okTrace("[Taiwan, GeoIP] Success Account:%s, IP:%s, CountryCode:%s",
                account.c_str(), ip.c_str(), countryCode);
        return true;
    }

    if (G_CEnvironment()->get_server_group() == 3)
    {
        cMyTrace globalTrace(
            __PRETTY_FUNCTION__, 0x55, 0);
        globalTrace("[Taiwan, GeoIP] Success Global server group. Account:%s, IP:%s, CountryCode:%s",
                    account.c_str(), ip.c_str(), countryCode);
        return true;
    }

    {
        cMyTrace failTrace(__PRETTY_FUNCTION__, 0x5a, 0);
        failTrace("[Taiwan, GeoIP] Fail Account:%s, IP:%s, CountryCode:%s",
                  account.c_str(), ip.c_str(), countryCode);
    }

    Stream* stream = GlobalData::s_stream_pool->Acquire(
        "localtaiwan/System/RestrictGeolocation.cpp", 0x5c);
    CStreamGuard guard(stream, true);
    try
    {
        guard << 0x11b;
        guard << -1;
        SIG_SAVE_GEO_REJECT* sig = guard.GetInBuffer<SIG_SAVE_GEO_REJECT>();
        char* base = sig->m_pad;
        strncpy(base + 0x10, countryCode, 0x10);
        strncpy(base, ip.c_str(), 0x10);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    catch (...)
    {
        throw;
    }
    return false;
}
// ============================================================================
// df_game_r 还原 —— RestrictGeolocation 实现（原 GlobalData.cpp 局部镜像收敛，
// Main 批准：W6_ARAD；声明=权威 RestrictGeolocation.h）。
//
// ORIG 证据：docs/class_func_reports/RestrictGeolocation/*.md（地址见头文件注释）。
// [b28 判词·N] _GLOBAL__I__ZN19RestrictGeolocationC2Ev @0x0817113b NEAR(2)：
// 唯一真差 = sub $0x18 vs $0x8（8/8 其余逐条同）。根因：ORIG __static_init
// @0x081710fb(0x40B) 动态初始化 2 对象——ios_base::Init @0x93f56f2（`<iostream>`
// include 链，__cxa_atexit dtor 0x86da140）+ 全局 std::string("ABCDEFGH…"@0x8ae5878)
// @0x93f56f4（Sso 短优化路径），4.1 按 callee 实帧保守取 0x18；本 TU 现无
// ioinit/string 对象 ⇒ 4.4 继承空壳 callee 塌缩为 0x8（P210 同形机制，但
// _GLOBAL__I 为编译器末置发射、定义不可前移，无源码解）。属 P155/P128 结构
// 噪音族（_GLOBAL__I 内容/TU 归属）；真正缺口在 __static_init（DIFF 40 行：
// ios_base::Init + string("ABCDEFGH…") 对象归属本 TU 未还原），已上报 Main。
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

#include <cstring>

// Taiwan 域 internal_stream（TaiwanQueries.h 权威声明；包类型枚举见该头）。
#include "TaiwanInternal.h"

extern CEnvironment* G_CEnvironment();

// libGeoIP C API（ORIG `nm -u df_game_r` 权威：U GeoIP_new / U GeoIP_delete /
// U GeoIP_country_code_by_addr，调用点为 @plt：closeGeolocation 0x08170a96 →
// GeoIP_delete@plt、initGeolocation 0x08170a5c → GeoIP_new@plt、
// getCountryCode 0x08170fb0/0x08170fd5 → GeoIP_country_code_by_addr@plt）。
// 工具链无 GeoIP.h，按第三方 C 库惯例手写 extern "C" 声明（同 Dispatchers.cpp
// uncompress、APSystemOnlyOrig.cpp compress2 口径；非 §4.5 asm 桥）。
extern "C" {
void* GeoIP_new(int db_type);                       // GEOIP_STANDARD = 0
void GeoIP_delete(void* handle);
const char* GeoIP_country_code_by_addr(void* handle, const char* qry);
}

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
        m_geoip = GeoIP_new(0);
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
    // ORIG 0x08170a88：无空指针判定，直接 GeoIP_delete(this->m_geoip)。
    GeoIP_delete(m_geoip);
}

void RestrictGeolocation::clearAllowCountry()
{
    // ORIG 0x08170b16。
    m_allowCountry.clear();
}

void RestrictGeolocation::addAllowCountry(std::string country)
{
    // ORIG 0x08170a9e：op!= 实参 arg0=-0x14(end)/arg1=-0x18(find) ⇒ 源写
    // `end() != it`（it 命名局部固定 find 先求值，4.4 对 != 右起算 it 再算
    // end 临时，槽序与 ORIG 一致）；test/jne 空 then 岛 + insert 尾 jmp/nop
    // （P223 goto 复现形）。
    boost::unordered_set<std::string>::const_iterator it =
        m_allowCountry.find(country);
    if (m_allowCountry.end() != it)
        goto DONE;
    m_allowCountry.insert(country);
DONE:
    ;
}

bool RestrictGeolocation::isAllowCountryCode(std::string country)
{
    // ORIG 0x0817109a：先 find（结果落 -0x10）后 end（临时 -0xc）——命名局部
    // 固定左到右求值序（4.4 对 operator!= 双临时恒右起）；if-return 保 ORIG
    // 尾部 bool 归一（mov$1/jmp/mov$0）岛（P56）。
    boost::unordered_set<std::string>::const_iterator found =
        m_allowCountry.find(country);
    if (found != m_allowCountry.end())
        return true;
    return false;
}

void RestrictGeolocation::loadData()
{
    // ORIG 0x08170fec：Acquire → Taiwan::internal_stream(guard,4,-1)（报告
    // loadData.md：localtaiwan/System/RestrictGeolocation.cpp 行 0x8b）→
    // clearAllowCountry → DB_Q 投递。Acquire 结果全程寄存器直传（P0：ORIG
    // call 后无槽往返，`mov %eax,0x4(%esp)` 直用）⇒ 写成内联实参。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localtaiwan/System/RestrictGeolocation.cpp", 0x8b),
                       true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(4), -1);
    clearAllowCountry();
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

bool RestrictGeolocation::isAllow(std::string ip)
{
    // ORIG 0x08170ecc：拷贝入参后转查国家码白名单；结果落 %ebx 跨 string D1，
    // 尾部 if-return bool 归一岛（P56）。
    if (isAllowCountryCode(ip))
        return true;
    return false;
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
    if (GeoIP_country_code_by_addr(m_geoip, ip.c_str()) != 0)
    {
        strcpy(out, GeoIP_country_code_by_addr(m_geoip, ip.c_str()));
    }
}

bool RestrictGeolocation::isAllow(std::string account, std::string ip)
{
    // ORIG 0x08170b2c：GeoIP 空实例→log+true；否则解析国家码：
    //  - 允许表命中 → log Success → true；
    //  - 未命中且 server_group==3（Global）→ log Success Global → true；
    //  - 其余 → 记 SIG_SAVE_GEO_REJECT(0x11b,msg=-1) 入 DB 队列 → false。
    // [b28d-N82] fresh 复验 237/241：差值 = 4 个具名 cMyTrace 对象槽方向
    // （ORIG 深→浅 -0x54/-0x44…，4.4 浅→深）与 string 形参拷贝帧 +8
    // （P221/N82 帧策略族；P221 全表达式临时链对本例实测不翻）⇒ 语义等价封顶。
    if (m_geoip == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x44, 5);
        trace("[Taiwan, GeoIP] Geo instance is null.");
        return true;
    }

    char countryCode[0x10];
    std::memset(countryCode, 0, sizeof(countryCode));
    if (GeoIP_country_code_by_addr(m_geoip, ip.c_str()) != 0)
    {
        strcpy(countryCode, GeoIP_country_code_by_addr(m_geoip, ip.c_str()));
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
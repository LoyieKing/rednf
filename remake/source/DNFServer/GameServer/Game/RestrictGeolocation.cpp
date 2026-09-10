// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 RestrictGeolocation/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
// ORIG __static_init @0x081710fb(0x40B) 含 std::ios_base::Init 全局对象
// （_ZStL8__ioinit @0x9404752，C1 + __cxa_atexit dtor 0x86da140）⇒ ORIG 本 TU
// 由 <iostream> 链引入 ioinit。补该 include 复现 __static_init 与 _GLOBAL__I 帧形。
#include <iostream>

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
#include "RestrictGeolocation/GeoIP_delete.inc"
const char* GeoIP_country_code_by_addr(void* handle, const char* qry);
}

#include "RestrictGeolocation/RestrictGeolocation.inc"
{
    // ORIG 0x0817097c：编译器先调基类 ARAD::Singleton<RestrictGeolocation>::Singleton()
    // （报告 call 081713a6）→ m_geoip=0 → unordered_set 以 hash=0xb 构造
    // （报告 call 08171406，四参：this+4, 0xb, &equal_to, &allocator）。体为空。
}

#include "RestrictGeolocation/dtor_RestrictGeolocation.inc"
{
    // ORIG 0x081709fe：closeGeolocation() + 集合析构（成员自动）。
    closeGeolocation();
}

#include "RestrictGeolocation/initGeolocation.inc"
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

#include "RestrictGeolocation/closeGeolocation.inc"
{
    // ORIG 0x08170a88：无空指针判定，直接 GeoIP_delete(this->m_geoip)。
    GeoIP_delete(m_geoip);
}

#include "RestrictGeolocation/clearAllowCountry.inc"
{
    // ORIG 0x08170b16。
    m_allowCountry.clear();
}

#include "RestrictGeolocation/addAllowCountry.inc"
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

#include "RestrictGeolocation/isAllowCountryCode.inc"
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

#include "RestrictGeolocation/loadData.inc"
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

#include "RestrictGeolocation/isAllow.inc"
{
    // ORIG 0x08170ecc：拷贝入参后转查国家码白名单；结果落 %ebx 跨 string D1，
    // 尾部 if-return bool 归一岛（P56）。
    if (isAllowCountryCode(ip))
        return true;
    return false;
}

#include "RestrictGeolocation/getCountryCode.inc"
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

#include "RestrictGeolocation/isAllow_1.inc"
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
        // [P2S5] ORIG 0x08170b50/0x08170b9c 为**链式临时**（gimplify 先算子调用实参、
        // 后算对象 ctor ⇒ 有副作用的实参代码在 ctor 之前）⇒ 具名对象+两次语句改回链式。
        cMyTrace(__PRETTY_FUNCTION__, 0x44, 5)(
            "[Taiwan, GeoIP] Geo instance is null.");
        return true;
    }

    char countryCode[0x10] = { 0, };
    if (GeoIP_country_code_by_addr(m_geoip, ip.c_str()) != 0)
    {
        strcpy(countryCode, GeoIP_country_code_by_addr(m_geoip, ip.c_str()));
    }

    if (isAllowCountryCode(countryCode))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x4e, 0)(
            "[Taiwan, GeoIP] Success Account:%s, IP:%s, CountryCode:%s",
            account.c_str(), ip.c_str(), countryCode);
        return true;
    }

    if (G_CEnvironment()->get_server_group() == 3)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x55, 0)(
            "[Taiwan, GeoIP] Success Global server group. Account:%s, IP:%s, CountryCode:%s",
            account.c_str(), ip.c_str(), countryCode);
        return true;
    }

    {
        cMyTrace(__PRETTY_FUNCTION__, 0x5a, 0)(
            "[Taiwan, GeoIP] Fail Account:%s, IP:%s, CountryCode:%s",
            account.c_str(), ip.c_str(), countryCode);
    }

    // [P2S5] ORIG 0x08170d18：Acquire 结果全程寄存器直传（call 后直接
    // `movl $0x1,0x8(%esp) … lea guard …`，无具名 Stream* 槽）⇒ 内联实参（P0）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localtaiwan/System/RestrictGeolocation.cpp", 0x5c),
                       true);
    // [P2S5] ORIG 0x08170ea5 landing pad 仅 `lea guard; call ~CStreamGuard;
    // call _Unwind_Resume`（无 __cxa_begin_catch/rethrow）⇒ ORIG 无 catch 块，
    // guard 由作用域正常析构；我方伪造的 try/catch(...){throw;} 多出整段 EH 序。
    // [P2S5] ORIG 0x08170dec/0x08170e07 各多一次 `lea guard; call
    // CStreamGuard::operator*()`（0x080c8c46）再 `call operator<<(int)`；
    // 0x08170e22 多一次 `operator->()`（0x080c8c4e）再 GetInBuffer
    // ⇒ 原码为 `*guard << x`（operator* 返回 CStreamGuard&）与
    //   `guard->GetInBuffer<T>()`。本仓 CStreamGuard.h:34 把 operator* 声明为
    //   `CStreamGuard*`（头轴：应为 `CStreamGuard&`，已上报父泳道）⇒ 此处用
    //   显式成员调用等价复现同一机器码。
    guard.operator*()->operator<<(0x11b);
    guard.operator*()->operator<<(-1);
    // [P2S5] ORIG 0x08170e35 只落一个槽（-0xc = sig）：`call GetInBuffer` →
    // `mov %eax,-0xc(%ebp)` → 两次 `mov -0xc(%ebp),…` 直接算 m_pad(+0)/
    // m_pad+0x10 ⇒ 无 `char* base` 具名中转。
    SIG_SAVE_GEO_REJECT* sig =
        guard.operator->()->GetInBuffer<SIG_SAVE_GEO_REJECT>();
    strncpy(sig->m_pad + 0x10, countryCode, 0x10);
    strncpy(sig->m_pad, ip.c_str(), 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return false;
}
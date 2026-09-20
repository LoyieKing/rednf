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
    strncpy(sig->m_rejCCode, countryCode, 0x10);
    strncpy(sig->m_rejIp, ip.c_str(), 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return false;
}
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

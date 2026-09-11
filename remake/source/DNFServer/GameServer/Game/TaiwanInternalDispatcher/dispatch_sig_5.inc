// ===== InterLoadGeolocation::dispatch_sig —— ORIG 0x0816088a =====
// buf==0 → cMyTrace(0xac) 日志 "[Taiwan, GeoIP] SigLoadRestrictPolicy is null." 返回 0xad；
// countryCount>0x2d → 日志(0xb2) 并写回 0x2d；循环 addAllowCountry 每国码（16 字节块），
// 每项 cMyTrace(0xb9,9,true,true) 日志 "GeoIP Allow Country Code : %s"；返回 0。
// [b28d-solo-TaiwanInternalDispatcher] AE（0x2du 无符号比较 + 两处 cMyTrace 临时链）
int InterLoadGeolocation::dispatch_sig(CUser* user, char* buf, int len)
{
    SigLoadGeolocation* sig = reinterpret_cast<SigLoadGeolocation*>(buf);
    if (sig == 0)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0xac, 5);
        tr("[Taiwan, GeoIP] SigLoadRestrictPolicy is null.");
        return 0xad;
    }
    if (sig->countryCount > 0x2du)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xb2, 5)(
            "[Taiwan, GeoIP] Invalid GeoIP country count. (%d)", sig->countryCount);
        sig->countryCount = 0x2d;
    }
    for (int i = 0; i < sig->countryCount; ++i)
    {
        // ORIG：构造临时 string 直接值传（无拷贝），addAllowCountry 参数槽复用
        ARAD::Singleton<RestrictGeolocation>::Get()->addAllowCountry(
            std::string(sig->countryCode[i]));
        cMyTrace(__PRETTY_FUNCTION__, 0xb9, 9, true, true)(
            "GeoIP Allow Country Code : %s", sig->countryCode[i]);
    }
    return 0;
}
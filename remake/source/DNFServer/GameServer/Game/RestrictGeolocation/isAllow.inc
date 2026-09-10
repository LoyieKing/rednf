bool RestrictGeolocation::isAllow(std::string ip)
{
    // ORIG 0x08170ecc：拷贝入参后转查国家码白名单；结果落 %ebx 跨 string D1，
    // 尾部 if-return bool 归一岛（P56）。
    if (isAllowCountryCode(ip))
        return true;
    return false;

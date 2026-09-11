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
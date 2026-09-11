// ----------------------------------------------------------------------------
// 1. ORIG T 0x0864e024 sz 0x91。
// 遍历 m_characList（+0x796e8 vector<_Charac_info>），首个 m_characNo（+0x00，
// 按 uint 比较）等于 characNo 的元素返回其 +0x26 字节（job，movsbl 符号扩展）；
// 未命中返回 -1。
// ----------------------------------------------------------------------------
int CUser::get_character_job(unsigned int characNo) const
{
    std::vector<_Charac_info>::const_iterator it = m_characList.begin();
    std::vector<_Charac_info>::const_iterator mark = m_characList.end();
    while (it != mark)
    {
        if (static_cast<unsigned int>(it->m_characNo) == characNo)
            return static_cast<int>(it->m_field26);  // +0x26 job（movsbl）
        ++it;
    }
    return -1;
}
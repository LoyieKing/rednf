// ----------------------------------------------------------------------------
// 12. ORIG T 0x081b2c94 sz 0x75。m_vec20 表按 m_avatarBonus 查 float。
// ----------------------------------------------------------------------------
float CHARAC_LOAD_MERCENARY::GetAvatarBonus() const
{
    // [b28d-27] ORIG 0x081b2c94：无 bool 槽——`vec.empty() || (signed char)size <
    // (signed char)m_avatarBonus` 直写条件产 eax 寄存器物化（mov $1/$0; test; je），
    // 比较为 8 位有符号（ORIG `cmp %al,%dl; jge`；探针 /tmp/d27_p5.cpp f4 逐条同形，
    // 32 位无符号形产 jae 见 f1）。
    const std::vector<std::pair<int, float> >& vec =
        G_CDataManager()->m_mercenarySystem.m_vec20;
    if (vec.empty() || static_cast<signed char>(vec.size()) <
                           static_cast<signed char>(m_avatarBonus))
        return 0.0f;
    return vec[static_cast<int>(static_cast<char>(m_avatarBonus))].second;
}
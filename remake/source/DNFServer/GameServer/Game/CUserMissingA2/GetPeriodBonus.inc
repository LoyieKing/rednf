// ----------------------------------------------------------------------------
// 13. ORIG T 0x081b2c1e sz 0x75。m_vec14 表按 m_field31 查 float。
// ----------------------------------------------------------------------------
float CHARAC_LOAD_MERCENARY::GetPeriodBonus() const
{
    // [b28d-27] ORIG 0x081b2c1e：同 GetAvatarBonus（8 位有符号比较 + 无 bool 槽）。
    const std::vector<std::pair<int, float> >& vec =
        G_CDataManager()->m_mercenarySystem.m_vec14;
    if (vec.empty() || static_cast<signed char>(vec.size()) <
                           static_cast<signed char>(m_field31))
        return 0.0f;
    return vec[static_cast<int>(static_cast<char>(m_field31))].second;
}
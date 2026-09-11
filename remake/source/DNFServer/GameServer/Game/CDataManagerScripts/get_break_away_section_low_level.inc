// ORIG W 0x822afd6 get_break_away_section_low_level(lv)：返回最后一个
// m_field1<=lv 段的 m_field1；首个 m_field1>lv 即返回 -1（循环内直返）。
int STGrowthPowerData::get_break_away_section_low_level(int lv)
{
    int result = -1;
    for (std::vector<STBreakAwaySection>::const_iterator it = m_vec0.begin();
         it != m_vec0.end(); ++it)
    {
        if ((*it).m_field1 > lv)   // ORIG 用 operator*（de）
            return result;
        result = (*it).m_field1;
    }
    return result;
}
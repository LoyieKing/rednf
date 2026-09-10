STBreakAwaySection STGrowthPowerData::get_break_away_section_data(int level)
{
    STBreakAwaySection out;
    for (std::vector<STBreakAwaySection>::const_iterator it = m_vec0.begin();
         it != m_vec0.end(); ++it)
    {
        if ((*it).m_field1 > level)   // ORIG 用 operator*（de）
            break;
        memmove(&out, &*it, sizeof(STBreakAwaySection));
    }
    return out;

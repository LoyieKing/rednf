// ORIG W 0x822b07e get_break_away_section_data(level)：sret；
// 返回最后一个 elem.m_field1 <= level 的段拷贝（memmove 3 字节；
// 一旦 m_field1>level 即退出，未命中返回清零段）。
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
}
int STGrowthPowerData::get_idx_break_away_section(int lv)
{
    int idx = 0;
    for (std::vector<STBreakAwaySection>::const_iterator it = m_vec0.begin();
         it != m_vec0.end(); ++it, ++idx)
    {
        // ORIG 0x0822b136：de() 每轮一次并把结果落 -0xc 具名槽，两字段从该槽重读
        // （P0 槽往返 ⇒ 原码确有具名引用局部）；两条件为**独立守卫**共享同一
        // continue 落点（`jg`/`jle` 同址 0x822b19d），合并 `&&` 会被 4.4 物化成
        // setcc 双物化（P133/P225 推广）⇒ 拆嵌套 if。
        const STBreakAwaySection& sec = *it;
        if ((unsigned char)sec.m_field1 <= lv)
        {
            if (lv < (unsigned char)sec.m_field2)
                return idx;
        }
    }
    return -1;

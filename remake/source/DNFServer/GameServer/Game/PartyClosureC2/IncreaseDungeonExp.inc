void CDungeonExpStatistic::IncreaseDungeonExp(int level, int exp, int count)
{
    // [b28d-solo-PartyClosureC2] ORIG 0x0860efca：
    //   P227 全表达式临时链（ORIG `mov 0xc(%ebp),%ebx` 在 ctor 前缓存 level）；
    //   P47/P222 极性交换（ORIG insert 路径直落、+= 路径在 else）；
    //   P0 具名引用缓存 `it->second`（ORIG `call operator->; add $4; mov %eax,-0xc`）。
    if (level < 1 || level > 200)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x581, 0)(
            "DUNGEON_EXP_STATISTIC wrong level(%d)", level);
        return;
    }
    std::map<int, stDungeonExp>::iterator it = m_map.find(level);
    if (m_map.end() == it)   // ORIG this=end 临时(-0x38)、arg=it(-0x4c)
    {
        stDungeonExp entry;
        entry.m_exp = exp;
        entry.m_count = count;
        entry.m_clearTimes = 1;
        m_map.insert(std::make_pair(level, entry));
    }
    else
    {
        stDungeonExp& entry = it->second;
        entry.m_exp += exp;
        entry.m_count += count;
        entry.m_clearTimes += 1;
    }

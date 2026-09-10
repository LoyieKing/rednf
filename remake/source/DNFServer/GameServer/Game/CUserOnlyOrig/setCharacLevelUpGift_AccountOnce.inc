void CUser::setCharacLevelUpGift_AccountOnce(SIG_LEVELUP_GIFT_ACCOUNT_ONCE& sig)
{
    // setCharacLevelUpGift_AccountOnce.md @0x8691dca：条目 stride 8 至多 11 条，
    // entry.second(+4)==0 停止；make_pair(entry.first, entry.second) 插入
    // multimap<int,int> m_levelUpGift.m_map(+0x8ec04)。
    // ORIG 0x08691dc0-：每表达式重取 `mov 0xc(%ebp),%eax`+ 二级寻址
    // `mov 0x4(%eax,%edx,8),%eax`（技巧37 免槽；memcpy 调用形违 ORIG，删）。
    // [b28d-22 修法] SigTypes_C.h 建具名 Entry 数组视图后改 sig.m_entries[i] 成员访问
    // （cast 指针算术恒折 shl+add，成员数组才产 ORIG 的 scaled 索引）。
    for (int i = 0; i < 11; ++i)
    {
        if (sig.m_entries[i].m_second == 0)
            break;
        m_levelUpGift.m_map.insert(std::make_pair(
            sig.m_entries[i].m_first,
            sig.m_entries[i].m_second));
    }

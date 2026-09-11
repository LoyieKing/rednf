// ==== ORIG 085df576（const） ====
// [b28d-27] DIFF N82/N86：ORIG 0x085df576；p167 CLEAR（10/10），残差为 pair 数组
// 临时槽与帧分配，语义闭合。
int CDeathMatchBattleMgr::getRanking(CUser** users, int* out) const
{
    int count = 0;
    std::pair<int, int> rank[8];
    memset(out, 0, 0x20);
    for (int i = 0; i <= 7; ++i)
    {
        if (users[i] != 0)
        {
            rank[i] = std::make_pair(i, m_killCount[i]);
            ++count;
        }
        else
        {
            rank[i] = std::make_pair(i, -1);
        }
    }
    std::sort(rank, rank + 8, funDeathMatchKillCntCompare);
    for (int j = 0; j < count; ++j)
    {
        out[j] = rank[j].first;
        // [本波·P227] 全表达式临时链（Main 配方②）：ORIG 0x085df576 在 cMyTrace
        // ctor **之前**右起求 varargs 并以 callee-saved 跨 ctor 保活；具名两语句形
        // 使 4.4 只在 ctor 后求值（帧/槽差 0x10 的来源）。
        cMyTrace(__PRETTY_FUNCTION__, 0x1695, 0)(
            "CDeathMatchBattleMgr::getRanking => Room(%d), Idx(%d), Rank(%d)",
            m_pRoom->get_index(), j, out[j]);
    }
    return count;
}
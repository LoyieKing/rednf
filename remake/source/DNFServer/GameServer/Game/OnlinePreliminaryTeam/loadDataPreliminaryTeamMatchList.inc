// ORIG T 0x0858831a：data 布局 int matchCount@+4 + 每 8 字节记录
// （key@0/value(count)@4）；逐条写入 m_playCountMap[key].m_count。
// [b28d-solo-OnlinePreliminaryTeam] P0/P183：ORIG 仅 d(-0x10)/i(-0xc) 两槽，
// 无 count/key/value 缓存局部（条件每轮重读 d->m_count，RHS 在 operator[] 之后
// 按 scaled 下标重读）；循环条件 ORIG 为 `mov d->m_count,%eax; cmp i,%eax; setg`
// ⇒ 写 `d->m_count > i`（`i < d->m_count` 反产 mov i/cmp count/setl）。
// ORIG 0x0858831a
int COnlinePreliminaryTeam::loadDataPreliminaryTeamMatchList(char* data)
{
    SIG_ONLINE_PRELIMINARY_TEAM_MATCH_LIST* d =
        reinterpret_cast<SIG_ONLINE_PRELIMINARY_TEAM_MATCH_LIST*>(data);
    for (int i = 0; d->m_count > i; ++i)
    {
        m_playCountMap[d->m_entries[i].m_otherTeamId].m_count =
            d->m_entries[i].m_playCount;
    }
    return 1;
}
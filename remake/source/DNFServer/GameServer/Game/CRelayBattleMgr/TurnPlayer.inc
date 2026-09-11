// [b29-R28 实修] ORIG 0x085dead6：150/150（gap=0）⇒ 本 TU 现 ae（near=0）。
// 修法 = P171 形态复原：`guard.put_byte(0)` → `guard.put_byte(count)`，ORIG
// 0x085deb71/0x085deb74 为「movl $0x0,-0x10(%ebp) → mov -0x10(%ebp),%eax →
// mov %eax,0x4(%esp)」= 读 count 槽而非立即数 0（原写法少 1 条且形态错）。
// 备注（非本件判定项）：OURS 槽 guard=-0x28/index=-0x1c vs ORIG guard=-0x24/
// index=-0x28 系 4.4「聚合优先占低槽、标量分区」分区差（N82附）；删具名 index
// 改内联实参已实测反劣（破坏 ORIG get_index 调用序 + 编译失败）⇒ 保留。
int CRelayBattleMgr::TurnPlayer()
{
    if (m_pRoom->IsEndPvpBattle())
    {
        return 1;
    }
    PacketGuard guard;
    int index;   // ORIG -0x28（比 guard -0x24 深；4.4 把聚合放最深 ⇒ NEAR 槽互换，N82附）
    int seat;
    int team;
    int count;
    int i;
    guard.put_header(0, 0x70);
    guard.put_byte(2);
    for (team = 1; team < 3; ++team)
    {
        seat = GetNextPlayer(team);
        guard.put_byte(seat);
        index = guard.get_index();
        count = 0;
        guard.put_byte(count);   // ORIG 0x085deb71/0x085deb74：读 count 槽（非立即数）
        if (m_changeSeqCount[team] > 0)
        {
            for (i = 0; i < 8; ++i)
            {
                if (m_battles[i].m_requestFight &&
                    m_battles[i].m_team == team)
                {
                    guard.put_byte(i);
                    guard.put_short(m_battles[i].m_seq);
                    ++count;
                    m_battles[i].m_requestFight = 0;
                }
            }
        }
        guard.put_byte(index, count);
    }
    guard.finalize(true);
    m_pRoom->send_to_pvp(guard);
    ResetChangeSequenceCount();
    return 1;
}
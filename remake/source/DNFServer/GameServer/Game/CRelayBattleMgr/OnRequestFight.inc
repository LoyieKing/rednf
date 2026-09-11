// ORIG 0x085de18c：条件读形 `movzbl 0x7(%eax,%ecx,1),%eax; xor $0x1,%eax;
// test %al,%al; je`。旧判词 N96+N18a（"寄存器物化无绑定不可达"）已作废——
// 正解 = 字段 m_requestFight **bool 化**（P248 档②）+ 裸 if(!f)，零布局影响。
void CRelayBattleMgr::OnRequestFight(int seat)
{
    if (seat < 0)
        return;
    if (seat > 7)
        return;
    // [b29 P248档②] 字段 bool 化后裸形即复现 ORIG `movzbl; xor$1; test`（P0 合规）。
    if (!m_battles[seat].m_requestFight)
    {
        m_battles[seat].m_requestFight = 1;
        IncreaseChangeSequenceCount(m_battles[seat].m_team);
    }
    else
    {
        m_battles[seat].m_requestFight = 0;
        DecreaseChangeSequenceCount(m_battles[seat].m_team);
    }
    SendRequestFight(seat);
}